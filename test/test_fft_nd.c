/**
 * test_fft_nd.c — Tests for fft2, ifft2, fftn, ifftn, rfft2, irfft2, rfftn, irfftn
 */

#include "test_utils.h"

int main(void) {
    TEST_MAIN("FFT N-Dimensional");

    /* ── fft2: 2D FFT ───────────────────────────────────────────────── */
    TEST_SECTION("fft2");

    {
        Array *a = make_f64_2d((double[]){1, 2, 3, 4}, 2, 2);
        Array *r = fft2(a);
        ASSERT_NOTNULL(r, "fft2 result not null");
        ASSERT_DTYPE(r, COMPLEX128, "fft2 output is COMPLEX128");
        ASSERT_NDIM(r, 2, "fft2 output is 2D");
        ASSERT_EQ_INT(r->shape[0], 2, "fft2 rows = 2");
        ASSERT_EQ_INT(r->shape[1], 2, "fft2 cols = 2");
        free_a(r); free_a(a);
    }

    {
        /* fft2 of 3x3 */
        Array *a = make_f64_2d((double[]){1, 2, 3,
                                          4, 5, 6,
                                          7, 8, 9}, 3, 3);
        Array *r = fft2(a);
        ASSERT_NOTNULL(r, "fft2 3x3 result not null");
        ASSERT_NDIM(r, 2, "fft2 3x3 output is 2D");
        ASSERT_EQ_INT(r->shape[0], 3, "fft2 3x3 rows=3");
        ASSERT_EQ_INT(r->shape[1], 3, "fft2 3x3 cols=3");
        free_a(r); free_a(a);
    }

    /* ── ifft2 round-trip ───────────────────────────────────────────── */
    TEST_SECTION("ifft2 round-trip");

    {
        Array *a = make_f64_2d((double[]){1, 2, 3, 4}, 2, 2);
        Array *f = fft2(a);
        ASSERT_NOTNULL(f, "fft2 for round-trip not null");
        Array *b = ifft2(f);
        ASSERT_NOTNULL(b, "ifft2 result not null");

        complex double *bd = (complex double*)b->data;
        double original[] = {1, 2, 3, 4};
        for (int i = 0; i < 4; i++) {
            ASSERT_TRUE(fabs(creal(bd[i]) - original[i]) < TOL_FFT
                        && fabs(cimag(bd[i])) < TOL_FFT,
                        "ifft2(fft2) round-trip recovers original");
        }

        free_a(b); free_a(f); free_a(a);
    }

    {
        /* 3x3 round-trip */
        Array *a = make_f64_2d((double[]){1, 2, 3,
                                          4, 5, 6,
                                          7, 8, 9}, 3, 3);
        Array *f = fft2(a);
        ASSERT_NOTNULL(f, "fft2 3x3 round-trip not null");
        Array *b = ifft2(f);
        ASSERT_NOTNULL(b, "ifft2 3x3 result not null");

        double orig[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
        complex double *bd = (complex double*)b->data;
        for (int i = 0; i < 9; i++) {
            ASSERT_TRUE(fabs(creal(bd[i]) - orig[i]) < TOL_FFT
                        && fabs(cimag(bd[i])) < TOL_FFT,
                        "ifft2(fft2) 3x3 round-trip");
        }

        free_a(b); free_a(f); free_a(a);
    }

    /* ── fftn: N-D FFT ──────────────────────────────────────────────── */
    TEST_SECTION("fftn");

    {
        /* fftn on 2D should be same as fft2 */
        Array *a = make_f64_2d((double[]){1, 2, 3, 4}, 2, 2);
        Array *r = fftn(a);
        ASSERT_NOTNULL(r, "fftn 2D result not null");
        ASSERT_DTYPE(r, COMPLEX128, "fftn output is COMPLEX128");
        ASSERT_NDIM(r, 2, "fftn output preserves ndim");
        free_a(r); free_a(a);
    }

    {
        /* fftn on 3D array (2x2x2) */
        Array *a = create_array((int[]){2, 2, 2}, 3, FLOAT64);
        if (a) {
            double *d = (double*)a->data;
            for (int i = 0; i < 8; i++) d[i] = (double)(i + 1);
            Array *r = fftn(a);
            ASSERT_NOTNULL(r, "fftn 3D result not null");
            ASSERT_NDIM(r, 3, "fftn 3D output is 3D");
            ASSERT_EQ_INT(r->size, 8, "fftn 3D output size 8");
            free_a(r);
        }
        free_a(a);
    }

    /* ── ifftn round-trip ───────────────────────────────────────────── */
    TEST_SECTION("ifftn round-trip");

    {
        Array *a = make_f64_2d((double[]){1, 2, 3, 4}, 2, 2);
        Array *f = fftn(a);
        ASSERT_NOTNULL(f, "fftn 2D not null");
        Array *b = ifftn(f);
        ASSERT_NOTNULL(b, "ifftn 2D result not null");

        complex double *bd = (complex double*)b->data;
        for (int i = 0; i < 4; i++) {
            ASSERT_TRUE(fabs(creal(bd[i]) - (i + 1)) < TOL_FFT
                        && fabs(cimag(bd[i])) < TOL_FFT,
                        "ifftn(fftn) 2D round-trip");
        }

        free_a(b); free_a(f); free_a(a);
    }

    /* ── rfft2: 2D real FFT ─────────────────────────────────────────── */
    TEST_SECTION("rfft2");

    {
        /* rfft2: real input -> shorter output in last dimension */
        Array *a = make_f64_2d((double[]){1, 2, 3, 4, 5, 6}, 3, 2);
        Array *r = rfft2(a);
        ASSERT_NOTNULL(r, "rfft2 result not null");
        ASSERT_DTYPE(r, COMPLEX128, "rfft2 output is COMPLEX128");
        ASSERT_NDIM(r, 2, "rfft2 output is 2D");
        /* n=2 -> last dim = 2//2+1 = 2, but n is cols=2 */
        /* Actually rfft2 transforms along last axis,
           cols=2 -> output cols = 2//2+1 = 2 (same for tiny n) */
        free_a(r); free_a(a);
    }

    {
        /* rfft2 with larger last dimension */
        Array *a = make_f64_2d((double[]){1, 2, 3, 4,
                                          5, 6, 7, 8}, 2, 4);
        Array *r = rfft2(a);
        ASSERT_NOTNULL(r, "rfft2 2x4 result not null");
        /* cols=4 -> last dim = 4//2+1 = 3 */
        ASSERT_EQ_INT(r->shape[0], 2, "rfft2 2x4 rows=2");
        ASSERT_EQ_INT(r->shape[1], 3, "rfft2 2x4 cols=3 (4//2+1)");
        free_a(r); free_a(a);
    }

    /* ── irfft2: round-trip ─────────────────────────────────────────── */
    TEST_SECTION("irfft2 round-trip");

    {
        Array *a = make_f64_2d((double[]){1, 2, 3, 4,
                                          5, 6, 7, 8}, 2, 4);
        Array *rf = rfft2(a);
        ASSERT_NOTNULL(rf, "rfft2 2x4 not null");

        int s[2] = {2, 4};
        Array *b = irfft2(rf, s);
        ASSERT_NOTNULL(b, "irfft2 result not null");
        ASSERT_DTYPE(b, FLOAT64, "irfft2 output is FLOAT64");
        ASSERT_EQ_ARR((double*)b->data,
                      ((double[]){1, 2, 3, 4, 5, 6, 7, 8}), 8, TOL_FFT,
                      "irfft2(rfft2) round-trip");

        free_a(b); free_a(rf); free_a(a);
    }

    /* ── rfftn: N-D real FFT ────────────────────────────────────────── */
    TEST_SECTION("rfftn");

    {
        Array *a = make_f64_2d((double[]){1, 2, 3, 4,
                                          5, 6, 7, 8}, 2, 4);
        Array *r = rfftn(a);
        ASSERT_NOTNULL(r, "rfftn 2x4 result not null");
        ASSERT_DTYPE(r, COMPLEX128, "rfftn output is COMPLEX128");
        ASSERT_NDIM(r, 2, "rfftn output is 2D");
        /* last dim n=4 -> n//2+1=3 */
        ASSERT_EQ_INT(r->shape[1], 3, "rfftn 2x4 cols=3");
        free_a(r); free_a(a);
    }

    /* ── irfftn: round-trip ─────────────────────────────────────────── */
    TEST_SECTION("irfftn round-trip");

    {
        Array *a = make_f64_2d((double[]){1, 2, 3, 4,
                                          5, 6, 7, 8}, 2, 4);
        Array *rf = rfftn(a);
        ASSERT_NOTNULL(rf, "rfftn for irfftn not null");

        int s[2] = {2, 4};
        Array *b = irfftn(rf, s);
        ASSERT_NOTNULL(b, "irfftn result not null");
        ASSERT_DTYPE(b, FLOAT64, "irfftn output is FLOAT64");
        ASSERT_EQ_ARR((double*)b->data,
                      ((double[]){1, 2, 3, 4, 5, 6, 7, 8}), 8, TOL_FFT,
                      "irfftn(rfftn) round-trip");

        free_a(b); free_a(rf); free_a(a);
    }

    {
        /* Square 2D round-trip via rfftn/irfftn */
        Array *a = make_f64_2d((double[]){10, 20, 30, 40}, 2, 2);
        Array *rf = rfftn(a);
        ASSERT_NOTNULL(rf, "rfftn 2x2 not null");
        int s[2] = {2, 2};
        Array *b = irfftn(rf, s);
        ASSERT_NOTNULL(b, "irfftn 2x2 result not null");
        ASSERT_EQ_ARR((double*)b->data,
                      ((double[]){10, 20, 30, 40}), 4, TOL_FFT,
                      "irfftn(rfftn) 2x2 round-trip");
        free_a(b); free_a(rf); free_a(a);
    }

    TEST_SUMMARY();
    return (g_fail > 0) ? 1 : 0;
}
