#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "array.h"
#include "utils.h"

/* Solve Vandermonde system A^T A c = A^T y via Gaussian elimination */
static int solve_normal(double *A, double *y, int n, int deg, double *c) {
    int m = deg + 1;
    double *ATA = calloc(m * m, sizeof(double));
    double *ATy = calloc(m, sizeof(double));
    if (!ATA || !ATy) { free(ATA); free(ATy); return -1; }

    /* ATA[i][j] = sum_k x_k^(i+j) ; ATy[i] = sum_k y_k * x_k^i */
    for (int k = 0; k < n; k++) {
        double xk = A[k], yk = y[k];
        double xp[32]; xp[0] = 1.0;
        for (int i = 1; i < 2*m; i++) xp[i] = xp[i-1] * xk;
        for (int i = 0; i < m; i++) {
            ATy[i] += yk * xp[i];
            for (int j = 0; j < m; j++) ATA[i*m + j] += xp[i + j];
        }
    }
    /* Gaussian elimination (no pivoting for simplicity — Vandermonde is well-conditioned for small deg) */
    for (int i = 0; i < m; i++) {
        int pivot = i;
        double maxv = fabs(ATA[i*m+i]);
        for (int r = i+1; r < m; r++)
            if (fabs(ATA[r*m+i]) > maxv) { maxv = fabs(ATA[r*m+i]); pivot = r; }
        if (maxv < 1e-15) { free(ATA); free(ATy); return -1; }
        if (pivot != i) {
            for (int j = 0; j < m; j++) { double t = ATA[i*m+j]; ATA[i*m+j] = ATA[pivot*m+j]; ATA[pivot*m+j] = t; }
            double t = ATy[i]; ATy[i] = ATy[pivot]; ATy[pivot] = t;
        }
        double piv = ATA[i*m+i];
        for (int j = i; j < m; j++) ATA[i*m+j] /= piv;
        ATy[i] /= piv;
        for (int r = i+1; r < m; r++) {
            double f = ATA[r*m+i];
            for (int j = i; j < m; j++) ATA[r*m+j] -= f * ATA[i*m+j];
            ATy[r] -= f * ATy[i];
        }
    }
    for (int i = m-1; i >= 0; i--) {
        c[i] = ATy[i];
        for (int j = i+1; j < m; j++) c[i] -= ATA[i*m+j] * c[j];
    }
    free(ATA); free(ATy);
    return 0;
}

Array* polyfit(Array *x, Array *y, int deg) {
    if (!x || !y || x->ndim != 1 || y->ndim != 1 || x->size != y->size) {
        fprintf(stderr,"polyfit: 1D arrays of same size required\n"); return NULL;
    }
    int n = x->size;
    if (deg < 0 || deg >= n) { fprintf(stderr,"polyfit: deg must be 0 <= deg < n\n"); return NULL; }
    double *xv = malloc(n * sizeof(double)), *yv = malloc(n * sizeof(double));
    if (!xv || !yv) { free(xv); free(yv); return NULL; }
    for (int i = 0; i < n; i++) {
        xv[i] = get_value((char*)x->data + i*dtype_size(x->dtype), x->dtype);
        yv[i] = get_value((char*)y->data + i*dtype_size(y->dtype), y->dtype);
    }
    double *c = calloc(deg + 1, sizeof(double));
    if (!c) { free(xv); free(yv); return NULL; }
    if (solve_normal(xv, yv, n, deg, c) != 0) { free(xv); free(yv); free(c); return NULL; }
    /* Reverse c: NumPy returns descending powers */
    Array *r = create_array((int[]){deg+1}, 1, FLOAT64);
    if (r) {
        double *rd = (double*)r->data;
        for (int i = 0; i <= deg; i++) rd[i] = c[deg - i];
    }
    free(xv); free(yv); free(c);
    return r;
}
