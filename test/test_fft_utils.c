/**
 * test_fft_utils.c — Tests for fftshift, ifftshift, fftfreq, rfftfreq
 */

#include "test_utils.h"

int main(void) {
    TEST_MAIN("FFT Utilities");

    /* ── fftshift: even length ──────────────────────────────────────── */
    TEST_SECTION("fftshift");

    {
        /* fftshift([1,2,3,4]) = [3,4,1,2] (even) */
        Array *a = make_f64_1d((double[]){1, 2, 3, 4}, 4);
        Array *r = fftshift(a);
        ASSERT_NOTNULL(r, "fftshift even result not null");
        ASSERT_DTYPE(r, FLOAT64, "fftshift preserves FLOAT64 dtype");
        ASSERT_EQ_ARR((double*)r->data,
                      ((double[]){3, 4, 1, 2}), 4, TOL_F64,
                      "fftshift [1,2,3,4] -> [3,4,1,2]");
        free_a(r); free_a(a);
    }

    {
        /* fftshift([1,2,3,4,5]) = [4,5,1,2,3] (odd) */
        Array *a = make_f64_1d((double[]){1, 2, 3, 4, 5}, 5);
        Array *r = fftshift(a);
        ASSERT_NOTNULL(r, "fftshift odd result not null");
        ASSERT_EQ_ARR((double*)r->data,
                      ((double[]){4, 5, 1, 2, 3}), 5, TOL_F64,
                      "fftshift [1,2,3,4,5] -> [4,5,1,2,3]");
        free_a(r); free_a(a);
    }

    {
        /* fftshift 2D */
        Array *a = make_f64_2d((double[]){1, 2, 3, 4}, 2, 2);
        Array *r = fftshift(a);
        ASSERT_NOTNULL(r, "fftshift 2D result not null");
        ASSERT_NDIM(r, 2, "fftshift 2D output is 2D");
        ASSERT_EQ_ARR((double*)r->data,
                      ((double[]){4, 3, 2, 1}), 4, TOL_F64,
                      "fftshift 2x2 quadrant swap");
        free_a(r); free_a(a);
    }

    /* ── ifftshift: inverse of fftshift ─────────────────────────────── */
    TEST_SECTION("ifftshift");

    {
        /* ifftshift(fftshift(x)) = x */
        Array *a = make_f64_1d((double[]){1, 2, 3, 4}, 4);
        Array *shifted = fftshift(a);
        ASSERT_NOTNULL(shifted, "fftshift even for inv not null");
        Array *r = ifftshift(shifted);
        ASSERT_NOTNULL(r, "ifftshift result not null");
        ASSERT_EQ_ARR((double*)r->data,
                      ((double[]){1, 2, 3, 4}), 4, TOL_F64,
                      "ifftshift(fftshift(x)) recovers even x");
        free_a(r); free_a(shifted); free_a(a);
    }

    {
        /* ifftshift(fftshift(x)) = x for odd */
        Array *a = make_f64_1d((double[]){1, 2, 3, 4, 5}, 5);
        Array *shifted = fftshift(a);
        ASSERT_NOTNULL(shifted, "fftshift odd for inv not null");
        Array *r = ifftshift(shifted);
        ASSERT_NOTNULL(r, "ifftshift odd result not null");
        ASSERT_EQ_ARR((double*)r->data,
                      ((double[]){1, 2, 3, 4, 5}), 5, TOL_F64,
                      "ifftshift(fftshift(x)) recovers odd x");
        free_a(r); free_a(shifted); free_a(a);
    }

    {
        /* ifftshift(fftshift(x)) = x for 2D */
        Array *a = make_f64_2d((double[]){1, 2, 3, 4}, 2, 2);
        Array *shifted = fftshift(a);
        ASSERT_NOTNULL(shifted, "fftshift 2D for inv not null");
        Array *r = ifftshift(shifted);
        ASSERT_NOTNULL(r, "ifftshift 2D result not null");
        ASSERT_EQ_ARR((double*)r->data,
                      ((double[]){1, 2, 3, 4}), 4, TOL_F64,
                      "ifftshift(fftshift 2D) recovers original");
        free_a(r); free_a(shifted); free_a(a);
    }

    /* test INT32 input preserves behavior */
    {
        Array *a = make_i32_1d((int[]){1, 2, 3, 4}, 4);
        Array *shifted = fftshift(a);
        ASSERT_NOTNULL(shifted, "fftshift i32 result not null");
        Array *r = ifftshift(shifted);
        ASSERT_NOTNULL(r, "ifftshift i32 result not null");
        ASSERT_EQ_IARR((int*)r->data,
                      ((int[]){1, 2, 3, 4}), 4,
                      "ifftshift(fftshift i32) recovers original");
        free_a(r); free_a(shifted); free_a(a);
    }

    /* ── fftfreq ────────────────────────────────────────────────────── */
    TEST_SECTION("fftfreq");

    {
        /* fftfreq(4, 1.0) = [0, 0.25, -0.5, -0.25] */
        Array *r = fftfreq(4, 1.0);
        ASSERT_NOTNULL(r, "fftfreq n=4 result not null");
        ASSERT_DTYPE(r, FLOAT64, "fftfreq output is FLOAT64");
        ASSERT_EQ_ARR((double*)r->data,
                      ((double[]){0.0, 0.25, -0.5, -0.25}), 4, TOL_F64,
                      "fftfreq n=4 d=1 -> [0,0.25,-0.5,-0.25]");
        free_a(r);
    }

    {
        /* fftfreq(4, 2.0) = [0, 0.125, -0.25, -0.125] */
        Array *r = fftfreq(4, 2.0);
        ASSERT_NOTNULL(r, "fftfreq n=4 d=2 result not null");
        ASSERT_EQ_ARR((double*)r->data,
                      ((double[]){0.0, 0.125, -0.25, -0.125}), 4, TOL_F64,
                      "fftfreq n=4 d=2 -> [0,0.125,-0.25,-0.125]");
        free_a(r);
    }

    {
        /* fftfreq odd: n=5 d=1 */
        Array *r = fftfreq(5, 1.0);
        ASSERT_NOTNULL(r, "fftfreq n=5 result not null");
        /* numpy.fft.fftfreq(5) = [0., 0.2, 0.4, -0.4, -0.2] */
        ASSERT_EQ_ARR((double*)r->data,
                      ((double[]){0.0, 0.2, 0.4, -0.4, -0.2}), 5, TOL_F64,
                      "fftfreq n=5 -> [0,0.2,0.4,-0.4,-0.2]");
        free_a(r);
    }

    {
        /* fftfreq with INT32 input */
        Array *r = fftfreq(8, 1.0);
        ASSERT_NOTNULL(r, "fftfreq n=8 result not null");
        ASSERT_SIZE(r, 8, "fftfreq n=8 size=8");
        /* n=8: [0, 1/8, 2/8, 3/8, -4/8, -3/8, -2/8, -1/8] */
        double f8_exp[] = {0.0, 0.125, 0.25, 0.375,
                           -0.5, -0.375, -0.25, -0.125};
        ASSERT_EQ_ARR((double*)r->data, f8_exp, 8, TOL_F64,
                      "fftfreq n=8");
        free_a(r);
    }

    /* ── rfftfreq ───────────────────────────────────────────────────── */
    TEST_SECTION("rfftfreq");

    {
        /* rfftfreq(4, 1.0) = [0, 0.25, 0.5] (only non-negative freqs) */
        Array *r = rfftfreq(4, 1.0);
        ASSERT_NOTNULL(r, "rfftfreq n=4 result not null");
        ASSERT_DTYPE(r, FLOAT64, "rfftfreq output is FLOAT64");
        ASSERT_EQ_ARR((double*)r->data,
                      ((double[]){0.0, 0.25, 0.5}), 3, TOL_F64,
                      "rfftfreq n=4 -> [0,0.25,0.5]");
        free_a(r);
    }

    {
        /* rfftfreq(8, 1.0) */
        Array *r = rfftfreq(8, 1.0);
        ASSERT_NOTNULL(r, "rfftfreq n=8 result not null");
        ASSERT_SIZE(r, 5, "rfftfreq n=8 size=5 (n//2+1)");
        ASSERT_EQ_ARR((double*)r->data,
                      ((double[]){0.0, 0.125, 0.25, 0.375, 0.5}), 5, TOL_F64,
                      "rfftfreq n=8 -> [0,0.125,0.25,0.375,0.5]");
        free_a(r);
    }

    {
        /* rfftfreq(5, 1.0) odd */
        Array *r = rfftfreq(5, 1.0);
        ASSERT_NOTNULL(r, "rfftfreq n=5 result not null");
        /* n=5: [0, 1/5, 2/5] = [0, 0.2, 0.4] */
        ASSERT_EQ_ARR((double*)r->data,
                      ((double[]){0.0, 0.2, 0.4}), 3, TOL_F64,
                      "rfftfreq n=5 -> [0,0.2,0.4]");
        free_a(r);
    }

    {
        /* rfftfreq with d=0.5 */
        Array *r = rfftfreq(4, 0.5);
        ASSERT_NOTNULL(r, "rfftfreq n=4 d=0.5 result not null");
        ASSERT_EQ_ARR((double*)r->data,
                      ((double[]){0.0, 0.5, 1.0}), 3, TOL_F64,
                      "rfftfreq n=4 d=0.5 -> [0,0.5,1.0]");
        free_a(r);
    }

    /* ── consistent freqs in fftfreq and rfftfreq ───────────────────── */
    TEST_SECTION("freqs consistency");

    {
        /* rfftfreq should match first half+1 of fftfreq (non-negative part) */
        Array *ff = fftfreq(6, 1.0);
        Array *rf = rfftfreq(6, 1.0);
        ASSERT_NOTNULL(ff, "fftfreq not null");
        ASSERT_NOTNULL(rf, "rfftfreq not null");

        /* fftfreq(6) = [0, 1/6, 2/6, -3/6, -2/6, -1/6]
           rfftfreq(6) = [0, 1/6, 2/6, 3/6]  (abs of Nyquist) */
        /* Actually numpy rfftfreq(6) = [0, 0.1667, 0.3333, 0.5] */
        double *freqs = (double*)ff->data;
        double *rfreqs = (double*)rf->data;

        /* First rf->size elements should match */
        ASSERT_EQ_DBL(rfreqs[0], freqs[0], TOL_F64, "rfftfreq[0] = fftfreq[0] = 0");
        ASSERT_EQ_DBL(rfreqs[1], freqs[1], TOL_F64, "rfftfreq[1] = fftfreq[1]");
        ASSERT_EQ_DBL(rfreqs[2], freqs[2], TOL_F64, "rfftfreq[2] = fftfreq[2]");
        /* Nyquist: rfftfreq[3] = 0.5, fftfreq[3] = -0.5 -> abs values match */
        ASSERT_EQ_DBL(rfreqs[3], 0.5, TOL_F64, "rfftfreq n=6 nyquist = 0.5");

        free_a(rf); free_a(ff);
    }

    TEST_SUMMARY();
    return (g_fail > 0) ? 1 : 0;
}
