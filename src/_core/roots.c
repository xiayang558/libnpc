#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "array.h"
#include "utils.h"

/* ───────────────────────────────────────────────────────────────
 *  Francis double-shift QR algorithm for real upper Hessenberg
 *  matrices (the companion matrix is Hessenberg by construction).
 *
 *  Finds eigenvalues reliably for non-symmetric real matrices.
 * ─────────────────────────────────────────────────────────────── */

static double dh_sign(double a, double b) {
    return b >= 0.0 ? fabs(a) : -fabs(a);
}

/* Compute Householder reflector H = I - tau * [1; v[1..len-1]] * [1, v[1..len-1]]
 * such that H * x = beta * e1.  v[0] == 1 is implicit.
 * On exit: x[0] = beta, x[1..len-1] = v[1..len-1]; tau is returned separately. */
static double make_householder(double *x, int len) {
    double xn = 0.0;
    for (int i = 0; i < len; i++) xn += x[i] * x[i];
    xn = sqrt(xn);
    if (xn == 0.0) return 0.0;
    double alpha = x[0];
    double beta  = -dh_sign(xn, alpha);
    double tau   = (beta - alpha) / beta;
    x[0] = beta;
    double inv = 1.0 / (alpha - beta);
    for (int i = 1; i < len; i++) x[i] *= inv;
    return tau;
}


#define HQR_MAX_ITER  200
#define HQR_EPS       2.220446049250313e-16

/**
 * hqr_eigenvalues — compute eigenvalues of a real upper Hessenberg matrix
 *
 * Uses Francis (double-shift) implicit QR algorithm.
 * H must be square, n×n, row-major.  On output each eigenvalue's real
 * part may sit on the diagonal or in 2×2 blocks.
 *
 * @param H        n×n upper Hessenberg (row-major), modified in place
 * @param n        dimension
 * @param eigvals  output: n complex eigenvalues
 */
static int hqr_eigenvalues(double *H, int n, complex double *eigvals) {
    if (n == 0) return 0;
    if (n == 1) { eigvals[0] = H[0] + 0.0*I; return 0; }
    if (n == 2) {
        double a = H[0], b = H[1], c = H[2], d = H[3];
        double tr = a + d, disc = tr*tr - 4.0*(a*d - b*c);
        if (disc >= 0.0) {
            double sq = sqrt(disc);
            eigvals[0] = 0.5*(tr + sq) + 0.0*I;
            eigvals[1] = 0.5*(tr - sq) + 0.0*I;
        } else {
            double sq = sqrt(-disc);
            eigvals[0] = 0.5*tr + 0.5*sq*I;
            eigvals[1] = 0.5*tr - 0.5*sq*I;
        }
        return 0;
    }

    int nn = n;   /* active trailing dimension */
    int iter = 0;

    while (nn >= 1) {
        /* ── find negligible subdiagonal element ── */
        int l = nn - 1;
        while (l >= 1) {
            double s = fabs(H[(l-1)*n + (l-1)]) + fabs(H[l*n + l]);
            if (s == 0.0) s = fabs(H[nn-1 + (nn-1)*n]);  /* fallback norm */
            if (fabs(H[l*n + (l-1)]) <= HQR_EPS * s) {
                H[l*n + (l-1)] = 0.0;
                break;
            }
            l--;
        }

        if (l == nn - 1) {
            /* 1×1 block — real eigenvalue */
            eigvals[nn-1] = H[(nn-1)*n + (nn-1)] + 0.0*I;
            nn--;
            iter = 0;
        } else if (l == nn - 2) {
            /* 2×2 block — extract eigenvalue pair */
            double a = H[(nn-2)*n + (nn-2)], b = H[(nn-2)*n + (nn-1)];
            double c = H[(nn-1)*n + (nn-2)], d = H[(nn-1)*n + (nn-1)];
            double tr   = a + d;
            double det  = a*d - b*c;
            double disc = tr*tr - 4.0*det;
            if (disc >= 0.0) {
                double sq = sqrt(disc);
                eigvals[nn-2] = 0.5*(tr + sq) + 0.0*I;
                eigvals[nn-1] = 0.5*(tr - sq) + 0.0*I;
            } else {
                double sq = sqrt(-disc);
                eigvals[nn-2] = 0.5*tr + 0.5*sq*I;
                eigvals[nn-1] = 0.5*tr - 0.5*sq*I;
            }
            nn -= 2;
            iter = 0;
        } else if (iter >= HQR_MAX_ITER) {
            fprintf(stderr, "hqr: failed to converge after %d iterations (n=%d)\n",
                    HQR_MAX_ITER, n);
            return -1;
        } else {
            /* ── Francis double-shift QR step ── */

            /* shifts s, t from trailing 2×2:
             * (H - s1*I)(H - s2*I) = H² - s H + t I
             * where s = s1+s2 = tr,  t = s1*s2 = det           */
            double s = H[(nn-2)*n + (nn-2)] + H[(nn-1)*n + (nn-1)];
            double t = H[(nn-2)*n + (nn-2)] * H[(nn-1)*n + (nn-1)]
                     - H[(nn-2)*n + (nn-1)] * H[(nn-1)*n + (nn-2)];

            /* first 3 elements of column l of M = H² - s H + t I */
            double x = H[l*n + l]*H[l*n + l] + H[l*n + (l+1)]*H[(l+1)*n + l]
                       - s*H[l*n + l] + t;
            double y = H[(l+1)*n + l] * (H[l*n + l] + H[(l+1)*n + (l+1)] - s);
            double z = H[(l+1)*n + l] * H[(l+2)*n + (l+1)];

            double work[3];
            double tau;

            for (int k = l; k <= nn - 2; k++) {
                int nv = (k == nn - 2) ? 2 : 3;

                if (k == l) {
                    work[0] = x; work[1] = y; work[2] = z;
                } else {
                    /* bulge sits at rows k..k+nv-1 of column k-1 */
                    work[0] = H[k*n + (k-1)];
                    work[1] = H[(k+1)*n + (k-1)];
                    if (nv == 3) work[2] = H[(k+2)*n + (k-1)];
                }

                tau = make_householder(work, nv);
                if (tau != 0.0) {
                    /* Apply from left  — rows k..k+nv-1 */
                    for (int j = 0; j < n; j++) {
                        double sum = H[k*n + j];
                        for (int i = 1; i < nv; i++)
                            sum += work[i] * H[(k+i)*n + j];
                        sum *= tau;
                        H[k*n + j] -= sum;
                        for (int i = 1; i < nv; i++)
                            H[(k+i)*n + j] -= sum * work[i];
                    }
                    /* Apply from right — columns k..k+nv-1 */
                    int last_row = (k + nv < n) ? k + nv : n - 1;
                    for (int i = 0; i <= last_row; i++) {
                        double sum = H[i*n + k];
                        for (int j = 1; j < nv; j++)
                            sum += work[j] * H[i*n + (k+j)];
                        sum *= tau;
                        H[i*n + k] -= sum;
                        for (int j = 1; j < nv; j++)
                            H[i*n + (k+j)] -= sum * work[j];
                    }
                }
            }
            iter++;
        }
    }
    return 0;
}

