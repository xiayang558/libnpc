/**
 * test_utils.h — Shared test framework for libnpc
 *
 * Usage in each test file:
 *   #include "test_utils.h"
 *   int main(void) {
 *       TEST_MAIN("Suite Name");
 *       // ... test cases using ASSERT_* macros ...
 *       TEST_SUMMARY();
 *       return (g_fail > 0) ? 1 : 0;
 *   }
 */

#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include <complex.h>
#include <float.h>
#include "array.h"
#include "function.h"
#include "shape.h"
#include "random.h"
#include "datetime64.h"
#include "utils.h"
#include "window.h"
#include "histogram.h"
#include "io.h"
#include "linalg.h"
#include "einsum.h"
#include "npzfile.h"

/* ─── Global counters ─────────────────────────────────────────────── */

extern int g_pass;
extern int g_fail;

/* ─── Test case helpers ───────────────────────────────────────────── */

void t_start(const char *name);
void t_pass(void);
void t_fail(const char *fmt, ...);

/** Convenience: print suite banner */
#define TEST_MAIN(name) \
    printf("===== %s =====\n\n", name)

/** Convenience: print summary and section header */
#define TEST_SECTION(name) \
    printf("\n  -- %s --\n", name)

#define TEST_SUMMARY() do { \
    int _t = g_pass + g_fail; \
    printf("\n===== %d/%d passed", g_pass, _t); \
    if (g_fail > 0) printf("  (%d FAILED)", g_fail); \
    printf(" =====\n"); \
} while(0)

/* ─── Assertion macros ────────────────────────────────────────────── */

/** Assert boolean condition is true */
#define ASSERT_TRUE(cond, label) do { \
    t_start(label); \
    if (cond) t_pass(); else t_fail("expected true, got false"); \
} while(0)

/** Assert pointer is NULL */
#define ASSERT_NULL(ptr, label) do { \
    t_start(label); \
    if ((ptr) == NULL) t_pass(); else t_fail("expected NULL"); \
} while(0)

/** Assert pointer is not NULL */
#define ASSERT_NOTNULL(ptr, label) do { \
    t_start(label); \
    if ((ptr) != NULL) t_pass(); else t_fail("unexpected NULL"); \
} while(0)

/** Assert two ints are equal */
#define ASSERT_EQ_INT(got, exp, label) do { \
    t_start(label); \
    if ((got) == (exp)) t_pass(); else t_fail("got %d, expected %d", (int)(got), (int)(exp)); \
} while(0)

/** Assert two int64s are equal */
#define ASSERT_EQ_I64(got, exp, label) do { \
    t_start(label); \
    if ((got) == (exp)) t_pass(); else t_fail("got %lld, expected %lld", (long long)(got), (long long)(exp)); \
} while(0)

/** Assert two doubles are close within absolute tolerance */
#define ASSERT_EQ_DBL(got, exp, tol, label) do { \
    t_start(label); \
    double _g = (got), _e = (exp); \
    if (isnan(_e) && isnan(_g)) t_pass(); \
    else if (fabs(_g - _e) <= (tol)) t_pass(); \
    else t_fail("got %.12g, expected %.12g (diff %.2e)", _g, _e, fabs(_g - _e)); \
} while(0)

/** Assert two double arrays are close within absolute tolerance */
#define ASSERT_EQ_ARR(got, exp, n, tol, label) do { \
    t_start(label); \
    if ((got) == NULL) { t_fail("got NULL array"); break; } \
    if ((n) == 0) { t_pass(); break; } \
    double _w = 0; int _wi = 0; \
    for (int _i = 0; _i < (n); _i++) { \
        double _d = fabs((got)[_i] - (exp)[_i]); \
        if (isnan((exp)[_i]) && isnan((got)[_i])) continue; \
        if (_d > _w) { _w = _d; _wi = _i; } \
    } \
    if (_w <= (tol)) t_pass(); \
    else t_fail("max|diff|=%.2e at [%d] (got %.12g, exp %.12g)", _w, _wi, (got)[_wi], (exp)[_wi]); \
} while(0)

