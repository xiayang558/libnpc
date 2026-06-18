#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <complex.h>
#include "array.h"
#include "utils.h"
#include "io.h"

/**
 * savetxt  --  Save an array to a text file (like numpy.savetxt)
 *
 * @param filename  output file path
 * @param arr       array to save
 * @param fmt       format string (e.g. "%.18e"), uses "%g" when fmt is NULL
 * @param delimiter column delimiter, uses " " when NULL
 * @param newline   row delimiter, uses "\n" when NULL
 * @param header    file header string, not written when NULL
 * @param footer    file footer string, not written when NULL
 * @param comments  comment prefix, not written when NULL; default "# "
 * @return 0 success, -1 failed
 */
int savetxt(const char *filename, Array *arr, const char *fmt,
            const char *delimiter, const char *newline,
            const char *header, const char *footer, const char *comments) {
    if (filename == NULL || arr == NULL) {
        fprintf(stderr, "savetxt: NULL argument\n");
        return -1;
    }

    /* Defaults */
    if (fmt == NULL) fmt = "%g";
    if (delimiter == NULL) delimiter = " ";
    if (newline == NULL) newline = "\n";
    if (comments == NULL) comments = "# ";

    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        fprintf(stderr, "savetxt: cannot open file '%s'\n", filename);
        return -1;
    }

    /* Flatten extra dims to get rows × cols */
    int ndim = arr->ndim;
    int ncols, nrows;

    if (ndim == 1) {
        ncols = 1;
        nrows = arr->size;
    } else {
        ncols = arr->shape[ndim - 1];
        nrows = arr->size / ncols;
    }

    /* Write header */
    if (header != NULL) {
        fputs(comments, fp);
        fputs(header, fp);
        fputs(newline, fp);
    }

    /* Write data */
    size_t elem_sz = dtype_size(arr->dtype);
    char *data = (char*)arr->data;
    int is_complex = (arr->dtype == COMPLEX64 || arr->dtype == COMPLEX128);

    for (int row = 0; row < nrows; row++) {
        for (int col = 0; col < ncols; col++) {
            int idx = row * ncols + col;
            void *ptr = data + idx * elem_sz;

            if (is_complex) {
                complex double z;
                if (arr->dtype == COMPLEX64)
                    z = *(complex float*)ptr;
                else
                    z = *(complex double*)ptr;
                fprintf(fp, "(");
                fprintf(fp, fmt, creal(z));
                if (cimag(z) >= 0) fputc('+', fp);
                fprintf(fp, fmt, cimag(z));
                fprintf(fp, "j)");
            } else {
                double val = get_value(ptr, arr->dtype);
                /* Integer format specifiers need integer arguments */
                int fmt_len = (int)strlen(fmt);
                char last = (fmt_len > 0) ? fmt[fmt_len - 1] : 0;
                if (last == 'd' || last == 'i' || last == 'u' ||
                    last == 'x' || last == 'X' || last == 'o') {
                    fprintf(fp, fmt, (int64_t)val);
                } else {
                    fprintf(fp, fmt, val);
                }
            }

            if (col < ncols - 1) fputs(delimiter, fp);
        }
        fputs(newline, fp);
    }

    /* Write footer */
    if (footer != NULL) {
        fputs(comments, fp);
        fputs(footer, fp);
        fputs(newline, fp);
    }

    fclose(fp);
    return 0;
}
