/**
 * test_fft_basic.c — Tests for fft, ifft
 */

#include "test_utils.h"

int main(void) {
    TEST_MAIN("FFT Basic");

    /* ── fft: DC signal [1,1,1,1] ──────────────────────────────────── */
    TEST_SECTION("fft DC");

    {
        /* All energy at DC: fft([1,1,1,1]) = [4,0,0,0] */
        Array *a = make_f64_1d((double[]){1, 1, 1, 1}, 4);
        Array *r = fft(a);
        ASSERT_NOTNULL(r, "fft DC result not null");
        ASSERT_DTYPE(r, COMPLEX128, "fft output dtype COMPLEX128");
        ASSERT_SIZE(r, 4, "fft output size matches input");
        /* Result: [4+0j, 0+0j, 0+0j, 0+0j] */
        complex double *rd = (complex double*)r->data;
        ASSERT_EQ_DBL(creal(rd[0]), 4.0, TOL_FFT, "fft DC k=0 = 4");
        ASSERT_EQ_DBL(cimag(rd[0]), 0.0, TOL_FFT, "fft DC k=0 imag = 0");
        ASSERT_EQ_DBL(creal(rd[1]), 0.0, TOL_FFT, "fft DC k=1 = 0");
        ASSERT_EQ_DBL(creal(rd[2]), 0.0, TOL_FFT, "fft DC k=2 = 0");
        ASSERT_EQ_DBL(creal(rd[3]), 0.0, TOL_FFT, "fft DC k=3 = 0");
        free_a(r); free_a(a);
    }

    /* ── fft: impulse ───────────────────────────────────────────────── */
    TEST_SECTION("fft impulse");

    {
        /* fft([1,0,0,0]) = [1,1,1,1] (impulse -> all frequencies) */
        Array *a = make_f64_1d((double[]){1, 0, 0, 0}, 4);
        Array *r = fft(a);
        ASSERT_NOTNULL(r, "fft impulse result not null");
        complex double *rd = (complex double*)r->data;
        ASSERT_EQ_DBL(creal(rd[0]), 1.0, TOL_FFT, "fft impulse k=0 = 1");
        ASSERT_EQ_DBL(creal(rd[1]), 1.0, TOL_FFT, "fft impulse k=1 = 1");
        ASSERT_EQ_DBL(creal(rd[2]), 1.0, TOL_FFT, "fft impulse k=2 = 1");
        ASSERT_EQ_DBL(creal(rd[3]), 1.0, TOL_FFT, "fft impulse k=3 = 1");
        free_a(r); free_a(a);
    }

    /* ── fft + ifft round-trip ──────────────────────────────────────── */
    TEST_SECTION("fft ifft round-trip");

    {
        Array *a = make_f64_1d((double[]){1, 2, 3, 4}, 4);
        Array *f = fft(a);
        ASSERT_NOTNULL(f, "fft result not null");
        Array *b = ifft(f);
        ASSERT_NOTNULL(b, "ifft result not null");

        /* ifft returns COMPLEX128, check real parts equal original */
        complex double *bd = (complex double*)b->data;
        double original[] = {1, 2, 3, 4};
        for (int i = 0; i < 4; i++) {
            ASSERT_TRUE(fabs(creal(bd[i]) - original[i]) < TOL_FFT
                        && fabs(cimag(bd[i])) < TOL_FFT,
                        "ifft round-trip recovers original");
        }

        free_a(b); free_a(f); free_a(a);
    }

    /* ── fft linearity: fft(a+b) = fft(a) + fft(b) ──────────────────── */
    TEST_SECTION("fft linearity");

    {
        Array *a = make_f64_1d((double[]){1, 2, 3, 4}, 4);
        Array *b = make_f64_1d((double[]){5, 6, 7, 8}, 4);
        Array *sum_ab = add(a, b);
        Array *fft_sum = fft(sum_ab);

        Array *fft_a = fft(a);
        Array *fft_b = fft(b);
        Array *fft_added = add(fft_a, fft_b);

        ASSERT_NOTNULL(fft_sum, "fft(a+b) not null");
        ASSERT_NOTNULL(fft_added, "fft(a)+fft(b) not null");

        complex double *sd = (complex double*)fft_sum->data;
        complex double *ad = (complex double*)fft_added->data;
        for (int i = 0; i < 4; i++) {
            ASSERT_TRUE(cabs(sd[i] - ad[i]) < TOL_FFT,
                        "fft linearity holds");
        }

        free_a(fft_added); free_a(fft_b); free_a(fft_a);
        free_a(fft_sum); free_a(sum_ab); free_a(b); free_a(a);
    }

    /* ── fft different lengths ──────────────────────────────────────── */
    TEST_SECTION("fft lengths");

    {
        /* length 2 */
        Array *a = make_f64_1d((double[]){1, 2}, 2);
        Array *r = fft(a);
        ASSERT_NOTNULL(r, "fft n=2 result not null");
        ASSERT_SIZE(r, 2, "fft n=2 output size 2");
        free_a(r); free_a(a);
    }

    {
        /* length 8 */
        Array *a = make_f64_1d((double[]){1, 2, 3, 4, 5, 6, 7, 8}, 8);
        Array *f = fft(a);
        ASSERT_NOTNULL(f, "fft n=8 result not null");
        Array *b = ifft(f);
        ASSERT_NOTNULL(b, "ifft n=8 result not null");

        complex double *bd = (complex double*)b->data;
        for (int i = 0; i < 8; i++) {
            ASSERT_TRUE(fabs(creal(bd[i]) - (i + 1)) < TOL_FFT
                        && fabs(cimag(bd[i])) < TOL_FFT,
                        "n=8 round-trip");
        }
        free_a(b); free_a(f); free_a(a);
    }

    /* ifft of 2D input not supported (library returns NULL) */

    /* ── fft of INT32 input (promoted) ──────────────────────────────── */
    {
        Array *a = make_i32_1d((int[]){1, 1, 1, 1}, 4);
        Array *r = fft(a);
        ASSERT_NOTNULL(r, "fft of i32 result not null");
        ASSERT_DTYPE(r, COMPLEX128, "fft of i32 is COMPLEX128");
        complex double *rd = (complex double*)r->data;
        ASSERT_EQ_DBL(creal(rd[0]), 4.0, TOL_FFT, "fft i32 DC k=0 = 4");
        free_a(r); free_a(a);
    }

    TEST_SUMMARY();
    return (g_fail > 0) ? 1 : 0;
}
