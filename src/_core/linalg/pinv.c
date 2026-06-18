#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "array.h"
#include "shape.h"

/* Non-LAPACK version: regularized least squares pseudoinverse */
static Array* pinv_no_lapack(Array *a, double rcond) {
    if (a == NULL || a->ndim != 2) {
        fprintf(stderr, "pinv: input must be a 2D matrix\n");
        return NULL;
    }
    int m = a->shape[0];
    int n = a->shape[1];
    (void)rcond; /* Ignore user rcond, use fixed regularization */

    /* Convert to double precision */
    Array *A = astype(a, FLOAT64);
    if (!A) return NULL;
    Array *At = transpose(A);
    Array *pinv_mat = NULL;
    double lambda = 1e-10;  /* Regularization coefficient */

    if (m >= n) {
        Array *AtA = dot(At, A);
        if (!AtA) goto cleanup;
        for (int i = 0; i < n; ++i) {
            double *diag = (double*)AtA->data + i * n + i;
            *diag += lambda;
        }
        Array *AtA_inv = inv(AtA);
        if (AtA_inv) {
            pinv_mat = dot(AtA_inv, At);
            free_array(AtA_inv);
        }
        free_array(AtA);
    } else {
        Array *AAt = dot(A, At);
        if (!AAt) goto cleanup;
        for (int i = 0; i < m; ++i) {
            double *diag = (double*)AAt->data + i * m + i;
            *diag += lambda;
        }
        Array *AAt_inv = inv(AAt);
        if (AAt_inv) {
            pinv_mat = dot(At, AAt_inv);
            free_array(AAt_inv);
        }
        free_array(AAt);
    }
cleanup:
    free_array(At);
    free_array(A);
    return pinv_mat;
}

/* LAPACK version: SVD-based pseudoinverse with rcond thresholding */

#ifdef USE_LAPACK
#include <complex.h>

/* LAPACK function declarations */
void dgesvd_(char *jobu, char *jobvt, int *m, int *n,
             double *a, int *lda, double *s,
             double *u, int *ldu, double *vt, int *ldvt,
             double *work, int *lwork, int *info);
void zgesvd_(char *jobu, char *jobvt, int *m, int *n,
             complex double *a, int *lda, double *s,
             complex double *u, int *ldu, complex double *vt, int *ldvt,
             complex double *work, int *lwork, double *rwork, int *info);

