#include "array.h"
#include "linalg.h"
#include <math.h>

/**
 * QR decomposition  --  Modified Gram-Schmidt algorithm
 *
 * Decompose m x n matrix A as A = Q * R:
 *   Q: m x min(m,n) orthogonal column matrix (Q^T Q = I)
 *   R: min(m,n) x n upper triangular matrix (if m >= n) or row-full upper trapezoidal (if m < n)
 *
 * Supports FLOAT32 / FLOAT64.
 */
QRResult qr(Array *arr) {
    QRResult res = {NULL, NULL};
    if (arr == NULL || arr->ndim != 2) {
        fprintf(stderr, "qr: input must be a 2D matrix\n");
        return res;
    }
    if (arr->dtype != FLOAT32 && arr->dtype != FLOAT64) {
        fprintf(stderr, "qr: only FLOAT32 and FLOAT64 supported\n");
        return res;
    }

    int m = arr->shape[0];
    int n = arr->shape[1];
    int k = (m < n) ? m : n;  /* min(m, n)  --  reduced QR */

    /* Use double uniformly for computation to ensure precision */
    double *A = malloc(m * n * sizeof(double));
    double *Q = malloc(m * k * sizeof(double));
    double *R = calloc(k * n, sizeof(double));
    if (!A || !Q || !R) {
        free(A); free(Q); free(R);
        fprintf(stderr, "qr: malloc failed\n");
        return res;
    }

    if (arr->dtype == FLOAT64) {
        memcpy(A, arr->data, m * n * sizeof(double));
    } else {
        float *src = (float*)arr->data;
        for (int i = 0; i < m * n; i++) A[i] = (double)src[i];
    }

    /* Modified Gram-Schmidt:
     * for j = 0..k-1:
     *   v = A[:,j]   (copy column j)
     *   for i = 0..j-1:
     *     R[i][j] = Q[:,i]^T * v
     *     v = v - R[i][j] * Q[:,i]
     *   R[j][j] = ||v||
     *   if R[j][j] > eps: Q[:,j] = v / R[j][j]
     *   else: Q[:,j] = 0
     */
    double *v = malloc(m * sizeof(double));
    for (int j = 0; j < k; j++) {
        /* v = column j of A */
        for (int i = 0; i < m; i++) v[i] = A[i * n + j];

        for (int p = 0; p < j; p++) {
            double dot = 0.0;
            for (int i = 0; i < m; i++) dot += Q[i * k + p] * v[i];
            R[p * n + j] = dot;
            for (int i = 0; i < m; i++) v[i] -= dot * Q[i * k + p];
        }

        double norm = 0.0;
        for (int i = 0; i < m; i++) norm += v[i] * v[i];
        norm = sqrt(norm);

        if (norm > 1e-12) {
            R[j * n + j] = norm;
            for (int i = 0; i < m; i++) Q[i * k + j] = v[i] / norm;
        } else {
            R[j * n + j] = 0.0;
            for (int i = 0; i < m; i++) Q[i * k + j] = 0.0;
        }
    }

    /* handle remaining columns (when m < n, j >= k), compute R = Q^T * A[:,j] */
    for (int j = k; j < n; j++) {
        for (int p = 0; p < k; p++) {
            double dot = 0.0;
            for (int i = 0; i < m; i++) dot += Q[i * k + p] * A[i * n + j];
            R[p * n + j] = dot;
        }
    }

    free(v);

    /* build output */
    res.Q = create_array((int[]){m, k}, 2, arr->dtype);
    res.R = create_array((int[]){k, n}, 2, arr->dtype);
    if (!res.Q || !res.R) {
        free_array(res.Q); free_array(res.R);
        res.Q = NULL; res.R = NULL;
        free(A); free(Q); free(R);
        return res;
    }

    if (arr->dtype == FLOAT64) {
        memcpy(res.Q->data, Q, m * k * sizeof(double));
        memcpy(res.R->data, R, k * n * sizeof(double));
    } else {
        float *qd = (float*)res.Q->data;
        float *rd = (float*)res.R->data;
        for (int i = 0; i < m * k; i++) qd[i] = (float)Q[i];
        for (int i = 0; i < k * n; i++) rd[i] = (float)R[i];
    }

    free(A); free(Q); free(R);
    return res;
}
