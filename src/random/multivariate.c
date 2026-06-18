/**
 * multivariate.c — multivariate distributions and utility functions
 *
 * Includes Fisher-Yates permutation/shuffle, weighted random choice,
 * multinomial, multivariate normal (Cholesky), Dirichlet, and random bytes.
 */

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "random.h"
#include "array.h"
#include "utils.h"

/** Allocate a FLOAT64 array; NULL/ndim=0 returns scalar. */
static Array* make_f64(int *shape, int ndim) {
    if (!shape || ndim <= 0) { shape = (int[]){1}; ndim = 1; }
    return create_array(shape, ndim, FLOAT64);
}

/* ─── Permutation / Shuffle ─────────────────────────────────────── */

/**
 * random_permutation — return a randomly permuted copy of the input
 *
 * Equivalent to numpy.random.permutation.
 * Uses the Fisher-Yates (Knuth) shuffle algorithm.
 *
 * @param x  Input array (any dtype, any shape)
 * @return A new array with the same elements in random order (same shape/dtype)
 */
Array* random_permutation(Array *x) {
    if (!x) return NULL;
    Array *r = copy(x);
    if (!r) return NULL;
    int n = r->size;
    if (n < 2) return r;

    size_t esz = dtype_size(r->dtype);
    char *data = (char*)r->data;
    char *tmp = malloc(esz);

    for (int i = n - 1; i > 0; i--) {
        int j = (int)(random_uint64() % (i + 1));
        if (i != j) {
            memcpy(tmp, data + i * esz, esz);
            memcpy(data + i * esz, data + j * esz, esz);
            memcpy(data + j * esz, tmp, esz);
        }
    }
    free(tmp);
    return r;
}

/**
 * random_shuffle — randomly shuffle an array in-place
 *
 * Equivalent to numpy.random.shuffle.
 * Uses the Fisher-Yates (Knuth) shuffle algorithm.
 *
 * @param x  Array to shuffle in-place (any dtype, any shape)
 */
void random_shuffle(Array *x) {
    if (!x || x->size < 2) return;
    int n = x->size;
    size_t esz = dtype_size(x->dtype);
    char *data = (char*)x->data;
    char *tmp = malloc(esz);
    for (int i = n - 1; i > 0; i--) {
        int j = (int)(random_uint64() % (i + 1));
        if (i != j) {
            memcpy(tmp, data + i * esz, esz);
            memcpy(data + i * esz, data + j * esz, esz);
            memcpy(data + j * esz, tmp, esz);
        }
    }
    free(tmp);
}

/* ─── Random Choice ─────────────────────────────────────────────── */

/**
 * random_choice — randomly sample elements from an array
 *
 * Equivalent to numpy.random.choice.
 * Supports uniform (p=NULL) and weighted (p!=NULL) sampling.
 *
 * @param a        Source array (1D)
 * @param size     Number of samples to draw
 * @param replace  Whether to sample with replacement (non-zero = yes)
 * @param p        Optional probability weights (1D, same length as a)
 * @return 1D array of sampled elements (same dtype as a)
 */
Array* random_choice(Array *a, int size, int replace, Array *p) {
    if (!a || size < 0) return NULL;
    int n = a->size;
    if (n == 0) return NULL;

    if (!replace && size > n) size = n;

    Array *r = create_array((int[]){size}, 1, a->dtype);
    if (!r) return NULL;
    size_t esz = dtype_size(a->dtype);
    char *src = (char*)a->data;
    char *dst = (char*)r->data;

    if (p) {
        /* Weighted sampling with replacement */
        double *cum = malloc(n * sizeof(double));
        double sum = 0;
        size_t p_sz = dtype_size(p->dtype);
        for (int i = 0; i < n; i++) {
            sum += get_value((char*)p->data + i * p_sz, p->dtype);
            cum[i] = sum;
        }
        if (sum > 0) {
            for (int i = 0; i < size; i++) {
                double u = random_double() * sum;
                int idx = 0;
                while (idx < n && cum[idx] < u) idx++;
                if (idx >= n) idx = n - 1;
                memcpy(dst + i * esz, src + idx * esz, esz);
            }
        }
        free(cum);
    } else {
        /* Uniform sampling with replacement */
        for (int i = 0; i < size; i++) {
            int idx = (int)(random_uint64() % n);
            memcpy(dst + i * esz, src + idx * esz, esz);
        }
    }
    return r;
}

