#include <ctype.h>
#include "array.h"
#include "shape.h"
#include "io.h"
#include "npzfile.h"


// parse one complex string, supports "1.0+2.0j", "3.0-4.0j" and real numbers like "5.0"
static int parse_complex(const char *str, complex double *out) {
    char *end;
    double real = strtod(str, &end);
    if (end == str) return -1;
    // skip whitespace
    while (isspace((unsigned char)*end)) end++;
    if (*end == '+' || *end == '-') {
        char sign = *end;
        double imag = strtod(end + 1, &end);
        if (*end == 'j' || *end == 'J') {
            if (sign == '-') imag = -imag;
            *out = real + imag * I;
            return 0;
        }
    }
    // no imaginary part, treat as a real number
    *out = real + 0.0 * I;
    return 0;
}

// split one line, return real or complex array based on data type
static void* split_line(const char *line, char delim, int *count, DataType intermediate_dtype) {
    int is_complex = (intermediate_dtype == COMPLEX128);
    int cap = 10;
    void *vals;
    if (is_complex) {
        vals = malloc(cap * sizeof(complex double));
    } else {
        vals = malloc(cap * sizeof(double));
    }
    if (!vals) return NULL;
    int n = 0;
    const char *p = line;
    while (*p) {
        while (*p && *p == delim) p++;
        if (!*p) break;
        const char *start = p;
        while (*p && *p != delim) p++;
        char field[256];
        int len = p - start;
        if (len >= 256) len = 255;
        strncpy(field, start, len);
        field[len] = '\0';
        if (is_complex) {
            complex double val;
            if (parse_complex(field, &val) == 0) {
                if (n >= cap) {
                    cap *= 2;
                    vals = realloc(vals, cap * sizeof(complex double));
                    if (!vals) { free(vals); return NULL; }
                }
                ((complex double*)vals)[n++] = val;
            }
        } else {
            double val = strtod(field, NULL);
            if (n >= cap) {
                cap *= 2;
                vals = realloc(vals, cap * sizeof(double));
                if (!vals) { free(vals); return NULL; }
            }
            ((double*)vals)[n++] = val;
        }
    }
    *count = n;
    return vals;
}


/**
 * @brief read numeric array from text file (supports integer, floating point, complex)
 * @param fname       file name
 * @param delimiter   field delimiter (single character, e.g., ',')
 * @param comment     comment character (single character, e.g., '#', set to 0 to disable)
 * @param skiplines   number of lines to skip at the beginning
 * @param max_rows    maximum rows to read (-1 means all)
 * @param usecols     1D integer array, specifying column indices to read (can be NULL)
 * @param ndmin       minimum ndim (0, 1, 2)
 * @param unpack      whether to transpose (1 means transpose)
 * @param dtype       target data type: INT32, FLOAT64, COMPLEX128
 * @return            read array (2D or 1D or scalar)
 */