/** Assert two complex double arrays are close within absolute tolerance */
#define ASSERT_EQ_CPX(got, exp, n, tol, label) do { \
    t_start(label); \
    if ((got) == NULL) { t_fail("got NULL array"); break; } \
    if ((n) == 0) { t_pass(); break; } \
    double _w = 0; int _wi = 0; \
    for (int _i = 0; _i < (n); _i++) { \
        double _d = cabs((got)[_i] - (exp)[_i]); \
        if (_d > _w) { _w = _d; _wi = _i; } \
    } \
    if (_w <= (tol)) t_pass(); \
    else t_fail("max|cabs|=%.2e at [%d]", _w, _wi); \
} while(0)

/** Assert two int arrays are exactly equal */
#define ASSERT_EQ_IARR(got, exp, n, label) do { \
    t_start(label); \
    if ((got) == NULL) { t_fail("got NULL array"); break; } \
    int _mismatch = 0; int _mi = 0; \
    for (int _i = 0; _i < (n); _i++) { \
        if ((got)[_i] != (exp)[_i]) { _mismatch = 1; _mi = _i; break; } \
    } \
    if (!_mismatch) t_pass(); \
    else t_fail("mismatch at [%d]: got %d, expected %d", _mi, (got)[_mi], (exp)[_mi]); \
} while(0)

/** Assert Array has expected dtype */
#define ASSERT_DTYPE(arr, exp_dtype, label) do { \
    t_start(label); \
    if ((arr) == NULL) { t_fail("got NULL array"); break; } \
    if ((arr)->dtype == (exp_dtype)) t_pass(); \
    else t_fail("dtype %d, expected %d", (arr)->dtype, (int)(exp_dtype)); \
} while(0)

/** Assert Array has expected size */
#define ASSERT_SIZE(arr, exp_size, label) do { \
    t_start(label); \
    if ((arr) == NULL) { t_fail("got NULL array"); break; } \
    if ((arr)->size == (exp_size)) t_pass(); \
    else t_fail("size %d, expected %d", (arr)->size, (int)(exp_size)); \
} while(0)

/** Assert Array has expected ndim */
#define ASSERT_NDIM(arr, exp_ndim, label) do { \
    t_start(label); \
    if ((arr) == NULL) { t_fail("got NULL array"); break; } \
    if ((arr)->ndim == (exp_ndim)) t_pass(); \
    else t_fail("ndim %d, expected %d", (arr)->ndim, (int)(exp_ndim)); \
} while(0)

/* ─── Tolerance constants ─────────────────────────────────────────── */

#define TOL_F64  1e-12
#define TOL_F32  5e-6
#define TOL_FFT  1e-10
#define TOL_SVD  1e-8

/* ─── Helper: create simple 1D float64 array from double data ─────── */

static inline Array *make_f64_1d(const double *data, int n) {
    Array *a = create_array((int[]){n}, 1, FLOAT64);
    if (a && data) memcpy(a->data, data, n * sizeof(double));
    return a;
}

static inline Array *make_i32_1d(const int *data, int n) {
    Array *a = create_array((int[]){n}, 1, INT32);
    if (a && data) memcpy(a->data, data, n * sizeof(int));
    return a;
}

static inline Array *make_f64_2d(const double *data, int r, int c) {
    Array *a = create_array((int[]){r, c}, 2, FLOAT64);
    if (a && data) memcpy(a->data, data, r * c * sizeof(double));
    return a;
}

static inline Array *make_i32_2d(const int *data, int r, int c) {
    Array *a = create_array((int[]){r, c}, 2, INT32);
    if (a && data) memcpy(a->data, data, r * c * sizeof(int));
    return a;
}

static inline Array *make_c128_1d(const complex double *data, int n) {
    Array *a = create_array((int[]){n}, 1, COMPLEX128);
    if (a && data) memcpy(a->data, data, n * sizeof(complex double));
    return a;
}

/** Free an array and set pointer to NULL (safe). Always returns NULL for chaining. */
static inline void *free_a(Array *a) {
    if (a) free_array(a);
    return NULL;
}

#endif /* TEST_UTILS_H */
