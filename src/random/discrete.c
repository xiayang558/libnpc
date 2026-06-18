/**
 * discrete.c — discrete probability distributions
 */

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "random.h"
#include "array.h"

/** Allocate a FLOAT64 array; NULL/ndim=0 returns scalar. */
static Array* make_f64(int *shape, int ndim) {
    if (!shape || ndim <= 0) { shape = (int[]){1}; ndim = 1; }
    return create_array(shape, ndim, FLOAT64);
}

/** Allocate an INT64 array; NULL/ndim=0 returns scalar. */
static Array* make_i64(int *shape, int ndim) {
    if (!shape || ndim <= 0) { shape = (int[]){1}; ndim = 1; }
    return create_array(shape, ndim, INT64);
}

/* ─── Discrete distributions ────────────────────────────────────── */

/**
 * random_geometric — generate geometric variates with success probability p
 *
 * Equivalent to numpy.random.geometric.
 * Inverse CDF: ceil(log(1-u) / log(1-p)) - 1.
 *
 * @param p      Success probability (0 < p <= 1)
 * @param shape  Output shape / ndim
 * @return INT64 array of geometric variates (number of failures before success)
 */
Array* random_geometric(double p, int *shape, int ndim) {
    if (p <= 0 || p > 1) p = 0.5;
    Array *r = make_i64(shape, ndim); if (!r) return NULL;
    int64_t *d = (int64_t*)r->data;
    double log1mp = log(1.0 - p);
    for (int i = 0; i < r->size; i++)
        d[i] = (int64_t)ceil(log(1.0 - random_double()) / log1mp) - 1;
    return r;
}

/**
 * random_poisson — generate Poisson variates with rate lambda
 *
 * Equivalent to numpy.random.poisson.
 * Small lambda (< 30): Knuth's product-of-uniforms method.
 * Large lambda: normal approximation.
 *
 * @param lam    Rate parameter (>= 0)
 * @param shape  Output shape / ndim
 * @return INT64 array of Poisson variates
 */
Array* random_poisson(double lam, int *shape, int ndim) {
    if (lam < 0) lam = 0;
    Array *r = make_i64(shape, ndim); if (!r) return NULL;
    int64_t *d = (int64_t*)r->data;

    for (int i = 0; i < r->size; i++) {
        if (lam < 30) {
            /* Knuth: count uniform products until exp(-lambda) exceeded */
            double L = exp(-lam);
            int64_t k = 0;
            double p = 1.0;
            do { k++; p *= random_double(); } while (p > L);
            d[i] = k - 1;
        } else {
            /* Normal approximation for large lambda */
            d[i] = (int64_t)(lam + sqrt(lam) * random_standard_normal_val() + 0.5);
            if (d[i] < 0) d[i] = 0;
        }
    }
    return r;
}

/**
 * random_binomial — generate binomial variates B(n, p)
 *
 * Equivalent to numpy.random.binomial.
 * Small n (< 30): direct counting of Bernoulli trials.
 * Large n: normal approximation.
 *
 * @param n      Number of trials (>= 0)
 * @param p      Success probability per trial
 * @param shape  Output shape / ndim
 * @return INT64 array of binomial variates
 */
Array* random_binomial(int n, double p, int *shape, int ndim) {
    if (n <= 0) n = 1;
    if (p < 0) p = 0; if (p > 1) p = 1;
    Array *r = make_i64(shape, ndim); if (!r) return NULL;
    int64_t *d = (int64_t*)r->data;

    for (int i = 0; i < r->size; i++) {
        if (n < 30) {
            int64_t cnt = 0;
            for (int j = 0; j < n; j++)
                if (random_double() < p) cnt++;
            d[i] = cnt;
        } else {
            double mu = n * p;
            double sigma = sqrt(n * p * (1.0 - p));
            int64_t v = (int64_t)(mu + sigma * random_standard_normal_val() + 0.5);
            if (v < 0) v = 0; if (v > n) v = n;
            d[i] = v;
        }
    }
    return r;
}

