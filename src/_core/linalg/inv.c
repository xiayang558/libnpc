#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "array.h"

#ifndef USE_LAPACK

Array* inv(Array *a) {
    if (a == NULL) {
        fprintf(stderr, "inv: NULL array argument\n");
        return NULL;
    }
    if (a->ndim != 2 || a->shape[0] != a->shape[1]) {
        fprintf(stderr, "inv: input must be a square matrix\n");
        return NULL;
    }
    int n = a->shape[0];
    // determine working type: real goes to FLOAT64, complex goes to COMPLEX128
    int is_complex = (a->dtype == COMPLEX64 || a->dtype == COMPLEX128);
    DataType work_dtype = is_complex ? COMPLEX128 : FLOAT64;
    // Convert to working type
    Array *work = astype(a, work_dtype);
    if (!work) return NULL;
    size_t elem_sz = dtype_size(work_dtype);
    char *data = (char*)work->data;

    // build augmented matrix [A | I] (n x 2n)
    int aug_cols = 2 * n;
    int aug_shape[2] = {n, aug_cols};
    Array *aug = create_array(aug_shape, 2, work_dtype);
    if (!aug) { free_array(work); return NULL; }
    char *aug_data = (char*)aug->data;
    // copy A to left side, set right side as identity matrix
    for (int i = 0; i < n; ++i) {
        memcpy(aug_data + i * aug_cols * elem_sz, data + i * n * elem_sz, n * elem_sz);
        for (int j = 0; j < n; ++j) {
            void *ptr = aug_data + i * aug_cols * elem_sz + (n + j) * elem_sz;
            if (is_complex) {
                complex double *p = (complex double*)ptr;
                *p = (i == j) ? 1.0 + 0.0*I : 0.0 + 0.0*I;
            } else {
                double *p = (double*)ptr;
                *p = (i == j) ? 1.0 : 0.0;
            }
        }
    }

    double eps = 1e-12;
    // Gauss-Jordan elimination
    for (int col = 0; col < n; ++col) {
        // find pivot row (largest absolute value in column)
        int pivot = col;
        double max_abs = 0.0;
        for (int row = col; row < n; ++row) {
            double abs_val;
            if (is_complex) {
                complex double val = *(complex double*)(aug_data + row * aug_cols * elem_sz + col * elem_sz);
                abs_val = cabs(val);
            } else {
                abs_val = fabs(*(double*)(aug_data + row * aug_cols * elem_sz + col * elem_sz));
            }
            if (abs_val > max_abs) {
                max_abs = abs_val;
                pivot = row;
            }
        }
        if (max_abs < eps) {
            free_array(work);
            free_array(aug);
            fprintf(stderr, "inv: singular matrix\n");
            return NULL;
        }
        // swap current row and pivot row
        if (pivot != col) {
            char *row_col = aug_data + col * aug_cols * elem_sz;
            char *row_pivot = aug_data + pivot * aug_cols * elem_sz;
            char *tmp = malloc(aug_cols * elem_sz);
            if (!tmp) {
                free_array(work); free_array(aug);
                return NULL;
            }
            memcpy(tmp, row_col, aug_cols * elem_sz);
            memcpy(row_col, row_pivot, aug_cols * elem_sz);
            memcpy(row_pivot, tmp, aug_cols * elem_sz);
            free(tmp);
        }
        // normalize pivot row
        if (is_complex) {
            complex double diag = *(complex double*)(aug_data + col * aug_cols * elem_sz + col * elem_sz);
            for (int j = col; j < aug_cols; ++j) {
                complex double *ptr = (complex double*)(aug_data + col * aug_cols * elem_sz + j * elem_sz);
                *ptr /= diag;
            }
        } else {
            double diag = *(double*)(aug_data + col * aug_cols * elem_sz + col * elem_sz);
            for (int j = col; j < aug_cols; ++j) {
                double *ptr = (double*)(aug_data + col * aug_cols * elem_sz + j * elem_sz);
                *ptr /= diag;
            }
        }
        // eliminate other rows
        for (int row = 0; row < n; ++row) {
            if (row != col) {
                if (is_complex) {
                    complex double factor = *(complex double*)(aug_data + row * aug_cols * elem_sz + col * elem_sz);
                    for (int j = col; j < aug_cols; ++j) {
                        complex double *dst = (complex double*)(aug_data + row * aug_cols * elem_sz + j * elem_sz);
                        complex double *src = (complex double*)(aug_data + col * aug_cols * elem_sz + j * elem_sz);
                        *dst -= factor * (*src);
                    }
                } else {
                    double factor = *(double*)(aug_data + row * aug_cols * elem_sz + col * elem_sz);
                    for (int j = col; j < aug_cols; ++j) {
                        double *dst = (double*)(aug_data + row * aug_cols * elem_sz + j * elem_sz);
                        double *src = (double*)(aug_data + col * aug_cols * elem_sz + j * elem_sz);
                        *dst -= factor * (*src);
                    }
                }
            }
        }
    }

    // extract inverse matrix (right side n columns)
    int inv_shape[2] = {n, n};
    Array *inv_mat = create_array(inv_shape, 2, work_dtype);
    if (!inv_mat) {
        free_array(work);
        free_array(aug);
        return NULL;
    }
    char *inv_data = (char*)inv_mat->data;
    for (int i = 0; i < n; ++i) {
        memcpy(inv_data + i * n * elem_sz,
               aug_data + i * aug_cols * elem_sz + n * elem_sz,
               n * elem_sz);
    }
    free_array(work);
    free_array(aug);
    return inv_mat;
}

