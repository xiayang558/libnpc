#include "array.h"
#include "linalg.h"
#include <math.h>

/**
 * cholesky — Cholesky decomposition (like numpy.linalg.cholesky)
 *
 * Decomposes a symmetric positive-definite matrix A into L * L^T,
 * where L is lower triangular. Uses the Cholesky-Banachiewicz algorithm.
 *
 * @param arr  Input matrix (2D square, supports FLOAT32 / FLOAT64)
 * @return     Lower triangular matrix L (same dtype as input), NULL on failure
 */
Array* cholesky(Array *arr) {
    if (arr == NULL || arr->ndim != 2) {
        fprintf(stderr, "cholesky: input must be a 2D matrix\n");
        return NULL;
    }
    if (arr->shape[0] != arr->shape[1]) {
        fprintf(stderr, "cholesky: input must be a square matrix\n");
        return NULL;
    }
    if (arr->dtype != FLOAT32 && arr->dtype != FLOAT64) {
        fprintf(stderr, "cholesky: only FLOAT32 and FLOAT64 supported\n");
        return NULL;
    }

    int n = arr->shape[0];
    size_t elem_size = dtype_size(arr->dtype);

    /* Create result matrix L, initialize to zero */
    Array *L = create_array((int[]){n, n}, 2, arr->dtype);
    if (L == NULL) return NULL;
    memset(L->data, 0, n * n * elem_size);

    /* Copy input matrix (the algorithm reads original values) */
    Array *A_copy = copy_array(arr);
    if (A_copy == NULL) {
        free_array(L);
        return NULL;
    }

    if (arr->dtype == FLOAT64) {
        double *a = (double*)A_copy->data;
        double *l = (double*)L->data;
        for (int i = 0; i < n; i++) {
            for (int j = 0; j <= i; j++) {
                double sum = a[i * n + j];
                for (int k = 0; k < j; k++)
                    sum -= l[i * n + k] * l[j * n + k];
                if (i == j) {
                    if (sum <= 0) {
                        fprintf(stderr, "cholesky: matrix is not positive definite\n");
                        free_array(A_copy); free_array(L); return NULL;
                    }
                    l[i * n + i] = sqrt(sum);
                } else {
                    l[i * n + j] = sum / l[j * n + j];
                }
            }
        }
    } else {
        float *a = (float*)A_copy->data;
        float *l = (float*)L->data;
        for (int i = 0; i < n; i++) {
            for (int j = 0; j <= i; j++) {
                float sum = a[i * n + j];
                for (int k = 0; k < j; k++)
                    sum -= l[i * n + k] * l[j * n + k];
                if (i == j) {
                    if (sum <= 0) {
                        fprintf(stderr, "cholesky: matrix is not positive definite\n");
                        free_array(A_copy); free_array(L); return NULL;
                    }
                    l[i * n + i] = sqrtf(sum);
                } else {
                    l[i * n + j] = sum / l[j * n + j];
                }
            }
        }
    }

    free_array(A_copy);
    return L;
}