/* ───────────────────────────────────────────────────────────────
 *  roots  —  polynomial roots (like numpy.roots)
 * ─────────────────────────────────────────────────────────────── */

/**
 * roots — compute polynomial roots via companion matrix eigenvalues
 *
 * p(x) = p[0]*x^n + p[1]*x^{n-1} + ... + p[n]
 *
 * @param p  1D array of polynomial coefficients (descending powers)
 * @return   1D COMPLEX128 array of length = degree (may be empty)
 */
Array* roots(Array *p) {
    if (p == NULL) {
        fprintf(stderr, "roots: NULL argument\n");
        return NULL;
    }
    if (p->ndim != 1) {
        fprintf(stderr, "roots: coefficients must be 1D\n");
        return NULL;
    }

    /* ── strip leading zeros ── */
    int n = p->size;
    size_t elem_sz = dtype_size(p->dtype);
    char *pdata = (char*)p->data;

    int lead = 0;
    while (lead < n && fabs(get_value(pdata + lead * elem_sz, p->dtype)) < 1e-300) {
        lead++;
    }
    if (lead >= n) {
        return create_array((int[]){0}, 1, COMPLEX128);
    }

    n -= lead;
    if (n == 1) {
        return create_array((int[]){0}, 1, COMPLEX128);
    }

    /* ── normalize coefficients (divide by leading) ── */
    double leading = get_value(pdata + lead * elem_sz, p->dtype);
    double *coeff = malloc(n * sizeof(double));
    if (!coeff) return NULL;
    for (int i = 0; i < n; i++) {
        coeff[i] = get_value(pdata + (lead + i) * elem_sz, p->dtype) / leading;
    }

    /* ── build companion matrix ── */
    int mat_n = n - 1;
    double *mat = calloc(mat_n * mat_n, sizeof(double));
    if (!mat) { free(coeff); return NULL; }

    /* Companion matrix (upper Hessenberg):
     * subdiagonal = 1.0, last column = -coeff[n-1], -coeff[n-2], ..., -coeff[1] */
    for (int i = 0; i < mat_n; i++) {
        if (i + 1 < mat_n)
            mat[(i + 1) * mat_n + i] = 1.0;
        mat[i * mat_n + (mat_n - 1)] = -coeff[mat_n - i];
    }
    free(coeff);

    /* ── compute eigenvalues via Hessenberg QR ── */
    complex double *eigvals = malloc(mat_n * sizeof(complex double));
    if (!eigvals) { free(mat); return NULL; }

    int status = hqr_eigenvalues(mat, mat_n, eigvals);
    free(mat);

    if (status != 0) {
        free(eigvals);
        fprintf(stderr, "roots: eigenvalue computation failed\n");
        return NULL;
    }

    Array *result = create_array((int[]){mat_n}, 1, COMPLEX128);
    if (!result) { free(eigvals); return NULL; }
    memcpy(result->data, eigvals, mat_n * sizeof(complex double));
    free(eigvals);

    return result;
}