Array* readtxt(const char *fname, const char *delimiter, char comment,
               int skiplines, int max_rows, Array *usecols, int ndmin, int unpack, DataType dtype) {
    if (fname == NULL) {
        fprintf(stderr, "readtxt: fname is NULL\n");
        return NULL;
    }
    if (skiplines < 0 || max_rows < -1) {
        fprintf(stderr, "readtxt: skiplines or max_rows invalid\n");
        return NULL;
    }
    // determine delimiter
    char delim = (delimiter && *delimiter) ? delimiter[0] : ' ';
    int is_complex = (dtype == COMPLEX64 || dtype == COMPLEX128);
    int is_complex_target = (dtype == COMPLEX64 || dtype == COMPLEX128);
    DataType intermediate_dtype = is_complex_target ? COMPLEX128 : FLOAT64;


    FILE *fp = fopen(fname, "r");
    if (!fp) {
        fprintf(stderr, "readtxt: cannot open file '%s'\n", fname);
        return NULL;
    }

    // handle usecols
    int ncols = 0;
    int *cols = NULL;
    if (usecols != NULL) {
        if (usecols->ndim != 1 || (usecols->dtype != INT32 && usecols->dtype != INT64)) {
            fprintf(stderr, "readtxt: usecols must be 1D integer array\n");
            fclose(fp);
            return NULL;
        }
        ncols = usecols->size;
        cols = malloc(ncols * sizeof(int));
        if (!cols) { fclose(fp); return NULL; }
        memcpy(cols, usecols->data, ncols * sizeof(int));
        for (int i = 0; i < ncols; ++i) {
            if (cols[i] < 0) {
                fprintf(stderr, "readtxt: negative column index\n");
                free(cols); fclose(fp);
                return NULL;
            }
        }
    }

    // skip specified number of lines
    char line[65536];
    for (int i = 0; i < skiplines; ++i) {
        if (!fgets(line, sizeof(line), fp)) break;
    }

    // dynamic storage for data rows
    void **data = NULL;
    int rows = 0;
    int cols_per_row = -1;
    int max_limit = (max_rows == -1) ? 1000000 : max_rows;

    while (rows < max_limit && fgets(line, sizeof(line), fp)) {
        // strip newline
        line[strcspn(line, "\n")] = '\0';
        // handle comment
        if (comment) {
            char *cpos = strchr(line, comment);
            if (cpos) *cpos = '\0';
        }
        // skip empty lines
        if (strlen(line) == 0) continue;
        int cnt;
        void *vals = split_line(line, delim, &cnt, dtype);
        if (!vals) continue;

        if (cols_per_row == -1) {
            if (usecols) {
                // check whether required column indices are valid
                cols_per_row = ncols;
                int max_col = 0;
                for (int i = 0; i < ncols; ++i) {
                    if (cols[i] > max_col) max_col = cols[i];
                }
                if (max_col >= cnt) {
                    fprintf(stderr, "readtxt: column index %d out of range (line %d has %d columns)\n",
                            max_col, rows+1, cnt);
                    free(vals);
                    fclose(fp);
                    for (int r = 0; r < rows; ++r) free(data[r]);
                    free(data);
                    free(cols);
                    return NULL;
                }
            } else {
                cols_per_row = cnt;
            }
        } else {
            int expected = usecols ? ncols : cols_per_row;
            if ((usecols && cnt < (cols[cols_per_row-1]+1)) || (!usecols && cnt != expected)) {
                fprintf(stderr, "readtxt: row %d column count mismatch (expected %d, got %d)\n",
                        rows+1, expected, cnt);
                free(vals);
                continue;
            }
        }

        // extract needed columns
        int out_cols = usecols ? ncols : cols_per_row;
        size_t elem_sz = is_complex ? sizeof(complex double) : sizeof(double);
        void *row_vals = malloc(out_cols * elem_sz);
        if (!row_vals) {
            free(vals);
            fclose(fp);
            for (int r = 0; r < rows; ++r) free(data[r]);
            free(data);
            free(cols);
            return NULL;
        }
        if (usecols) {
            if (is_complex) {
                complex double *src = (complex double*)vals;
                complex double *dst = (complex double*)row_vals;
                for (int i = 0; i < ncols; ++i) {
                    dst[i] = src[cols[i]];
                }
            } else {
                double *src = (double*)vals;
                double *dst = (double*)row_vals;
                for (int i = 0; i < ncols; ++i) {
                    dst[i] = src[cols[i]];
                }
            }
        } else {
            memcpy(row_vals, vals, out_cols * elem_sz);
        }
        free(vals);

        data = realloc(data, (rows + 1) * sizeof(void*));
        if (!data) {
            free(row_vals);
            fclose(fp);
            for (int r = 0; r < rows; ++r) free(data[r]);
            free(data);
            free(cols);
            return NULL;
        }
        data[rows++] = row_vals;
    }
    fclose(fp);
    free(cols);

    if (rows == 0) {
        // empty array
        int shape[2] = {0, 0};
        return create_array(shape, 2, dtype);
    }

    int out_cols = usecols ? ncols : cols_per_row;
    int shape[2] = {rows, out_cols};
    Array *result = create_array(shape, 2, dtype);
    if (!result) {
        for (int r = 0; r < rows; ++r) free(data[r]);
        free(data);
        return NULL;
    }

    // size_t elem_sz_out = dtype_size(dtype);
    // char *res_data = (char*)result->data;
    // if (is_complex) {
    //     complex double *src;
    //     complex double *dst = (complex double*)res_data;
    //     for (int i = 0; i < rows; ++i) {
    //         src = (complex double*)data[i];
    //         memcpy(dst + i * out_cols, src, out_cols * sizeof(complex double));
    //         free(data[i]);
    //     }
    // } else {
    //     double *src;
    //     double *dst = (double*)res_data;
    //     for (int i = 0; i < rows; ++i) {
    //         src = (double*)data[i];
    //         memcpy(dst + i * out_cols, src, out_cols * sizeof(double));
    //         free(data[i]);
    //     }
    // }
    // free(data);

    // convert data
    if (is_complex_target) {
        // complex conversion
        if (dtype == COMPLEX64) {
            complex float *dst = (complex float*)result->data;
            for (int i = 0; i < rows; ++i) {
                complex double *src = (complex double*)data[i];
                for (int j = 0; j < out_cols; ++j) {
                    dst[i * out_cols + j] = (complex float)src[j];
                }
                free(data[i]);
            }
        } else { // COMPLEX128
            char *dst = (char*)result->data;
            size_t out_sz = sizeof(complex double);
            for (int i = 0; i < rows; ++i) {
                memcpy(dst + i * out_cols * out_sz, data[i], out_cols * out_sz);
                free(data[i]);
            }
        }
    } else {
        // real number target (integer or floating point)
        size_t out_elem = dtype_size(dtype);
        char *dst = (char*)result->data;
        for (int i = 0; i < rows; ++i) {
            double *src = (double*)data[i];
            for (int j = 0; j < out_cols; ++j) {
                double val = src[j];
                switch (dtype) {
                    case INT8:   *(int8_t*)dst = (int8_t)round(val); break;
                    case INT16:  *(int16_t*)dst = (int16_t)round(val); break;
                    case INT32:  *(int32_t*)dst = (int32_t)round(val); break;
                    case INT64:  *(int64_t*)dst = (int64_t)round(val); break;
                    case UINT8:  *(uint8_t*)dst = (uint8_t)round(val); break;
                    case UINT16: *(uint16_t*)dst = (uint16_t)round(val); break;
                    case UINT32: *(uint32_t*)dst = (uint32_t)round(val); break;
                    case UINT64: *(uint64_t*)dst = (uint64_t)round(val); break;
                    case FLOAT32: *(float*)dst = (float)val; break;
                    case FLOAT64: *(double*)dst = val; break;
                    default: break;
                }
                dst += out_elem;
            }
            free(data[i]);
        }
    }

    // handle ndmin and unpack
    if (unpack) {
        Array *transposed = transpose(result);
        free_array(result);
        result = transposed;
    }
    if (ndmin > result->ndim) {
        if (ndmin == 1) {
            Array *new = atleast_1d(result);
            free_array(result);
            result = new;
        } else if (ndmin == 2) {
            Array *new = atleast_2d(result);
            free_array(result);
            result = new;
        }
    }
    return result;
}