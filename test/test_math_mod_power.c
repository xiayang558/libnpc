/**
 * test_math_mod_power.c — Tests for mod, mod_scalar, power
 */

#include "test_utils.h"

int main(void) {
    TEST_MAIN("Math Mod & Power");

    /* ── mod ──────────────────────────────────────────────────────── */
    TEST_SECTION("mod");

    {
        Array *a = make_f64_1d((double[]){10, 11, 12}, 3);
        Array *b = make_f64_1d((double[]){3, 3, 3}, 3);
        Array *r = mod(a, b);
        ASSERT_NOTNULL(r, "mod 1D f64");
        ASSERT_EQ_ARR((double*)r->data, ((double[]){1, 2, 0}), 3, TOL_F64, "mod values [10,11,12]%%3");
        free_a(r); free_a(b); free_a(a);
    }

    {
        Array *a = make_f64_1d((double[]){5.5, 6.5}, 2);
        Array *b = make_f64_1d((double[]){2.0, 2.0}, 2);
        Array *r = mod(a, b);
        ASSERT_NOTNULL(r, "mod with float values");
        ASSERT_EQ_ARR((double*)r->data, ((double[]){1.5, 0.5}), 2, TOL_F64, "mod [5.5,6.5]%%2");
        free_a(r); free_a(b); free_a(a);
    }

    /* i32 input */
    {
        Array *a = make_i32_1d((int[]){10, 11, 12}, 3);
        Array *b = make_i32_1d((int[]){3, 3, 3}, 3);
        Array *r = mod(a, b);
        ASSERT_NOTNULL(r, "mod i32 input");
        ASSERT_DTYPE(r, FLOAT64, "mod i32 -> FLOAT64");
        ASSERT_EQ_ARR((double*)r->data, ((double[]){1, 2, 0}), 3, TOL_F64, "mod i32 values");
        free_a(r); free_a(b); free_a(a);
    }

    /* ── mod_scalar ───────────────────────────────────────────────── */
    TEST_SECTION("mod_scalar");

    {
        Array *a = make_f64_1d((double[]){10, 11, 12}, 3);
        double s = 3.0;
        Array *r = mod_scalar(a, &s);
        ASSERT_NOTNULL(r, "mod_scalar f64");
        ASSERT_EQ_ARR((double*)r->data, ((double[]){1, 2, 0}), 3, TOL_F64, "mod_scalar [10,11,12]%%3");
        free_a(r); free_a(a);
    }

    {
        Array *a = make_i32_1d((int[]){10, 11, 12}, 3);
        int s = 3;
        Array *r = mod_scalar(a, &s);
        ASSERT_NOTNULL(r, "mod_scalar i32 input");
        ASSERT_EQ_ARR((double*)r->data, ((double[]){1, 2, 0}), 3, TOL_F64, "mod_scalar i32 values");
        free_a(r); free_a(a);
    }

    /* ── power ────────────────────────────────────────────────────── */
    TEST_SECTION("power");

    {
        Array *a = make_f64_1d((double[]){2, 3, 4}, 3);
        Array *b = make_f64_1d((double[]){2, 2, 2}, 3);
        Array *r = power(a, b);
        ASSERT_NOTNULL(r, "power [2,3,4]^2");
        ASSERT_DTYPE(r, FLOAT64, "power output dtype FLOAT64");
        ASSERT_EQ_ARR((double*)r->data, ((double[]){4, 9, 16}), 3, TOL_F64, "power values");
        free_a(r); free_a(b); free_a(a);
    }

    {
        Array *a = make_f64_1d((double[]){1, 2, 3}, 3);
        Array *b = make_f64_1d((double[]){0, 0, 0}, 3);
        Array *r = power(a, b);
        ASSERT_NOTNULL(r, "power ^0");
        ASSERT_EQ_ARR((double*)r->data, ((double[]){1, 1, 1}), 3, TOL_F64, "power ^0 = 1");
        free_a(r); free_a(b); free_a(a);
    }

    {
        Array *a = make_f64_1d((double[]){2, 2, 2}, 3);
        Array *b = make_f64_1d((double[]){1, 2, 3}, 3);
        Array *r = power(a, b);
        ASSERT_NOTNULL(r, "power 2^[1,2,3]");
        ASSERT_EQ_ARR((double*)r->data, ((double[]){2, 4, 8}), 3, TOL_F64, "power values 2^n");
        free_a(r); free_a(b); free_a(a);
    }

    /* i32 input promoted to FLOAT64 */
    {
        Array *a = make_i32_1d((int[]){2, 3, 4}, 3);
        Array *b = make_i32_1d((int[]){2, 2, 2}, 3);
        Array *r = power(a, b);
        ASSERT_NOTNULL(r, "power i32 input");
        ASSERT_DTYPE(r, FLOAT64, "power i32 -> FLOAT64 dtype");
        ASSERT_EQ_ARR((double*)r->data, ((double[]){4, 9, 16}), 3, TOL_F64, "power i32 values");
        free_a(r); free_a(b); free_a(a);
    }

    /* edge case: negative base with integer exponents */
    {
        Array *a = make_f64_1d((double[]){-2, -3, -1}, 3);
        Array *b = make_f64_1d((double[]){2, 3, 4}, 3);
        Array *r = power(a, b);
        ASSERT_NOTNULL(r, "power negative base int exp");
        ASSERT_EQ_ARR((double*)r->data, ((double[]){4, -27, 1}), 3, TOL_F64, "power neg base values");
        free_a(r); free_a(b); free_a(a);
    }

    {
        Array *a = make_f64_1d((double[]){-2.0}, 1);
        Array *b = make_f64_1d((double[]){0.5}, 1);
        Array *r = power(a, b);
        ASSERT_NOTNULL(r, "power negative base fractional exp");
        /* sqrt(-2) is NaN — just verify no crash and output is not NULL */
        free_a(r); free_a(b); free_a(a);
    }

    TEST_SUMMARY();
    return (g_fail > 0) ? 1 : 0;
}
