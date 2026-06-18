/**
 * test_math_trig_hyper.c — Tests for sinh_array, cosh_array, tanh_array,
 * arcsinh, arccosh, arctanh
 */

#include "test_utils.h"

int main(void) {
    TEST_MAIN("Math Hyperbolic Trig");

    /* ── sinh_array ───────────────────────────────────────────────── */
    TEST_SECTION("sinh_array");

    {
        double ln2 = log(2.0);
        Array *a = make_f64_1d((double[]){0, ln2}, 2);
        Array *r = sinh_array(a);
        ASSERT_NOTNULL(r, "sinh [0,ln2]");
        ASSERT_EQ_ARR((double*)r->data, ((double[]){0, 0.75}), 2, TOL_F64, "sinh values");
        free_a(r); free_a(a);
    }

    /* ── cosh_array ───────────────────────────────────────────────── */
    TEST_SECTION("cosh_array");

    {
        double ln2 = log(2.0);
        Array *a = make_f64_1d((double[]){0, ln2}, 2);
        Array *r = cosh_array(a);
        ASSERT_NOTNULL(r, "cosh [0,ln2]");
        ASSERT_EQ_ARR((double*)r->data, ((double[]){1, 1.25}), 2, TOL_F64, "cosh values");
        free_a(r); free_a(a);
    }

    /* ── tanh_array ───────────────────────────────────────────────── */
    TEST_SECTION("tanh_array");

    {
        Array *a = make_f64_1d((double[]){0, 100}, 2);
        Array *r = tanh_array(a);
        ASSERT_NOTNULL(r, "tanh [0,100]");
        /* tanh(0) = 0, tanh(100) -> 1 */
        ASSERT_EQ_ARR((double*)r->data, ((double[]){0, 1}), 2, TOL_F64, "tanh values");
        free_a(r); free_a(a);
    }

    /* ── arcsinh ──────────────────────────────────────────────────── */
    TEST_SECTION("arcsinh");

    {
        Array *a = make_f64_1d((double[]){0, 0.75}, 2);
        Array *r = arcsinh(a);
        ASSERT_NOTNULL(r, "arcsinh [0,0.75]");
        /* arcsinh(0.75) = ln(0.75 + sqrt(0.75^2+1)) ≈ ln(2) */
        double ln2 = log(2.0);
        ASSERT_EQ_ARR((double*)r->data, ((double[]){0, ln2}), 2, TOL_F64, "arcsinh values");
        free_a(r); free_a(a);
    }

    /* ── arccosh ──────────────────────────────────────────────────── */
    TEST_SECTION("arccosh");

    {
        Array *a = make_f64_1d((double[]){1, 1.25}, 2);
        Array *r = arccosh(a);
        ASSERT_NOTNULL(r, "arccosh [1,1.25]");
        /* arccosh(1.25) = ln(1.25 + sqrt(1.25^2-1)) ≈ ln(2) */
        double ln2 = log(2.0);
        ASSERT_EQ_ARR((double*)r->data, ((double[]){0, ln2}), 2, TOL_F64, "arccosh values");
        free_a(r); free_a(a);
    }

    /* ── arctanh ──────────────────────────────────────────────────── */
    TEST_SECTION("arctanh");

    {
        Array *a = make_f64_1d((double[]){0, 0.5}, 2);
        Array *r = arctanh(a);
        ASSERT_NOTNULL(r, "arctanh [0,0.5]");
        /* arctanh(0.5) = 0.5 * ln((1+0.5)/(1-0.5)) = 0.5 * ln(3) */
        double expected = 0.5 * log(3.0);
        ASSERT_EQ_ARR((double*)r->data, ((double[]){0, expected}), 2, TOL_F64, "arctanh values");
        free_a(r); free_a(a);
    }

    TEST_SUMMARY();
    return (g_fail > 0) ? 1 : 0;
}
