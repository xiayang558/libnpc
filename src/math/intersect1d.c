#include <stdlib.h>
#include <string.h>
#include "array.h"
#include "shape.h"

/* qsort comparison for doubles */
static int cmp_dbl(const void *a, const void *b) {
    double da = *(const double*)a;
    double db = *(const double*)b;
    return (da > db) - (da < db);
}

/* Lexicographic comparison for complex doubles: by real first, then imag */
static inline int cplx_lt(complex double a, complex double b) {
    double ra = creal(a), rb = creal(b);
    if (ra != rb) return ra < rb;
    return cimag(a) < cimag(b);
}
static int cmp_cplx(const void *a, const void *b) {
    complex double ca = *(const complex double*)a;
    complex double cb = *(const complex double*)b;
    if (ca == cb) return 0;
    return cplx_lt(ca, cb) ? -1 : 1;
}

/**
 * intersect1d — find sorted unique intersection of two arrays (like numpy.intersect1d)
 *
 * Flattens both arrays, sorts and deduplicates (unless assume_unique),
 * then returns sorted unique values present in both.
 * Real inputs → FLOAT64 output; complex inputs → COMPLEX128 output.
 *
 * @param arr1          First input array (any shape)
 * @param arr2          Second input array (any shape)
 * @param assume_unique If non-zero, skip deduplication step
 * @return 1D array of sorted unique intersecting values
 */
Array* intersect1d(Array *arr1, Array *arr2, int assume_unique) {
    if (arr1 == NULL || arr2 == NULL) {
        fprintf(stderr, "intersect1d: NULL array argument\n");
        return NULL;
    }

    /* Empty input → return empty 1D array */
    int is_cplx = (arr1->dtype == COMPLEX64 || arr1->dtype == COMPLEX128 ||
                   arr2->dtype == COMPLEX64 || arr2->dtype == COMPLEX128);
    DataType work_dtype = is_cplx ? COMPLEX128 : FLOAT64;
    size_t elem_sz = dtype_size(work_dtype);

    if (arr1->size == 0 || arr2->size == 0) {
        return create_array((int[]){0}, 1, work_dtype);
    }

    /* Flatten and convert to working type */
    Array *flat1 = flatten(arr1);
    Array *flat2 = flatten(arr2);
    if (!flat1 || !flat2) {
        if (flat1) free_array(flat1);
        if (flat2) free_array(flat2);
        return NULL;
    }
    Array *w1 = astype(flat1, work_dtype);
    Array *w2 = astype(flat2, work_dtype);
    free_array(flat1);
    free_array(flat2);
    if (!w1 || !w2) {
        if (w1) free_array(w1);
        if (w2) free_array(w2);
        return NULL;
    }

    int n1 = w1->size;
    int n2 = w2->size;
    char *data1 = (char*)w1->data;
    char *data2 = (char*)w2->data;

    if (is_cplx) {
        /* ─── Complex path ─── */
        qsort(data1, n1, elem_sz, cmp_cplx);
        qsort(data2, n2, elem_sz, cmp_cplx);

        if (!assume_unique) {
            int u1 = 1;
            for (int i = 1; i < n1; i++) {
                complex double *cur  = (complex double*)(data1 + i * elem_sz);
                complex double *prev = (complex double*)(data1 + (i-1) * elem_sz);
                if (*cur != *prev)
                    *(complex double*)(data1 + u1 * elem_sz) = *cur, u1++;
            }
            n1 = u1;
            int u2 = 1;
            for (int i = 1; i < n2; i++) {
                complex double *cur  = (complex double*)(data2 + i * elem_sz);
                complex double *prev = (complex double*)(data2 + (i-1) * elem_sz);
                if (*cur != *prev)
                    *(complex double*)(data2 + u2 * elem_sz) = *cur, u2++;
            }
            n2 = u2;
        }

        int max_out = (n1 < n2) ? n1 : n2;
        complex double *tmp = malloc(max_out * sizeof(complex double));
        if (!tmp) { free_array(w1); free_array(w2); return NULL; }

        int out_len = 0;
        int i = 0, j = 0;
        while (i < n1 && j < n2) {
            complex double a = *(complex double*)(data1 + i * elem_sz);
            complex double b = *(complex double*)(data2 + j * elem_sz);
            if (cplx_lt(a, b)) {
                i++;
            } else if (cplx_lt(b, a)) {
                j++;
            } else {
                tmp[out_len++] = a;
                i++; j++;
            }
        }

        Array *result;
        if (out_len == 0) {
            result = create_array((int[]){0}, 1, work_dtype);
        } else {
            result = create_array((int[]){out_len}, 1, work_dtype);
        }
        if (!result) { free(tmp); free_array(w1); free_array(w2); return NULL; }
        if (out_len > 0) memcpy(result->data, tmp, out_len * sizeof(complex double));
        free(tmp);
        free_array(w1); free_array(w2);
        return result;
    } else {
        /* ─── Real path ─── */
        qsort(data1, n1, elem_sz, cmp_dbl);
        qsort(data2, n2, elem_sz, cmp_dbl);

        if (!assume_unique) {
            int u1 = 1;
            for (int i = 1; i < n1; i++) {
                double *cur  = (double*)(data1 + i * elem_sz);
                double *prev = (double*)(data1 + (i-1) * elem_sz);
                if (*cur != *prev)
                    *(double*)(data1 + u1 * elem_sz) = *cur, u1++;
            }
            n1 = u1;
            int u2 = 1;
            for (int i = 1; i < n2; i++) {
                double *cur  = (double*)(data2 + i * elem_sz);
                double *prev = (double*)(data2 + (i-1) * elem_sz);
                if (*cur != *prev)
                    *(double*)(data2 + u2 * elem_sz) = *cur, u2++;
            }
            n2 = u2;
        }

        int max_out = (n1 < n2) ? n1 : n2;
        double *tmp = malloc(max_out * sizeof(double));
        if (!tmp) { free_array(w1); free_array(w2); return NULL; }

        int out_len = 0;
        int i = 0, j = 0;
        while (i < n1 && j < n2) {
            double a = *(double*)(data1 + i * elem_sz);
            double b = *(double*)(data2 + j * elem_sz);
            if (a < b) {
                i++;
            } else if (a > b) {
                j++;
            } else {
                tmp[out_len++] = a;
                i++; j++;
            }
        }

        Array *result;
        if (out_len == 0) {
            result = create_array((int[]){0}, 1, work_dtype);
        } else {
            result = create_array((int[]){out_len}, 1, work_dtype);
        }
        if (!result) { free(tmp); free_array(w1); free_array(w2); return NULL; }
        if (out_len > 0) memcpy(result->data, tmp, out_len * sizeof(double));
        free(tmp);
        free_array(w1); free_array(w2);
        return result;
    }
}