/* ─── Multinomial ───────────────────────────────────────────────── */

/**
 * random_multinomial — generate multinomial variates
 *
 * Equivalent to numpy.random.multinomial.
 * For each batch element, n trials are allocated across k categories
 * according to the probability vector pvals.
 *
 * @param n      Number of trials per batch element
 * @param pvals  1D array of category probabilities (length k, will be normalized)
 * @param shape  Batch shape (prefix dimensions before the k categories)
 * @param ndim   Number of batch dimensions
 * @return INT64 array of shape (shape..., k)
 */
Array* random_multinomial(int n, Array *pvals, int *shape, int ndim) {
    if (!pvals || pvals->ndim != 1 || n < 0) return NULL;
    int k = pvals->size;

    /* Compute output shape: batch_shape + [k] */
    int out_ndim;
    int *out_shape = malloc((ndim + 1) * sizeof(int));
    if (!shape || ndim <= 0) {
        out_ndim = 1; out_shape[0] = k;
    } else {
        out_ndim = ndim + 1;
        for (int i = 0; i < ndim; i++) out_shape[i] = shape[i];
        out_shape[ndim] = k;
    }

    Array *r = create_array(out_shape, out_ndim, INT64);
    free(out_shape);
    if (!r) return NULL;

    /* Normalize pvals to sum to 1 */
    double *prob = malloc(k * sizeof(double));
    double sum = 0;
    size_t p_sz = dtype_size(pvals->dtype);
    for (int i = 0; i < k; i++)
        sum += get_value((char*)pvals->data + i * p_sz, pvals->dtype);
    if (sum <= 0) {
        for (int i = 0; i < k; i++) prob[i] = 1.0 / k;
    } else {
        for (int i = 0; i < k; i++)
            prob[i] = get_value((char*)pvals->data + i * p_sz, pvals->dtype) / sum;
    }

    int batch = r->size / k;
    int64_t *d = (int64_t*)r->data;

    /* Build cumulative probabilities for fast lookup */
    double *cum = malloc(k * sizeof(double));
    for (int b = 0; b < batch; b++) {
        for (int j = 0; j < k; j++) d[b * k + j] = 0;
        cum[0] = prob[0];
        for (int j = 1; j < k; j++) cum[j] = cum[j-1] + prob[j];
        /* Generate n uniform samples, sort into bins */
        for (int trial = 0; trial < n; trial++) {
            double u = random_double();
            int idx = 0;
            while (idx < k - 1 && u > cum[idx]) idx++;
            d[b * k + idx]++;
        }
    }
    free(cum);
    free(prob);
    return r;
}

/* ─── Multivariate Normal ───────────────────────────────────────── */

/**
 * random_multivariate_normal — generate multivariate normal variates
 *
 * Equivalent to numpy.random.multivariate_normal.
 * Uses Cholesky decomposition: X = mean + L * Z where cov = L * L^T
 * and Z consists of independent standard normal variates.
 *
 * @param mean  1D mean vector (length d)
 * @param cov   2D covariance matrix (d x d, must be positive semi-definite)
 * @param shape Batch shape (prefix dimensions before d)
 * @param ndim  Number of batch dimensions
 * @return FLOAT64 array of shape (shape..., d)
 */
