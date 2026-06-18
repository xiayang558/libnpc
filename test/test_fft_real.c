/**
 * test_fft_real.c — Tests for rfft, irfft, hfft, ihfft
 */

#include "test_utils.h"

int main(void) {
    TEST_MAIN("FFT Real");

    /* ── rfft: real input -> shorter output ─────────────────────────── */
    TEST_SECTION("rfft");

    {
        /* rfft length n -> output length n//2 + 1 */
        Array *a = make_f64_1d((double[]){1, 2, 3, 4}, 4);
        Array *r = rfft(a);
        ASSERT_NOTNULL(r, "rfft [1,2,3,4] result not null");
        ASSERT_DTYPE(r, COMPLEX128, "rfft output is COMPLEX128");
        /* n=4 -> output size = 4//2 + 1 = 3 */
        ASSERT_EQ_INT(r->size, 3, "rfft n=4 output size = 3");
        free_a(r); free_a(a);
    }

    {
        /* rfft with n=5 -> output length 5//2+1 = 3 */
        Array *a = make_f64_1d((double[]){1, 2, 3, 4, 5}, 5);
        Array *r = rfft(a);
        ASSERT_NOTNULL(r, "rfft n=5 result not null");
        ASSERT_EQ_INT(r->size, 3, "rfft n=5 output size = 3");
        free_a(r); free_a(a);
    }

    {
        /* rfft with n=8 -> output length 8//2+1 = 5 */
        Array *a = make_f64_1d((double[]){1, 2, 3, 4, 5, 6, 7, 8}, 8);
        Array *r = rfft(a);
        ASSERT_NOTNULL(r, "rfft n=8 result not null");
        ASSERT_EQ_INT(r->size, 5, "rfft n=8 output size = 5");
        free_a(r); free_a(a);
    }

    /* rfft output is shorter than fft output */
    {
        Array *a = make_f64_1d((double[]){1, 2, 3, 4, 5, 6}, 6);
        Array *rf = rfft(a);
        Array *ff = fft(a);
        ASSERT_NOTNULL(rf, "rfft n=6 result not null");
        ASSERT_NOTNULL(ff, "fft n=6 result not null");
        ASSERT_TRUE(rf->size < ff->size, "rfft output shorter than fft output");
        ASSERT_EQ_INT(rf->size, 4, "rfft n=6 size = 4 (n//2+1)");
        ASSERT_EQ_INT(ff->size, 6, "fft n=6 size = 6 (n)");
        free_a(ff); free_a(rf); free_a(a);
    }

    /* ── irfft: round-trip ──────────────────────────────────────────── */
    TEST_SECTION("irfft");

    {
        /* irfft(rfft(a), n) ≈ a */
        Array *a = make_f64_1d((double[]){1, 2, 3, 4}, 4);
        Array *rf = rfft(a);
        ASSERT_NOTNULL(rf, "rfft for round-trip not null");
        Array *b = irfft(rf, 4);
        ASSERT_NOTNULL(b, "irfft result not null");
        ASSERT_DTYPE(b, FLOAT64, "irfft output is FLOAT64");
        ASSERT_EQ_ARR((double*)b->data,
                      ((double[]){1, 2, 3, 4}), 4, TOL_FFT,
                      "irfft(rfft([1,2,3,4])) ≈ [1,2,3,4]");
        free_a(b); free_a(rf); free_a(a);
    }

    {
        /* round-trip with n=8 */
        Array *a = make_f64_1d((double[]){1, 3, 5, 7, 9, 11, 13, 15}, 8);
        Array *rf = rfft(a);
        ASSERT_NOTNULL(rf, "rfft n=8 not null");
        Array *b = irfft(rf, 8);
        ASSERT_NOTNULL(b, "irfft n=8 result not null");
        ASSERT_EQ_ARR((double*)b->data,
                      ((double[]){1, 3, 5, 7, 9, 11, 13, 15}), 8, TOL_FFT,
                      "irfft(rfft) n=8 round-trip");
        free_a(b); free_a(rf); free_a(a);
    }

    {
        /* round-trip with odd n */
        Array *a = make_f64_1d((double[]){1, 2, 3, 4, 5}, 5);
        Array *rf = rfft(a);
        ASSERT_NOTNULL(rf, "rfft n=5 not null");
        Array *b = irfft(rf, 5);
        ASSERT_NOTNULL(b, "irfft n=5 result not null");
        ASSERT_EQ_ARR((double*)b->data,
                      ((double[]){1, 2, 3, 4, 5}), 5, TOL_FFT,
                      "irfft(rfft) n=5 round-trip");
        free_a(b); free_a(rf); free_a(a);
    }

    /* ── hfft: half-spectrum to real signal ─────────────────────────── */
    TEST_SECTION("hfft");

    {
        /* hfft from rfft should reconstruct a real signal of length n */
        Array *a = make_f64_1d((double[]){1, 2, 3, 4, 5, 6}, 6);
        Array *rf = rfft(a);
        ASSERT_NOTNULL(rf, "rfft for hfft test not null");
        Array *b = hfft(rf, 6);
        ASSERT_NOTNULL(b, "hfft result not null");
        ASSERT_DTYPE(b, FLOAT64, "hfft output is FLOAT64");
        ASSERT_SIZE(b, 6, "hfft output size=6");
        free_a(b); free_a(rf); free_a(a);
    }

    {
        /* hfft with odd n=5 */
        Array *a = make_f64_1d((double[]){10, 20, 30, 40, 50}, 5);
        Array *rf = rfft(a);
        ASSERT_NOTNULL(rf, "rfft for hfft n=5 not null");
        Array *b = hfft(rf, 5);
        ASSERT_NOTNULL(b, "hfft n=5 result not null");
        ASSERT_SIZE(b, 5, "hfft n=5 output size=5");
        free_a(b); free_a(rf); free_a(a);
    }

    /* ── ihfft: real signal to half-spectrum ────────────────────────── */
    TEST_SECTION("ihfft");

    {
        Array *a = make_f64_1d((double[]){1, 2, 3, 4}, 4);
        Array *h = ihfft(a, 4);
        ASSERT_NOTNULL(h, "ihfft result not null");
        ASSERT_DTYPE(h, COMPLEX128, "ihfft output is COMPLEX128");
        /* ihfft n=4 -> output length n//2+1 = 3 */
        ASSERT_EQ_INT(h->size, 3, "ihfft n=4 output len = 3");
        free_a(h); free_a(a);
    }

    {
        /* ihfft + hfft round-trip */
        Array *a = make_f64_1d((double[]){1, 2, 3, 4, 5, 6, 7, 8}, 8);
        Array *h = ihfft(a, 8);
        ASSERT_NOTNULL(h, "ihfft not null");
        Array *b = hfft(h, 8);
        ASSERT_NOTNULL(b, "hfft after ihfft not null");
        ASSERT_EQ_ARR((double*)b->data,
                      ((double[]){1, 2, 3, 4, 5, 6, 7, 8}), 8, TOL_FFT,
                      "hfft(ihfft(x)) round-trip");
        free_a(b); free_a(h); free_a(a);
    }

    /* ── rfft with INT32 input ──────────────────────────────────────── */
    {
        Array *a = make_i32_1d((int[]){1, 2, 3, 4}, 4);
        Array *r = rfft(a);
        ASSERT_NOTNULL(r, "rfft i32 result not null");
        ASSERT_DTYPE(r, COMPLEX128, "rfft i32 output is COMPLEX128");
        ASSERT_EQ_INT(r->size, 3, "rfft i32 n=4 size = 3");
        free_a(r); free_a(a);
    }

    TEST_SUMMARY();
    return (g_fail > 0) ? 1 : 0;
}
