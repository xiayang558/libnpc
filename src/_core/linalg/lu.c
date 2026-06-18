#include "array.h"
#include "linalg.h"
#include <math.h>
#include <string.h>

/**
 * LU decomposition  --  Doolittle algorithm (with partial pivoting)
 *
 * Decompose n x n matrix A as P * A = L * U:
 *   P: n x n permutation matrix
 *   L: n x n unit lower triangular matrix (diagonal is 1)
 *   U: n x n upper triangular matrix
 *
 * Supports FLOAT32 / FLOAT64.
 */
LUResult lu(Array *arr) {
    LUResult res = {NULL, NULL, NULL};
    if (arr == NULL || arr->ndim != 2) {
        fprintf(stderr, "lu: input must be a 2D matrix\n");
        return res;
    }
    if (arr->shape[0] != arr->shape[1]) {
        fprintf(stderr, "lu: input must be a square matrix\n");
        return res;
    }
    if (arr->dtype != FLOAT32 && arr->dtype != FLOAT64) {
        fprintf(stderr, "lu: only FLOAT32 and FLOAT64 supported\n");
        return res;
    }

    int n = arr->shape[0];

    /* Use double uniformly for computation */
    double *A = malloc(n * n * sizeof(double));
    double *L = calloc(n * n, sizeof(double));
    double *U = malloc(n * n * sizeof(double));
    double *P = calloc(n * n, sizeof(double));
    if (!A || !L || !U || !P) {
        free(A); free(L); free(U); free(P);
        return res;
    }

    if (arr->dtype == FLOAT64) {
        memcpy(A, arr->data, n * n * sizeof(double));
    } else {
        float *src = (float*)arr->data;
        for (int i = 0; i < n * n; i++) A[i] = (double)src[i];
    }

    /* initialize: U = A, L = I, P = I */
    memcpy(U, A, n * n * sizeof(double));
    for (int i = 0; i < n; i++) {
        L[i * n + i] = 1.0;
        P[i * n + i] = 1.0;
    }

    /* Doolittle algorithm */
    for (int k = 0; k < n; k++) {
        /* Partial pivoting: find row with largest absolute value in column k */
        int pivot = k;
        double max_val = fabs(U[k * n + k]);
        for (int i = k + 1; i < n; i++) {
            double val = fabs(U[i * n + k]);
            if (val > max_val) { max_val = val; pivot = i; }
        }

        if (max_val < 1e-15) {
            fprintf(stderr, "lu: singular matrix (zero pivot at column %d)\n", k);
            free(A); free(L); free(U); free(P);
            return res;
        }

        /* swap U rows */
        if (pivot != k) {
            for (int j = 0; j < n; j++) {
                double tmp_u = U[k * n + j];
                U[k * n + j] = U[pivot * n + j];
                U[pivot * n + j] = tmp_u;
            }
            /* swap P rows */
            for (int j = 0; j < n; j++) {
                double tmp_p = P[k * n + j];
                P[k * n + j] = P[pivot * n + j];
                P[pivot * n + j] = tmp_p;
            }
            /* swap L first k-1 columns */
            for (int j = 0; j < k; j++) {
                double tmp_l = L[k * n + j];
                L[k * n + j] = L[pivot * n + j];
                L[pivot * n + j] = tmp_l;
            }
        }

        /* elimination */
        for (int i = k + 1; i < n; i++) {
            double factor = U[i * n + k] / U[k * n + k];
            L[i * n + k] = factor;
            U[i * n + k] = 0.0;
            for (int j = k + 1; j < n; j++) {
                U[i * n + j] -= factor * U[k * n + j];
            }
        }
    }

    /* build output */
    res.P = create_array((int[]){n, n}, 2, arr->dtype);
    res.L = create_array((int[]){n, n}, 2, arr->dtype);
    res.U = create_array((int[]){n, n}, 2, arr->dtype);

    if (!res.P || !res.L || !res.U) {
        free_array(res.P); free_array(res.L); free_array(res.U);
        res.P = NULL; res.L = NULL; res.U = NULL;
        free(A); free(L); free(U); free(P);
        return res;
    }

    if (arr->dtype == FLOAT64) {
        memcpy(res.P->data, P, n * n * sizeof(double));
        memcpy(res.L->data, L, n * n * sizeof(double));
        memcpy(res.U->data, U, n * n * sizeof(double));
    } else {
        float *pd = (float*)res.P->data;
        float *ld = (float*)res.L->data;
        float *ud = (float*)res.U->data;
        for (int i = 0; i < n * n; i++) { pd[i] = (float)P[i]; }
        for (int i = 0; i < n * n; i++) { ld[i] = (float)L[i]; }
        for (int i = 0; i < n * n; i++) { ud[i] = (float)U[i]; }
    }

    free(A); free(L); free(U); free(P);
    return res;
}