Array* random_multivariate_normal(Array *mean, Array *cov, int *shape, int ndim) {
    if (!mean || !cov || mean->ndim != 1 || cov->ndim != 2) return NULL;
    int d = mean->size;
    if (cov->shape[0] != d || cov->shape[1] != d) return NULL;

    /* Compute Cholesky decomposition: cov = L * L^T, L is lower triangular */
    double *L = calloc(d * d, sizeof(double));
    size_t cov_esz = dtype_size(cov->dtype);
    for (int i = 0; i < d; i++) {
        for (int j = 0; j <= i; j++) {
            double s = get_value((char*)cov->data + (i * d + j) * cov_esz, cov->dtype);
            for (int kp = 0; kp < j; kp++)
                s -= L[i * d + kp] * L[j * d + kp];
            if (i == j) L[i * d + j] = sqrt(s > 0 ? s : 0);
            else L[i * d + j] = s / L[j * d + j];
        }
    }

    /* Output shape: batch_shape + [d] */
    int out_ndim;
    int *out_shape = malloc((ndim + 1) * sizeof(int));
    if (!shape || ndim <= 0) {
        out_ndim = 1; out_shape[0] = d;
    } else {
        out_ndim = ndim + 1;
        for (int i = 0; i < ndim; i++) out_shape[i] = shape[i];
        out_shape[ndim] = d;
    }

    Array *r = create_array(out_shape, out_ndim, FLOAT64);
    free(out_shape);
    if (!r) { free(L); return NULL; }

    int batch = r->size / d;
    double *rd = (double*)r->data;
    double *z = malloc(d * sizeof(double));
    size_t mean_esz = dtype_size(mean->dtype);

    for (int b = 0; b < batch; b++) {
        /* Generate independent standard normals */
        for (int j = 0; j < d; j++)
            z[j] = random_standard_normal_val();
        /* Transform by Cholesky factor and add mean */
        for (int i = 0; i < d; i++) {
            double s = 0;
            for (int j = 0; j <= i; j++)
                s += L[i * d + j] * z[j];
            rd[b * d + i] = s + get_value((char*)mean->data + i * mean_esz, mean->dtype);
        }
    }

    free(z); free(L);
    return r;
}

/* ─── Dirichlet ─────────────────────────────────────────────────── */

/**
 * random_dirichlet — generate Dirichlet variates
 *
 * Equivalent to numpy.random.dirichlet.
 * Generated as normalized gamma variates:
 * For each category j, draw G_j ~ Gamma(alpha_j, 1), then normalize Y_j = G_j / sum(G).
 *
 * @param alpha  1D array of concentration parameters (length k, each > 0)
 * @param shape  Batch shape (prefix dimensions before k)
 * @param ndim   Number of batch dimensions
 * @return FLOAT64 array of shape (shape..., k), each row sums to 1
 */
Array* random_dirichlet(Array *alpha, int *shape, int ndim) {
    if (!alpha || alpha->ndim != 1) return NULL;
    int k = alpha->size;

    int out_ndim;
    int *out_shape = malloc((ndim + 1) * sizeof(int));
    if (!shape || ndim <= 0) {
        out_ndim = 1; out_shape[0] = k;
    } else {
        out_ndim = ndim + 1;
        for (int i = 0; i < ndim; i++) out_shape[i] = shape[i];
        out_shape[ndim] = k;
    }

    Array *r = create_array(out_shape, out_ndim, FLOAT64);
    free(out_shape);
    if (!r) return NULL;

    int batch = r->size / k;
    double *rd = (double*)r->data;
    size_t a_esz = dtype_size(alpha->dtype);

    for (int b = 0; b < batch; b++) {
        double sum = 0;
        /* Generate raw gamma variates */
        for (int j = 0; j < k; j++) {
            double aj = get_value((char*)alpha->data + j * a_esz, alpha->dtype);
            if (aj <= 0) aj = 1e-10;
            rd[b * k + j] = random_standard_gamma_val(aj);
            sum += rd[b * k + j];
        }
        /* Normalize to sum to 1 */
        for (int j = 0; j < k; j++)
            rd[b * k + j] /= sum;
    }
    return r;
}

/* ─── Random Bytes ──────────────────────────────────────────────── */

/**
 * random_bytes — generate random bytes
 *
 * Equivalent to numpy.random.bytes.
 *
 * @param n  Number of bytes to generate
 * @return UINT8 array of length n
 */
Array* random_bytes(int n) {
    if (n <= 0) n = 1;
    Array *r = create_array((int[]){n}, 1, UINT8);
    if (!r) return NULL;
    uint8_t *d = (uint8_t*)r->data;
    for (int i = 0; i < n; i++)
        d[i] = (uint8_t)(random_uint64() & 0xFF);
    return r;
}
