/**
 * test_misc.c — Tests for i0, bessel_i0, gcd_array, lcm, aldexp,
 *               trim_zeros, maximum, minimum, bmat, moveaxis, rollaxis,
 *               ptp, trapz, gradient, unique, setdiff1d, intersect1d
 */
#include "test_utils.h"
#include <stdint.h>

int main(void) {
    TEST_MAIN("i0 / bessel_i0 / gcd_array / lcm / aldexp / trim_zeros / "
              "maximum / minimum / bmat / moveaxis / rollaxis / ptp / trapz / "
              "gradient / unique / setdiff1d / intersect1d");

    /* ── i0 ─────────────────────────────────────────────────────────── */
    {
        Array *a = make_f64_1d((double[]){0.0, 1.0, 2.0}, 3);
        Array *r = i0(a);
        ASSERT_NOTNULL(r, "i0 [0,1,2]");
        if (r) {
            double *d = (double*)r->data;
            double expected[] = {1.0, 1.2660658777520084, 2.2795853023360668};
            ASSERT_EQ_ARR(d, expected, 3, 1e-6, "i0 values");
        }
        free_a(a); free_a(r);
    }

    /* ── bessel_i0 scalar ───────────────────────────────────────────── */
    {
        double v0 = bessel_i0(0.0);
        ASSERT_EQ_DBL(v0, 1.0, TOL_F64, "bessel_i0(0)=1");
        double v1 = bessel_i0(1.0);
        ASSERT_EQ_DBL(v1, 1.2660658777520084, 1e-6, "bessel_i0(1)~1.266");
    }

    /* ── gcd_array ──────────────────────────────────────────────────── */
    {
        Array *a = make_i32_1d((int[]){12, 18, 24}, 3);
        Array *b = make_i32_1d((int[]){8, 12, 16}, 3);
        Array *r = gcd_array(a, b);
        ASSERT_NOTNULL(r, "gcd_array");
        if (r) {
            int expected[] = {4, 6, 8};
            ASSERT_EQ_IARR((int*)r->data, expected, 3, "gcd_array values");
        }
        free_a(a); free_a(b); free_a(r);
    }

    /* ── lcm (returns INT64) ─────────────────────────────────────────── */
    {
        Array *a = make_i32_1d((int[]){2, 3, 4}, 3);
        Array *b = make_i32_1d((int[]){3, 4, 5}, 3);
        Array *r = lcm(a, b);
        ASSERT_NOTNULL(r, "lcm [2,3,4] [3,4,5]");
        if (r) {
            ASSERT_DTYPE(r, INT64, "lcm dtype=INT64");
            int64_t *d = (int64_t*)r->data;
            ASSERT_EQ_I64(d[0], 6, "lcm[0]");
            ASSERT_EQ_I64(d[1], 12, "lcm[1]");
            ASSERT_EQ_I64(d[2], 20, "lcm[2]");
        }
        free_a(a); free_a(b); free_a(r);
    }

    /* ── aldexp (exponent must be integer type) ─────────────────────────── */
    {
        Array *a = make_f64_1d((double[]){1.0, 2.0, 3.0}, 3);
        Array *b = make_i32_1d((int[]){1, 2, 3}, 3);
        Array *r = aldexp(a, b);
        ASSERT_NOTNULL(r, "aldexp");
        if (r) {
            ASSERT_SIZE(r, 3, "aldexp size");
            ASSERT_DTYPE(r, FLOAT64, "aldexp dtype");
        }
        free_a(a); free_a(b); free_a(r);
    }

    /* ── trim_zeros trim="fb" ───────────────────────────────────────── */
    {
        Array *a = make_i32_1d((int[]){0, 0, 1, 2, 3, 0, 0}, 7);
        Array *r = trim_zeros(a, "fb");
        ASSERT_NOTNULL(r, "trim_zeros fb [0,0,1,2,3,0,0]");
        if (r) {
            int expected[] = {1, 2, 3};
            ASSERT_EQ_IARR((int*)r->data, expected, 3, "trim_zeros fb values");
        }
        free_a(a); free_a(r);
    }

    /* ── trim_zeros trim="f" ────────────────────────────────────────── */
    {
        Array *a = make_i32_1d((int[]){0, 0, 1, 2, 3, 0, 0}, 7);
        Array *r = trim_zeros(a, "f");
        ASSERT_NOTNULL(r, "trim_zeros f");
        if (r) {
            int expected[] = {1, 2, 3, 0, 0};
            ASSERT_EQ_IARR((int*)r->data, expected, 5, "trim_zeros f values");
        }
        free_a(a); free_a(r);
    }

    /* ── trim_zeros trim="b" ────────────────────────────────────────── */
    {
        Array *a = make_i32_1d((int[]){0, 0, 1, 2, 3, 0, 0}, 7);
        Array *r = trim_zeros(a, "b");
        ASSERT_NOTNULL(r, "trim_zeros b");
        if (r) {
            int expected[] = {0, 0, 1, 2, 3};
            ASSERT_EQ_IARR((int*)r->data, expected, 5, "trim_zeros b values");
        }
        free_a(a); free_a(r);
    }

    /* ── maximum ────────────────────────────────────────────────────── */
    {
        Array *a = make_i32_1d((int[]){1, 5, 3}, 3);
        Array *b = make_i32_1d((int[]){2, 3, 4}, 3);
        Array *r = maximum(a, b);
        ASSERT_NOTNULL(r, "maximum [1,5,3] [2,3,4]");
        if (r) {
            int expected[] = {2, 5, 4};
            ASSERT_EQ_IARR((int*)r->data, expected, 3, "maximum values");
        }
        free_a(a); free_a(b); free_a(r);
    }

    /* ── minimum ────────────────────────────────────────────────────── */
    {
        Array *a = make_i32_1d((int[]){1, 5, 3}, 3);
        Array *b = make_i32_1d((int[]){2, 3, 4}, 3);
        Array *r = minimum(a, b);
        ASSERT_NOTNULL(r, "minimum [1,5,3] [2,3,4]");
        if (r) {
            int expected[] = {1, 3, 3};
            ASSERT_EQ_IARR((int*)r->data, expected, 3, "minimum values");
        }
        free_a(a); free_a(b); free_a(r);
    }

    /* ── bmat ───────────────────────────────────────────────────────── */
    {
        Array *a11 = make_i32_2d((int[]){1, 2, 3, 4}, 2, 2);
        Array *a12 = make_i32_2d((int[]){5, 6, 7, 8}, 2, 2);
        Array *a21 = make_i32_2d((int[]){9, 10, 11, 12}, 2, 2);
        Array *a22 = make_i32_2d((int[]){13, 14, 15, 16}, 2, 2);
        Array *blocks[] = {a11, a12, a21, a22};
        Array *r = bmat(blocks, 2, 2);
        ASSERT_NOTNULL(r, "bmat 2x2 blocks");
        if (r) {
            ASSERT_NDIM(r, 2, "bmat ndim");
            ASSERT_SIZE(r, 16, "bmat size");
            int *d = (int*)r->data;
            int expected[] = {1, 2, 5, 6, 3, 4, 7, 8, 9, 10, 13, 14, 11, 12, 15, 16};
            ASSERT_EQ_IARR(d, expected, 16, "bmat values");
        }
        free_a(a11); free_a(a12); free_a(a21); free_a(a22); free_a(r);
    }

    /* ── moveaxis 3D ────────────────────────────────────────────────── */
    {
        int shape[] = {2, 3, 4};
        Array *a = create_array(shape, 3, INT32);
        if (a) {
            /* fill with sequential values */
            for (int i = 0; i < 24; i++) ((int*)a->data)[i] = i;
            Array *r = moveaxis(a, 0, 2);
            ASSERT_NOTNULL(r, "moveaxis source=0 dest=2");
            if (r) {
                ASSERT_NDIM(r, 3, "moveaxis ndim");
                /* shape should be (3, 4, 2) */
                ASSERT_EQ_INT(r->shape[0], 3, "moveaxis shape[0]");
                ASSERT_EQ_INT(r->shape[1], 4, "moveaxis shape[1]");
                ASSERT_EQ_INT(r->shape[2], 2, "moveaxis shape[2]");
            }
            free_a(a); free_a(r);
        }
    }

    /* ── rollaxis 3D ────────────────────────────────────────────────── */
    {
        int shape[] = {2, 3, 4};
        Array *a = create_array(shape, 3, INT32);
        if (a) {
            for (int i = 0; i < 24; i++) ((int*)a->data)[i] = i;
            Array *r = rollaxis(a, 2, 0); /* move axis 2 to position 0 */
            ASSERT_NOTNULL(r, "rollaxis axis=2 start=0");
            if (r) {
                ASSERT_NDIM(r, 3, "rollaxis ndim");
                /* shape should be (4, 2, 3) */
                ASSERT_EQ_INT(r->shape[0], 4, "rollaxis shape[0]");
                ASSERT_EQ_INT(r->shape[1], 2, "rollaxis shape[1]");
                ASSERT_EQ_INT(r->shape[2], 3, "rollaxis shape[2]");
            }
            free_a(a); free_a(r);
        }
    }

    /* ── ptp 1D (returns FLOAT64 scalar) ────────────────────────────── */
    {
        Array *a = make_i32_1d((int[]){1, 5, 3, 9, 2}, 5);
        Array *r = ptp(a, -1, 0);
        ASSERT_NOTNULL(r, "ptp [1,5,3,9,2]");
        if (r) {
            ASSERT_DTYPE(r, FLOAT64, "ptp dtype=FLOAT64");
            ASSERT_EQ_DBL(*(double*)r->data, 8.0, TOL_F64, "ptp value = 8");
        }
        free_a(a); free_a(r);
    }

    /* ── ptp 2D axis=0 (falls back to full ptp) ───────────────────── */
    {
        /* [[1, 5],
         *  [3, 9],
         *  [2, 7]] */
        Array *a = make_i32_2d((int[]){1, 5, 3, 9, 2, 7}, 3, 2);
        Array *r = ptp(a, 0, 0);
        ASSERT_NOTNULL(r, "ptp 2D axis=0");
        if (r) {
            ASSERT_DTYPE(r, FLOAT64, "ptp 2D dtype=FLOAT64");
            ASSERT_EQ_DBL(*(double*)r->data, 8.0, TOL_F64, "ptp 2D axis=0 value");
        }
        free_a(a); free_a(r);
    }

    /* ── trapz ──────────────────────────────────────────────────────── */
    {
        Array *y = make_f64_1d((double[]){1.0, 2.0, 3.0}, 3);
        Array *x = make_f64_1d((double[]){0.0, 1.0, 2.0}, 3);
        Array *r = trapz(y, x, 1.0, -1);
        ASSERT_NOTNULL(r, "trapz y=[1,2,3] x=[0,1,2]");
        if (r) {
            double expected = 4.0; /* (1+2)/2*1 + (2+3)/2*1 = 1.5+2.5 = 4 */
            ASSERT_EQ_DBL(*(double*)r->data, expected, TOL_F64, "trapz value = 4");
        }
        free_a(y); free_a(x); free_a(r);
    }

    /* ── trapz with dx ──────────────────────────────────────────────── */
    {
        Array *y = make_f64_1d((double[]){1.0, 2.0, 3.0}, 3);
        Array *r = trapz(y, NULL, 2.0, -1);
        ASSERT_NOTNULL(r, "trapz y=[1,2,3] dx=2.0");
        if (r) {
            double expected = 8.0; /* (1+2)/2*2 + (2+3)/2*2 = 3+5 = 8 */
            ASSERT_EQ_DBL(*(double*)r->data, expected, TOL_F64, "trapz dx value = 8");
        }
        free_a(y); free_a(r);
    }

    /* ── gradient ───────────────────────────────────────────────────── */
    {
        Array *a = make_f64_1d((double[]){1.0, 2.0, 4.0, 7.0}, 4);
        Array **out = NULL;
        int num_out = 0;
        int ret = gradient(a, NULL, &out, &num_out);
        ASSERT_EQ_INT(ret, 0, "gradient returns 0");
        ASSERT_NOTNULL(out, "gradient out not NULL");
        if (out) {
            ASSERT_TRUE(num_out >= 1, "gradient num_out >= 1");
            if (num_out >= 1 && out[0]) {
                ASSERT_SIZE(out[0], 4, "gradient size");
            }
            for (int i = 0; i < num_out; i++) free_a(out[i]);
            free(out);
        }
        free_a(a);
    }

    /* ── gradient with spacing (0-d scalar) ───────────────────────────── */
    {
        Array *a = make_f64_1d((double[]){1.0, 2.0, 4.0, 7.0}, 4);
        double scalar = 2.0;
        Array *sp = create_scalar_array(&scalar, FLOAT64);
        Array **out = NULL;
        int num_out = 0;
        int ret = gradient(a, sp, &out, &num_out);
        ASSERT_EQ_INT(ret, 0, "gradient with spacing returns 0");
        if (out) {
            for (int i = 0; i < num_out; i++) free_a(out[i]);
            free(out);
        }
        free_a(a); free_a(sp);
    }

    /* ── unique (returns FLOAT64 values, INT64 counts) ─────────────── */
    {
        Array *a = make_i32_1d((int[]){3, 1, 2, 1, 3}, 5);
        UniqueResult r = unique(a, 0, 0, 0);
        ASSERT_NOTNULL(r.values, "unique [3,1,2,1,3] values");
        if (r.values) {
            ASSERT_DTYPE(r.values, FLOAT64, "unique dtype=FLOAT64");
            double expected[] = {1, 2, 3};
            ASSERT_EQ_ARR((double*)r.values->data, expected, 3, TOL_F64, "unique sorted values");
        }
        free_a(a); free_a(r.values); free_a(r.indices);
        free_a(r.inverse); free_a(r.counts);
    }

    /* ── unique with return_index ────────────────────────────────────── */
    {
        Array *a = make_i32_1d((int[]){3, 1, 2, 1, 3}, 5);
        UniqueResult r = unique(a, 1, 0, 0);
        ASSERT_NOTNULL(r.values, "unique with return_index");
        ASSERT_NOTNULL(r.indices, "unique indices not NULL");
        free_a(a); free_a(r.values); free_a(r.indices);
        free_a(r.inverse); free_a(r.counts);
    }

    /* ── unique with return_counts (counts are INT64) ────────────────── */
    {
        Array *a = make_i32_1d((int[]){3, 1, 2, 1, 3}, 5);
        UniqueResult r = unique(a, 0, 0, 1);
        ASSERT_NOTNULL(r.values, "unique with return_counts");
        ASSERT_NOTNULL(r.counts, "unique counts not NULL");
        if (r.counts) {
            int64_t *d = (int64_t*)r.counts->data;
            ASSERT_EQ_I64(d[0], 2, "unique counts[0]"); /* 1 appears 2x */
            ASSERT_EQ_I64(d[1], 1, "unique counts[1]"); /* 2 appears 1x */
            ASSERT_EQ_I64(d[2], 2, "unique counts[2]"); /* 3 appears 2x */
        }
        free_a(a); free_a(r.values); free_a(r.indices);
        free_a(r.inverse); free_a(r.counts);
    }

    /* ── setdiff1d (returns FLOAT64) ───────────────────────────────── */
    {
        Array *a = make_i32_1d((int[]){1, 2, 3, 4}, 4);
        Array *b = make_i32_1d((int[]){2, 4}, 2);
        Array *r = setdiff1d(a, b);
        ASSERT_NOTNULL(r, "setdiff1d [1,2,3,4] \\ [2,4]");
        if (r) {
            double expected[] = {1, 3};
            ASSERT_EQ_ARR((double*)r->data, expected, 2, TOL_F64, "setdiff1d values");
        }
        free_a(a); free_a(b); free_a(r);
    }

    /* ── intersect1d (returns FLOAT64 for integer input) ────────────── */
    {
        Array *a = make_i32_1d((int[]){1, 2, 3, 4}, 4);
        Array *b = make_i32_1d((int[]){3, 4, 5, 6}, 4);
        Array *r = intersect1d(a, b, 0);
        ASSERT_NOTNULL(r, "intersect1d");
        if (r) {
            double expected[] = {3, 4};
            ASSERT_EQ_ARR((double*)r->data, expected, 2, TOL_F64, "intersect1d values");
        }
        free_a(a); free_a(b); free_a(r);
    }

    /* ── intersect1d assume_unique=1 (returns FLOAT64) ─────────────── */
    {
        Array *a = make_i32_1d((int[]){1, 2, 3, 4}, 4);
        Array *b = make_i32_1d((int[]){2, 4, 6, 8}, 4);
        Array *r = intersect1d(a, b, 1);
        ASSERT_NOTNULL(r, "intersect1d assume_unique=1");
        if (r) {
            double expected[] = {2, 4};
            ASSERT_EQ_ARR((double*)r->data, expected, 2, TOL_F64, "intersect1d assume_unique values");
        }
        free_a(a); free_a(b); free_a(r);
    }

    /* ── bmat single block (returns NULL: requires >1 col) ──────────── */
    {
        Array *a = make_i32_2d((int[]){1, 2, 3, 4}, 2, 2);
        Array *blocks[] = {a};
        Array *r = bmat(blocks, 1, 1);
        ASSERT_NULL(r, "bmat 1x1 block returns NULL (not supported)");
        free_a(a); free_a(r);
    }

    /* ── FLOAT64 maximum/minimum ────────────────────────────────────── */
    {
        Array *a = make_f64_1d((double[]){1.5, 2.5, 3.5}, 3);
        Array *b = make_f64_1d((double[]){2.0, 2.0, 2.0}, 3);
        Array *mx = maximum(a, b);
        ASSERT_NOTNULL(mx, "maximum FLOAT64");
        if (mx) {
            double expected[] = {2.0, 2.5, 3.5};
            ASSERT_EQ_ARR((double*)mx->data, expected, 3, TOL_F64, "maximum FLOAT64 values");
        }
        Array *mn = minimum(a, b);
        ASSERT_NOTNULL(mn, "minimum FLOAT64");
        if (mn) {
            double expected[] = {1.5, 2.0, 2.0};
            ASSERT_EQ_ARR((double*)mn->data, expected, 3, TOL_F64, "minimum FLOAT64 values");
        }
        free_a(a); free_a(b); free_a(mx); free_a(mn);
    }

    TEST_SUMMARY();
    return (g_fail > 0) ? 1 : 0;
}