/**
 * random_negative_binomial — generate negative binomial variates NB(n, p)
 *
 * Equivalent to numpy.random.negative_binomial.
 * Uses the gamma-Poisson mixture: Gamma(n, (1-p)/p) then Poisson.
 *
 * @param n      Number of successes (> 0)
 * @param p      Success probability (0 < p <= 1)
 * @param shape  Output shape / ndim
 * @return INT64 array of negative binomial variates
 */
Array* random_negative_binomial(int n, double p, int *shape, int ndim) {
    if (n <= 0) n = 1;
    if (p <= 0 || p > 1) p = 0.5;
    Array *r = make_i64(shape, ndim); if (!r) return NULL;
    int64_t *d = (int64_t*)r->data;
    /* Gamma-Poisson mixture */
    for (int i = 0; i < r->size; i++) {
        double g = random_standard_gamma_val((double)n);
        double lam = g * (1.0 - p) / p;
        /* Poisson with that rate */
        if (lam < 30) {
            double L = exp(-lam);
            int64_t k = 0;
            double pp = 1.0;
            do { k++; pp *= random_double(); } while (pp > L);
            d[i] = k - 1;
        } else {
            d[i] = (int64_t)(lam + sqrt(lam) * random_standard_normal_val() + 0.5);
            if (d[i] < 0) d[i] = 0;
        }
    }
    return r;
}

/**
 * random_hypergeometric — generate hypergeometric variates
 *
 * Equivalent to numpy.random.hypergeometric.
 * Simulates drawing nsample items without replacement from an urn
 * containing ngood "good" and nbad "bad" items.
 *
 * @param ngood   Number of good items
 * @param nbad    Number of bad items
 * @param nsample Number of draws
 * @param shape   Output shape / ndim
 * @return INT64 array of hypergeometric variates (count of good items drawn)
 */
Array* random_hypergeometric(int ngood, int nbad, int nsample, int *shape, int ndim) {
    if (ngood < 0) ngood = 0; if (nbad < 0) nbad = 0;
    if (nsample > ngood + nbad) nsample = ngood + nbad;
    Array *r = make_i64(shape, ndim); if (!r) return NULL;
    int64_t *d = (int64_t*)r->data;

    for (int i = 0; i < r->size; i++) {
        int good = ngood, bad = nbad;
        int64_t cnt = 0;
        for (int k = 0; k < nsample; k++) {
            double prob = (double)good / (double)(good + bad);
            if (random_double() < prob) { cnt++; good--; }
            else { bad--; }
        }
        d[i] = cnt;
    }
    return r;
}

/**
 * random_zipf — generate Zipf variates with exponent a
 *
 * Equivalent to numpy.random.zipf.
 * Uses rejection sampling.
 *
 * @param a      Exponent parameter (> 1)
 * @param shape  Output shape / ndim
 * @return INT64 array of Zipf variates
 */
Array* random_zipf(double a, int *shape, int ndim) {
    if (a <= 1.0) a = 2.0;
    Array *r = make_i64(shape, ndim); if (!r) return NULL;
    int64_t *d = (int64_t*)r->data;

    double am1 = a - 1.0;
    double b = pow(2.0, am1);
    for (int i = 0; i < r->size; i++) {
        double u, v, x;
        do {
            u = random_double();
            v = random_double();
            x = floor(pow(u, -1.0 / am1));
        } while (v * x * (x + 1.0) / (x - 0.5) > b);
        d[i] = (int64_t)x;
    }
    return r;
}

/**
 * random_logseries — generate logarithmic series variates
 *
 * Equivalent to numpy.random.logseries.
 * Inverse transform method.
 *
 * @param p      Probability parameter (0 < p < 1)
 * @param shape  Output shape / ndim
 * @return INT64 array of logseries variates
 */
Array* random_logseries(double p, int *shape, int ndim) {
    if (p <= 0 || p >= 1) p = 0.5;
    Array *r = make_i64(shape, ndim); if (!r) return NULL;
    int64_t *d = (int64_t*)r->data;
    double t = -log(1.0 - p);
    for (int i = 0; i < r->size; i++) {
        double u = random_double();
        d[i] = (int64_t)(1.0 + log(1.0 - u * (1.0 - exp(-t))) / log(p));
    }
    return r;
}
