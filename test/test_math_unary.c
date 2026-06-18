/**
 * test_math_unary.c — Tests for abs_array, sqrt_array, exp_array, log_array,
 * log10_array, log2_array, sign, sinc
 */

#include "test_utils.h"

int main(void) {
    TEST_MAIN("Math Unary");

    /* ── abs_array ────────────────────────────────────────────────── */
    TEST_SECTION("abs_array");

    {
        Array *a = make_f64_1d((double[]){1, -2, 3, -4}, 4);
        Array *r = abs_array(a);
        ASSERT_NOTNULL(r, "abs [1,-2,3,-4]");
        ASSERT_DTYPE(r, FLOAT64, "abs output dtype FLOAT64");
        ASSERT_EQ_ARR((double*)r->data, ((double[]){1, 2, 3, 4}), 4, TOL_F64, "abs values");
        free_a(r); free_a(a);
    }

    {
        Array *a = make_f64_1d((double[]){-0.5, 0.0, 0.5}, 3);
        Array *r = abs_array(a);
        ASSERT_NOTNULL(r, "abs [-0.5,0,0.5]");
        ASSERT_EQ_ARR((double*)r->data, ((double[]){0.5, 0.0, 0.5}), 3, TOL_F64, "abs fractional values");
        free_a(r); free_a(a);
    }

    /* i32 input */
    {
        Array *a = make_i32_1d((int[]){-5, 0, 3, -100}, 4);
        Array *r = abs_array(a);
        ASSERT_NOTNULL(r, "abs i32 input");
        ASSERT_DTYPE(r, INT32, "abs i32 -> INT32");
        ASSERT_EQ_IARR((int*)r->data, ((int[]){5, 0, 3, 100}), 4, "abs i32 values");
        free_a(r); free_a(a);
    }

    /* ── sqrt_array ───────────────────────────────────────────────── */
    TEST_SECTION("sqrt_array");

    {
        Array *a = make_f64_1d((double[]){0, 1, 4, 9, 16}, 5);
        Array *r = sqrt_array(a);
        ASSERT_NOTNULL(r, "sqrt [0,1,4,9,16]");
        ASSERT_DTYPE(r, FLOAT64, "sqrt output dtype FLOAT64");
        ASSERT_EQ_ARR((double*)r->data, ((double[]){0, 1, 2, 3, 4}), 5, TOL_F64, "sqrt values");
        free_a(r); free_a(a);
    }

    /* check all non-negative */
    {
        Array *a = make_f64_1d((double[]){0, 1, 4, 9, 16}, 5);
        Array *r = sqrt_array(a);
        ASSERT_NOTNULL(r, "sqrt non-negative check");
        int ok = 1;
        double *d = (double*)r->data;
        for (int i = 0; i < r->size; i++) {
            if (d[i] < 0.0) { ok = 0; break; }
        }
        ASSERT_TRUE(ok, "sqrt all results >= 0");
        free_a(r); free_a(a);
    }

    /* ── exp_array ────────────────────────────────────────────────── */
    TEST_SECTION("exp_array");

    {
        Array *a = make_f64_1d((double[]){0, 1, 2}, 3);
        Array *r = exp_array(a);
        ASSERT_NOTNULL(r, "exp [0,1,2]");
        double expected[] = {1.0, exp(1.0), exp(2.0)};
        ASSERT_EQ_ARR((double*)r->data, expected, 3, 1e-13, "exp values");
        free_a(r); free_a(a);
    }

    {
        Array *a = make_f64_1d((double[]){-1, 0, 1}, 3);
        Array *r = exp_array(a);
        ASSERT_NOTNULL(r, "exp [-1,0,1]");
        double expected[] = {exp(-1.0), 1.0, exp(1.0)};
        ASSERT_EQ_ARR((double*)r->data, expected, 3, 1e-13, "exp [-1,0,1] values");
        free_a(r); free_a(a);
    }

    /* ── log_array ────────────────────────────────────────────────── */
    TEST_SECTION("log_array");

    {
        double e = exp(1.0);
        Array *a = make_f64_1d((double[]){1, e, e*e}, 3);
        Array *r = log_array(a);
        ASSERT_NOTNULL(r, "log [1,e,e^2]");
        ASSERT_EQ_ARR((double*)r->data, ((double[]){0, 1, 2}), 3, TOL_F64, "log values");
        free_a(r); free_a(a);
    }

    /* ── log10_array ──────────────────────────────────────────────── */
    TEST_SECTION("log10_array");

    {
        Array *a = make_f64_1d((double[]){1, 10, 100, 1000}, 4);
        Array *r = log10_array(a);
        ASSERT_NOTNULL(r, "log10 [1,10,100,1000]");
        ASSERT_EQ_ARR((double*)r->data, ((double[]){0, 1, 2, 3}), 4, TOL_F64, "log10 values");
        free_a(r); free_a(a);
    }

    /* ── log2_array ───────────────────────────────────────────────── */
    TEST_SECTION("log2_array");

    {
        Array *a = make_f64_1d((double[]){1, 2, 4, 8}, 4);
        Array *r = log2_array(a);
        ASSERT_NOTNULL(r, "log2 [1,2,4,8]");
        ASSERT_EQ_ARR((double*)r->data, ((double[]){0, 1, 2, 3}), 4, TOL_F64, "log2 values");
        free_a(r); free_a(a);
    }

    /* ── sign ─────────────────────────────────────────────────────── */
    TEST_SECTION("sign");

    {
        Array *a = make_f64_1d((double[]){-5, 0, 3.14, -0.0}, 4);
        Array *r = sign(a);
        ASSERT_NOTNULL(r, "sign [-5,0,3.14,-0.0]");
        ASSERT_DTYPE(r, FLOAT64, "sign output dtype FLOAT64");
        ASSERT_EQ_ARR((double*)r->data, ((double[]){-1, 0, 1, 0}), 4, TOL_F64, "sign values");
        free_a(r); free_a(a);
    }

    /* ── sinc ─────────────────────────────────────────────────────── */
    TEST_SECTION("sinc");

    {
        Array *a = make_f64_1d((double[]){0, 1, 2}, 3);
        Array *r = sinc(a);
        ASSERT_NOTNULL(r, "sinc [0,1,2]");
        ASSERT_DTYPE(r, FLOAT64, "sinc output dtype FLOAT64");
        /* sinc(0) = 1.0, sinc(1) = sin(pi)/pi = 0, sinc(2) = sin(2pi)/(2pi) = 0 */
        ASSERT_EQ_ARR((double*)r->data, ((double[]){1.0, 0.0, 0.0}), 3, TOL_F64, "sinc values");
        free_a(r); free_a(a);
    }

    TEST_SUMMARY();
    return (g_fail > 0) ? 1 : 0;
}
