/**
 * test_math_trig.c — Tests for sin_array, cos_array, tan_array,
 * arcsin, arccos, arctan
 */

#include "test_utils.h"

int main(void) {
    TEST_MAIN("Math Trigonometry");

    /* ── sin_array ────────────────────────────────────────────────── */
    TEST_SECTION("sin_array");

    {
        Array *a = make_f64_1d((double[]){0, M_PI / 2, M_PI, 3 * M_PI / 2}, 4);
        Array *r = sin_array(a);
        ASSERT_NOTNULL(r, "sin [0,pi/2,pi,3pi/2]");
        ASSERT_EQ_ARR((double*)r->data, ((double[]){0, 1, 0, -1}), 4, TOL_F64, "sin values");
        free_a(r); free_a(a);
    }

    /* ── cos_array ────────────────────────────────────────────────── */
    TEST_SECTION("cos_array");

    {
        Array *a = make_f64_1d((double[]){0, M_PI / 2, M_PI}, 3);
        Array *r = cos_array(a);
        ASSERT_NOTNULL(r, "cos [0,pi/2,pi]");
        ASSERT_EQ_ARR((double*)r->data, ((double[]){1, 0, -1}), 3, TOL_F64, "cos values");
        free_a(r); free_a(a);
    }

    /* ── tan_array ────────────────────────────────────────────────── */
    TEST_SECTION("tan_array");

    {
        Array *a = make_f64_1d((double[]){0, M_PI / 4}, 2);
        Array *r = tan_array(a);
        ASSERT_NOTNULL(r, "tan [0,pi/4]");
        ASSERT_EQ_ARR((double*)r->data, ((double[]){0, 1}), 2, TOL_F64, "tan values");
        free_a(r); free_a(a);
    }

    /* ── arcsin ───────────────────────────────────────────────────── */
    TEST_SECTION("arcsin");

    {
        Array *a = make_f64_1d((double[]){0, 0.5, 1}, 3);
        Array *r = arcsin(a);
        ASSERT_NOTNULL(r, "arcsin [0,0.5,1]");
        ASSERT_EQ_ARR((double*)r->data, ((double[]){0, M_PI/6, M_PI/2}), 3, TOL_F64, "arcsin values");
        free_a(r); free_a(a);
    }

    /* ── arccos ───────────────────────────────────────────────────── */
    TEST_SECTION("arccos");

    {
        Array *a = make_f64_1d((double[]){1, 0.5, 0}, 3);
        Array *r = arccos(a);
        ASSERT_NOTNULL(r, "arccos [1,0.5,0]");
        ASSERT_EQ_ARR((double*)r->data, ((double[]){0, M_PI/3, M_PI/2}), 3, TOL_F64, "arccos values");
        free_a(r); free_a(a);
    }

    /* ── arctan ───────────────────────────────────────────────────── */
    TEST_SECTION("arctan");

    {
        Array *a = make_f64_1d((double[]){0, 1}, 2);
        Array *r = arctan(a);
        ASSERT_NOTNULL(r, "arctan [0,1]");
        ASSERT_EQ_ARR((double*)r->data, ((double[]){0, M_PI/4}), 2, TOL_F64, "arctan values");
        free_a(r); free_a(a);
    }

    {
        /* arctan of a large number approaches pi/2 */
        Array *a = make_f64_1d((double[]){1e10}, 1);
        Array *r = arctan(a);
        ASSERT_NOTNULL(r, "arctan large -> ~pi/2");
        double got = ((double*)r->data)[0];
        ASSERT_TRUE(fabs(got - M_PI/2) < 1.5e-10, "arctan(1e10) ~= pi/2");
        free_a(r); free_a(a);
    }

    TEST_SUMMARY();
    return (g_fail > 0) ? 1 : 0;
}
