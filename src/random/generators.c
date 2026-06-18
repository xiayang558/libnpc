/**
 * generators.c — basic random variate generators
 *
 * Covers uniform [0,1), random integers, standard normal (Box-Muller),
 * normal(loc,scale), standard exponential, exponential(scale), and uniform(low,high).
 */

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "random.h"
#include "array.h"

/** Allocate and fill an array with pre-computed double values, casting to dtype */
static Array* make_array(double *vals, int *shape, int ndim, DataType dtype) {
    if (!shape || ndim <= 0) {
        shape = (int[]){1}; ndim = 1;
    }
    Array *a = create_array(shape, ndim, dtype);
    if (!a) return NULL;
    size_t esz = dtype_size(dtype);
    char *dst = (char*)a->data;
    for (int i = 0; i < a->size; i++) {
        double v = vals[i];
        switch (dtype) {
            case FLOAT64: memcpy(dst + i*esz, &v, 8); break;
            case FLOAT32: { float f = (float)v; memcpy(dst + i*esz, &f, 4); break; }
            case INT32: { int32_t iv = (int32_t)v; memcpy(dst + i*esz, &iv, 4); break; }
            case INT64: { int64_t iv = (int64_t)v; memcpy(dst + i*esz, &iv, 8); break; }
            default: break;
        }
    }
    return a;
}

/* ─── Uniform [0,1) ──────────────────────────────────────────────── */

/**
 * random_sample — generate uniformly distributed doubles in [0, 1)
 *
 * Equivalent to numpy.random.random_sample.
 *
 * @param shape  Output shape (NULL/ndim=0 returns scalar)
 * @param ndim   Number of dimensions
 * @return FLOAT64 array of uniform variates in [0, 1)
 */
Array* random_sample(int *shape, int ndim) {
    if (!shape || ndim <= 0) { shape = (int[]){1}; ndim = 1; }
    Array *a = create_array(shape, ndim, FLOAT64);
    if (!a) return NULL;
    double *d = (double*)a->data;
    for (int i = 0; i < a->size; i++) d[i] = random_double();
    return a;
}

/** Alias for random_sample (like numpy.random.rand) */
Array* random_rand(int *shape, int ndim) {
    return random_sample(shape, ndim);
}

/* ─── Random integers ───────────────────────────────────────────── */

/**
 * random_randint — generate uniform random integers in [low, high)
 *
 * Equivalent to numpy.random.randint.
 *
 * @param low    Lower bound (inclusive)
 * @param high   Upper bound (exclusive)
 * @param shape  Output shape
 * @param ndim   Number of dimensions
 * @param dtype  Output dtype (INT32 or INT64 recommended)
 * @return Array of random integers in [low, high)
 */
Array* random_randint(int low, int high, int *shape, int ndim, DataType dtype) {
    if (high <= low) { high = low + 1; }
    if (!shape || ndim <= 0) { shape = (int[]){1}; ndim = 1; }
    uint64_t range = (uint64_t)(high - low);

    Array *a = create_array(shape, ndim, dtype);
    if (!a) return NULL;
    size_t esz = dtype_size(dtype);
    char *dst = (char*)a->data;

    for (int i = 0; i < a->size; i++) {
        int64_t v = (int64_t)(low + random_uint64() % range);
        switch (dtype) {
            case INT32: { int32_t iv = (int32_t)v; memcpy(dst + i*esz, &iv, esz); break; }
            case INT64: memcpy(dst + i*esz, &v, esz); break;
            default: { double dv = (double)v; memcpy(dst + i*esz, &dv, esz); break; }
        }
    }
    return a;
}

/* ─── Normal distribution ───────────────────────────────────────── */

/**
 * random_standard_normal_val — generate a single N(0,1) variate
 *
 * Uses the Box-Muller transform (cosine variant).
 *
 * @return A standard normal variate
 */
double random_standard_normal_val(void) {
    double u1 = random_double();
    double u2 = random_double();
    return sqrt(-2.0 * log(u1 + 1e-300)) * cos(2.0 * M_PI * u2);
}

/**
 * random_standard_normal — generate standard normal variates N(0,1)
 *
 * Equivalent to numpy.random.standard_normal.
 * Generates pairs via Box-Muller for efficiency.
 *
 * @param shape  Output shape
 * @param ndim   Number of dimensions
 * @return FLOAT64 array of standard normal variates
 */