#else

// LAPACK function declarations (Fortran naming)
void dgetrf_(int *m, int *n, double *a, int *lda, int *ipiv, int *info);
void dgetri_(int *n, double *a, int *lda, int *ipiv, double *work, int *lwork, int *info);
void zgetrf_(int *m, int *n, complex double *a, int *lda, int *ipiv, int *info);
void zgetri_(int *n, complex double *a, int *lda, int *ipiv, complex double *work, int *lwork, int *info);

Array* inv(Array *a) {
    if (a == NULL || a->ndim != 2 || a->shape[0] != a->shape[1]) {
        fprintf(stderr, "inv: invalid input\n");
        return NULL;
    }
    int n = a->shape[0];
    int is_complex = (a->dtype == COMPLEX64 || a->dtype == COMPLEX128);
    // Convert to working type
    Array *work_arr = astype(a, is_complex ? COMPLEX128 : FLOAT64);
    if (!work_arr) return NULL;
    size_t elem_sz = dtype_size(work_arr->dtype);
    // allocate column-major matrix (LAPACK format)
    void *a_col = malloc(n * n * elem_sz);
    if (!a_col) { free_array(work_arr); return NULL; }
    // row-major -> column-major
    if (is_complex) {
        complex double *src = (complex double*)work_arr->data;
        complex double *dst = (complex double*)a_col;
        for (int i = 0; i < n; ++i)
            for (int j = 0; j < n; ++j)
                dst[j * n + i] = src[i * n + j];
    } else {
        double *src = (double*)work_arr->data;
        double *dst = (double*)a_col;
        for (int i = 0; i < n; ++i)
            for (int j = 0; j < n; ++j)
                dst[j * n + i] = src[i * n + j];
    }
    free_array(work_arr);

    int *ipiv = (int*)malloc(n * sizeof(int));
    if (!ipiv) { free(a_col); return NULL; }
    int info;

    if (is_complex) {
        zgetrf_(&n, &n, (complex double*)a_col, &n, ipiv, &info);
        if (info != 0) {
            fprintf(stderr, "inv: LU factorization failed\n");
            free(ipiv); free(a_col);
            return NULL;
        }
        int lwork = -1;
        complex double wkopt;
        zgetri_(&n, (complex double*)a_col, &n, ipiv, &wkopt, &lwork, &info);
        lwork = (int)creal(wkopt);
        complex double *work = (complex double*)malloc(lwork * sizeof(complex double));
        if (!work) { free(ipiv); free(a_col); return NULL; }
        zgetri_(&n, (complex double*)a_col, &n, ipiv, work, &lwork, &info);
        free(work);
        if (info != 0) {
            fprintf(stderr, "inv: inversion failed\n");
            free(ipiv); free(a_col);
            return NULL;
        }
    } else {
        dgetrf_(&n, &n, (double*)a_col, &n, ipiv, &info);
        if (info != 0) {
            fprintf(stderr, "inv: LU factorization failed\n");
            free(ipiv); free(a_col);
            return NULL;
        }
        int lwork = -1;
        double wkopt;
        dgetri_(&n, (double*)a_col, &n, ipiv, &wkopt, &lwork, &info);
        lwork = (int)wkopt;
        double *work = (double*)malloc(lwork * sizeof(double));
        if (!work) { free(ipiv); free(a_col); return NULL; }
        dgetri_(&n, (double*)a_col, &n, ipiv, work, &lwork, &info);
        free(work);
        if (info != 0) {
            fprintf(stderr, "inv: inversion failed\n");
            free(ipiv); free(a_col);
            return NULL;
        }
    }
    free(ipiv);

    // create result array, explicitly set dtype
    DataType out_dtype = is_complex ? COMPLEX128 : FLOAT64;
    Array *inv_mat = create_array((int[]){n, n}, 2, out_dtype);
    if (!inv_mat) { free(a_col); return NULL; }
    // column-major -> row-major
    if (is_complex) {
        complex double *src = (complex double*)a_col;
        complex double *dst = (complex double*)inv_mat->data;
        for (int i = 0; i < n; ++i)
            for (int j = 0; j < n; ++j)
                dst[i * n + j] = src[j * n + i];
    } else {
        double *src = (double*)a_col;
        double *dst = (double*)inv_mat->data;
        for (int i = 0; i < n; ++i)
            for (int j = 0; j < n; ++j)
                dst[i * n + j] = src[j * n + i];
    }
    free(a_col);
    return inv_mat;
}

#endif