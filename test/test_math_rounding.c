/**
 * test_math_rounding.c — Tests for floor_array, ceil_array, arint, atrunc,
 * around, fix
 */

#include "test_utils.h"

int main(void) {
    TEST_MAIN("Math Rounding");

    /* ── floor_array ──────────────────────────────────────────────── */
    TEST_SECTION("floor_array");

    {
        Array *a = make_f64_1d((double[]){1.1, 1.9, -1.1, -1.9}, 4);
        Array *r = floor_array(a);
        ASSERT_NOTNULL(r, "floor [1.1,1.9,-1.1,-1.9]");
        ASSERT_EQ_ARR((double*)r->data, ((double[]){1, 1, -2, -2}), 4, TOL_F64, "floor values");
        free_a(r); free_a(a);
    }

    /* i32 input */
    {
        Array *a = make_i32_1d((int[]){1, -2, 3, -4}, 4);
        Array *r = floor_array(a);
        ASSERT_NOTNULL(r, "floor i32 input");
        ASSERT_DTYPE(r, FLOAT64, "floor i32 -> FLOAT64");
        ASSERT_EQ_ARR((double*)r->data, ((double[]){1, -2, 3, -4}), 4, TOL_F64, "floor i32 values");
        free_a(r); free_a(a);
    }

    /* ── ceil_array ───────────────────────────────────────────────── */
    TEST_SECTION("ceil_array");

    {
        Array *a = make_f64_1d((double[]){1.1, 1.9, -1.1, -1.9}, 4);
        Array *r = ceil_array(a);
        ASSERT_NOTNULL(r, "ceil [1.1,1.9,-1.1,-1.9]");
        ASSERT_EQ_ARR((double*)r->data, ((double[]){2, 2, -1, -1}), 4, TOL_F64, "ceil values");
        free_a(r); free_a(a);
    }

    /* ── arint (banker's rounding) ────────────────────────────────── */
    TEST_SECTION("arint");

    {
        Array *a = make_f64_1d((double[]){1.1, 1.5, 2.5, -1.5, -2.5}, 5);
        Array *r = arint(a);
        ASSERT_NOTNULL(r, "arint [1.1,1.5,2.5,-1.5,-2.5]");
        double *d = (double*)r->data;
        /* Banker's rounding: all .5 values round to nearest even integer */
        /* 1.1 -> 1, 1.5 -> 2, 2.5 -> 2, -1.5 -> -2, -2.5 -> -2 */
        ASSERT_EQ_DBL(d[0], 1.0, TOL_F64, "arint 1.1");
        ASSERT_EQ_DBL(d[1], 2.0, TOL_F64, "arint 1.5 -> 2");
        ASSERT_EQ_DBL(d[2], 2.0, TOL_F64, "arint 2.5 -> 2");
        ASSERT_EQ_DBL(d[3], -2.0, TOL_F64, "arint -1.5 -> -2");
        ASSERT_EQ_DBL(d[4], -2.0, TOL_F64, "arint -2.5 -> -2");
        free_a(r); free_a(a);
    }

    /* ── atrunc ───────────────────────────────────────────────────── */
    TEST_SECTION("atrunc");

    {
        Array *a = make_f64_1d((double[]){1.1, 1.9, -1.1, -1.9}, 4);
        Array *r = atrunc(a);
        ASSERT_NOTNULL(r, "atrunc [1.1,1.9,-1.1,-1.9]");
        ASSERT_EQ_ARR((double*)r->data, ((double[]){1, 1, -1, -1}), 4, TOL_F64, "atrunc values");
        free_a(r); free_a(a);
    }

    /* ── around ───────────────────────────────────────────────────── */
    TEST_SECTION("around");

    {
        Array *a = make_f64_1d((double[]){1.234, 5.678, 3.14159}, 3);
        Array *r = around(a, 2);
        ASSERT_NOTNULL(r, "around decimals=2");
        ASSERT_EQ_ARR((double*)r->data, ((double[]){1.23, 5.68, 3.14}), 3, TOL_F64, "around dec=2 values");
        free_a(r); free_a(a);
    }

    {
        Array *a = make_f64_1d((double[]){1.5, 2.5, 3.5}, 3);
        Array *r = around(a, 0);
        ASSERT_NOTNULL(r, "around decimals=0");
        /* round half to even: 1.5 -> 2, 2.5 -> 2, 3.5 -> 4 */
        ASSERT_EQ_ARR((double*)r->data, ((double[]){2, 2, 4}), 3, TOL_F64, "around dec=0 values");
        free_a(r); free_a(a);
    }

    /* ── fix ──────────────────────────────────────────────────────── */
    TEST_SECTION("fix");

    {
        Array *a = make_f64_1d((double[]){1.1, 1.9, -1.1, -1.9}, 4);
        Array *r = fix(a);
        ASSERT_NOTNULL(r, "fix [1.1,1.9,-1.1,-1.9]");
        /* fix truncates toward zero, same as atrunc */
        ASSERT_EQ_ARR((double*)r->data, ((double[]){1, 1, -1, -1}), 4, TOL_F64, "fix values");
        free_a(r); free_a(a);
    }

    /* ── dtype check: i32 -> FLOAT64 ──────────────────────────────── */
    {
        Array *a = make_i32_1d((int[]){3, -3, 7, -7}, 4);
        Array *r = ceil_array(a);
        ASSERT_NOTNULL(r, "ceil i32 -> FLOAT64");
        ASSERT_DTYPE(r, FLOAT64, "ceil i32 dtype");
        free_a(r); free_a(a);
    }

    {
        Array *a = make_i32_1d((int[]){3, -3, 7, -7}, 4);
        Array *r = atrunc(a);
        ASSERT_NOTNULL(r, "atrunc i32 -> FLOAT64");
        ASSERT_DTYPE(r, FLOAT64, "atrunc i32 dtype");
        free_a(r); free_a(a);
    }

    {
        Array *a = make_i32_1d((int[]){3, -3, 7, -7}, 4);
        Array *r = fix(a);
        ASSERT_NOTNULL(r, "fix i32 -> INT32");
        ASSERT_DTYPE(r, INT32, "fix i32 dtype");
        free_a(r); free_a(a);
    }

    TEST_SUMMARY();
    return (g_fail > 0) ? 1 : 0;
}