Array* random_standard_normal(int *shape, int ndim) {
    if (!shape || ndim <= 0) { shape = (int[]){1}; ndim = 1; }
    Array *a = create_array(shape, ndim, FLOAT64);
    if (!a) return NULL;
    int n = a->size;
    double *d = (double*)a->data;
    /* Box-Muller generates two values at a time */
    for (int i = 0; i < n - 1; i += 2) {
        double u1 = random_double(), u2 = random_double();
        double r = sqrt(-2.0 * log(u1 + 1e-300));
        d[i]   = r * cos(2.0 * M_PI * u2);
        d[i+1] = r * sin(2.0 * M_PI * u2);
    }
    if (n % 2) d[n-1] = random_standard_normal_val();
    return a;
}

/** Alias for random_standard_normal (like numpy.random.randn) */
Array* random_randn(int *shape, int ndim) {
    return random_standard_normal(shape, ndim);
}

/**
 * random_normal — generate normal variates N(loc, scale^2)
 *
 * Equivalent to numpy.random.normal.
 *
 * @param loc    Mean of the distribution
 * @param scale  Standard deviation (> 0)
 * @param shape  Output shape
 * @param ndim   Number of dimensions
 * @return FLOAT64 array of normal variates
 */
Array* random_normal(double loc, double scale, int *shape, int ndim) {
    if (!shape || ndim <= 0) { shape = (int[]){1}; ndim = 1; }
    Array *a = create_array(shape, ndim, FLOAT64);
    if (!a) return NULL;
    int n = a->size;
    double *d = (double*)a->data;
    for (int i = 0; i < n; i++)
        d[i] = loc + scale * random_standard_normal_val();
    return a;
}

/* ─── Exponential / Uniform ─────────────────────────────────────── */

/**
 * random_standard_exponential_val — generate a single Exp(1) variate
 *
 * Uses the inverse CDF method: -log(1-u).
 *
 * @return A standard exponential variate (mean = 1)
 */
double random_standard_exponential_val(void) {
    return -log(1.0 - random_double());
}

/**
 * random_standard_exponential — generate standard exponential variates Exp(1)
 *
 * Equivalent to numpy.random.standard_exponential.
 *
 * @param shape  Output shape
 * @param ndim   Number of dimensions
 * @return FLOAT64 array of exponential variates with scale = 1
 */
Array* random_standard_exponential(int *shape, int ndim) {
    if (!shape || ndim <= 0) { shape = (int[]){1}; ndim = 1; }
    Array *a = create_array(shape, ndim, FLOAT64);
    if (!a) return NULL;
    double *d = (double*)a->data;
    for (int i = 0; i < a->size; i++)
        d[i] = -log(1.0 - random_double());
    return a;
}

/**
 * random_exponential — generate exponential variates Exp(1/scale)
 *
 * Equivalent to numpy.random.exponential.
 *
 * @param scale  Scale parameter (mean = scale)
 * @param shape  Output shape
 * @param ndim   Number of dimensions
 * @return FLOAT64 array of exponential variates
 */
Array* random_exponential(double scale, int *shape, int ndim) {
    if (!shape || ndim <= 0) { shape = (int[]){1}; ndim = 1; }
    Array *a = create_array(shape, ndim, FLOAT64);
    if (!a) return NULL;
    double *d = (double*)a->data;
    for (int i = 0; i < a->size; i++)
        d[i] = scale * (-log(1.0 - random_double()));
    return a;
}

/**
 * random_uniform — generate uniform variates in [low, high)
 *
 * Equivalent to numpy.random.uniform.
 *
 * @param low    Lower bound (inclusive)
 * @param high   Upper bound (exclusive)
 * @param shape  Output shape
 * @param ndim   Number of dimensions
 * @return FLOAT64 array of uniform variates in [low, high)
 */
Array* random_uniform(double low, double high, int *shape, int ndim) {
    if (!shape || ndim <= 0) { shape = (int[]){1}; ndim = 1; }
    Array *a = create_array(shape, ndim, FLOAT64);
    if (!a) return NULL;
    double *d = (double*)a->data;
    double spread = high - low;
    for (int i = 0; i < a->size; i++)
        d[i] = low + spread * random_double();
    return a;
}