static Array* pinv_lapack(Array *a, double rcond) {
    if (a == NULL || a->ndim != 2) {
        fprintf(stderr, "pinv: input must be a 2D matrix\n");
        return NULL;
    }
    int m = a->shape[0];
    int n = a->shape[1];
    int is_complex = (a->dtype == COMPLEX64 || a->dtype == COMPLEX128);
    int min_dim = (m < n) ? m : n;

    /* Convert to working type */
    Array *a_work = astype(a, is_complex ? COMPLEX128 : FLOAT64);
    if (!a_work) return NULL;

    /* Convert input matrix from row-major to column-major (LAPACK requires) */
    size_t elem_sz = dtype_size(a_work->dtype);
    void *a_col = malloc(m * n * elem_sz);
    if (!a_col) { free_array(a_work); return NULL; }
    if (is_complex) {
        complex double *src = (complex double*)a_work->data;
        complex double *dst = (complex double*)a_col;
        for (int i = 0; i < m; ++i)
            for (int j = 0; j < n; ++j)
                dst[j * m + i] = src[i * n + j];
    } else {
        double *src = (double*)a_work->data;
        double *dst = (double*)a_col;
        for (int i = 0; i < m; ++i)
            for (int j = 0; j < n; ++j)
                dst[j * m + i] = src[i * n + j];
    }

    /* Allocate singular value array */
    double *s = (double*)malloc(min_dim * sizeof(double));
    if (!s) { free(a_col); free_array(a_work); return NULL; }

    /* Allocate U and VT (column-major) */
    void *U = malloc(m * m * elem_sz);
    void *VT = malloc(n * n * elem_sz);
    if (!U || !VT) {
        free(s); free(a_col); free(U); free(VT); free_array(a_work);
        return NULL;
    }

    int info;
    if (is_complex) {
        char jobu = 'A', jobvt = 'A';
        int lwork = -1;
        complex double wkopt;
        double *rwork = (double*)malloc(5 * min_dim * sizeof(double));
        if (!rwork) {
            free(s); free(a_col); free(U); free(VT); free_array(a_work);
            return NULL;
        }
        zgesvd_(&jobu, &jobvt, &m, &n, (complex double*)a_col, &m, s,
                (complex double*)U, &m, (complex double*)VT, &n,
                &wkopt, &lwork, rwork, &info);
        lwork = (int)creal(wkopt);
        complex double *work = (complex double*)malloc(lwork * sizeof(complex double));
        if (!work) {
            free(rwork); free(s); free(a_col); free(U); free(VT); free_array(a_work);
            return NULL;
        }
        zgesvd_(&jobu, &jobvt, &m, &n, (complex double*)a_col, &m, s,
                (complex double*)U, &m, (complex double*)VT, &n,
                work, &lwork, rwork, &info);
        free(work); free(rwork);
    } else {
        char jobu = 'A', jobvt = 'A';
        int lwork = -1;
        double wkopt;
        dgesvd_(&jobu, &jobvt, &m, &n, (double*)a_col, &m, s,
                (double*)U, &m, (double*)VT, &n,
                &wkopt, &lwork, &info);
        lwork = (int)wkopt;
        double *work = (double*)malloc(lwork * sizeof(double));
        if (!work) {
            free(s); free(a_col); free(U); free(VT); free_array(a_work);
            return NULL;
        }
        dgesvd_(&jobu, &jobvt, &m, &n, (double*)a_col, &m, s,
                (double*)U, &m, (double*)VT, &n,
                work, &lwork, &info);
        free(work);
    }
    free(a_col);
    if (info != 0) {
        fprintf(stderr, "pinv: SVD failed with error %d\n", info);
        free(s); free(U); free(VT); free_array(a_work);
        return NULL;
    }

    /* Compute cutoff threshold */
    double eps = 1e-15;
    double max_s = s[0];
    for (int i = 1; i < min_dim; ++i) if (s[i] > max_s) max_s = s[i];
    double thr = (rcond > 0) ? rcond : max_s * eps * (m > n ? m : n);

    /* Convert U and V to row-major for easier computation */
    if (is_complex) {
        complex double *U_col = (complex double*)U;
        complex double *VT_col = (complex double*)VT;
        complex double *V_row = (complex double*)malloc(n * min_dim * sizeof(complex double));
        complex double *U_row = (complex double*)malloc(m * min_dim * sizeof(complex double));
        if (!V_row || !U_row) {
            free(s); free(U); free(VT); free_array(a_work);
            return NULL;
        }
        for (int i = 0; i < m; ++i)
            for (int k = 0; k < min_dim; ++k)
                U_row[i * min_dim + k] = U_col[k * m + i];
        for (int i = 0; i < n; ++i)
            for (int k = 0; k < min_dim; ++k)
                V_row[i * min_dim + k] = VT_col[k * n + i];

        Array *pinv_mat = create_array((int[]){n, m}, 2, COMPLEX128);
        if (!pinv_mat) { free(V_row); free(U_row); free(s); free(U); free(VT); free_array(a_work); return NULL; }
        complex double *res = (complex double*)pinv_mat->data;
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < m; ++j) {
                complex double sum = 0.0;
                for (int k = 0; k < min_dim; ++k) {
                    if (s[k] > thr) {
                        sum += V_row[i * min_dim + k] * (1.0 / s[k]) * conj(U_row[j * min_dim + k]);
                    }
                }
                res[i * m + j] = sum;
            }
        }
        free(V_row); free(U_row);
        free(s); free(U); free(VT); free_array(a_work);
        return pinv_mat;
    } else {
        double *U_col = (double*)U;
        double *VT_col = (double*)VT;
        double *V_row = (double*)malloc(n * min_dim * sizeof(double));
        double *U_row = (double*)malloc(m * min_dim * sizeof(double));
        if (!V_row || !U_row) {
            free(s); free(U); free(VT); free_array(a_work);
            return NULL;
        }
        for (int i = 0; i < m; ++i)
            for (int k = 0; k < min_dim; ++k)
                U_row[i * min_dim + k] = U_col[k * m + i];
        for (int i = 0; i < n; ++i)
            for (int k = 0; k < min_dim; ++k)
                V_row[i * min_dim + k] = VT_col[k * n + i];

        Array *pinv_mat = create_array((int[]){n, m}, 2, FLOAT64);
        if (!pinv_mat) { free(V_row); free(U_row); free(s); free(U); free(VT); free_array(a_work); return NULL; }
        double *res = (double*)pinv_mat->data;
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < m; ++j) {
                double sum = 0.0;
                for (int k = 0; k < min_dim; ++k) {
                    if (s[k] > thr) {
                        sum += V_row[i * min_dim + k] * (1.0 / s[k]) * U_row[j * min_dim + k];
                    }
                }
                res[i * m + j] = sum;
            }
        }
        free(V_row); free(U_row);
        free(s); free(U); free(VT); free_array(a_work);
        return pinv_mat;
    }
}
#endif

/* Unified entry point */
Array* pinv(Array *a, double rcond) {
#ifdef USE_LAPACK
    return pinv_lapack(a, rcond);
#else
    return pinv_no_lapack(a, rcond);
#endif
}