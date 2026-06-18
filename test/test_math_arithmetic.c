/**
 * test_math_arithmetic.c — Tests for add, subtract, multiply, divide,
 * true_divide, add_scalar, multiply_scalar
 */

#include "test_utils.h"

int main(void) {
    TEST_MAIN("Math Arithmetic");

    /* ── add ──────────────────────────────────────────────────────── */
    TEST_SECTION("add");

    {
        Array *a = make_f64_1d((double[]){1, 2, 3}, 3);
        Array *b = make_f64_1d((double[]){4, 5, 6}, 3);
        Array *r = add(a, b);
        ASSERT_NOTNULL(r, "add 1D f64");
        ASSERT_EQ_ARR((double*)r->data, ((double[]){5, 7, 9}), 3, TOL_F64, "add 1D f64 values");
        free_a(r); free_a(b); free_a(a);
    }

    {
        Array *a = make_f64_2d((double[]){1, 2, 3, 4}, 2, 2);
        Array *b = make_f64_2d((double[]){5, 6, 7, 8}, 2, 2);
        Array *r = add(a, b);
        ASSERT_NOTNULL(r, "add 2D f64");
        ASSERT_EQ_ARR((double*)r->data, ((double[]){6, 8, 10, 12}), 4, TOL_F64, "add 2D f64 values");
        free_a(r); free_a(b); free_a(a);
    }

    /* i32 variant - add preserves INT32 dtype */
    {
        Array *a = make_i32_1d((int[]){1, 2, 3}, 3);
        Array *b = make_i32_1d((int[]){4, 5, 6}, 3);
        Array *r = add(a, b);
        ASSERT_NOTNULL(r, "add 1D i32");
        ASSERT_EQ_IARR((int32_t*)r->data, ((int32_t[]){5, 7, 9}), 3, "add 1D i32 values");
        free_a(r); free_a(b); free_a(a);
    }

    /* ── add_scalar ───────────────────────────────────────────────── */
    TEST_SECTION("add_scalar");

    {
        Array *a = make_f64_1d((double[]){1, 2, 3}, 3);
        double s = 10.0;
        Array *r = add_scalar(a, &s);
        ASSERT_NOTNULL(r, "add_scalar f64");
        ASSERT_EQ_ARR((double*)r->data, ((double[]){11, 12, 13}), 3, TOL_F64, "add_scalar f64 values");
        free_a(r); free_a(a);
    }

    /* ── subtract ─────────────────────────────────────────────────── */
    TEST_SECTION("subtract");

    {
        Array *a = make_f64_1d((double[]){10, 20, 30}, 3);
        Array *b = make_f64_1d((double[]){1, 2, 3}, 3);
        Array *r = subtract(a, b);
        ASSERT_NOTNULL(r, "subtract 1D f64");
        ASSERT_EQ_ARR((double*)r->data, ((double[]){9, 18, 27}), 3, TOL_F64, "subtract 1D f64 values");
        free_a(r); free_a(b); free_a(a);
    }

    /* ── multiply ─────────────────────────────────────────────────── */
    TEST_SECTION("multiply");

    {
        Array *a = make_f64_1d((double[]){2, 3, 4}, 3);
        Array *b = make_f64_1d((double[]){5, 6, 7}, 3);
        Array *r = multiply(a, b);
        ASSERT_NOTNULL(r, "multiply 1D f64");
        ASSERT_EQ_ARR((double*)r->data, ((double[]){10, 18, 28}), 3, TOL_F64, "multiply 1D f64 values");
        free_a(r); free_a(b); free_a(a);
    }

    /* ── multiply_scalar ──────────────────────────────────────────── */
    TEST_SECTION("multiply_scalar");

    {
        Array *a = make_f64_1d((double[]){1, 2, 3}, 3);
        double s = 3.0;
        Array *r = multiply_scalar(a, &s);
        ASSERT_NOTNULL(r, "multiply_scalar f64");
        ASSERT_EQ_ARR((double*)r->data, ((double[]){3, 6, 9}), 3, TOL_F64, "multiply_scalar f64 values");
        free_a(r); free_a(a);
    }

    /* ── divide ───────────────────────────────────────────────────── */
    TEST_SECTION("divide");

    {
        Array *a = make_f64_1d((double[]){10.0, 20.0, 30.0}, 3);
        Array *b = make_f64_1d((double[]){2.0, 4.0, 5.0}, 3);
        Array *r = divide(a, b);
        ASSERT_NOTNULL(r, "divide 1D f64");
        ASSERT_EQ_ARR((double*)r->data, ((double[]){5, 5, 6}), 3, TOL_F64, "divide 1D f64 values");
        free_a(r); free_a(b); free_a(a);
    }

    /* i32 variant - divide preserves INT32 dtype */
    {
        Array *a = make_i32_1d((int[]){10, 20, 30}, 3);
        Array *b = make_i32_1d((int[]){2, 4, 5}, 3);
        Array *r = divide(a, b);
        ASSERT_NOTNULL(r, "divide 1D i32");
        ASSERT_EQ_IARR((int32_t*)r->data, ((int32_t[]){5, 5, 6}), 3, "divide 1D i32 values");
        free_a(r); free_a(b); free_a(a);
    }

    /* ── true_divide ──────────────────────────────────────────────── */
    TEST_SECTION("true_divide");

    {
        Array *a = make_i32_1d((int[]){3, 4, 5}, 3);
        Array *b = make_i32_1d((int[]){2, 2, 2}, 3);
        Array *r = true_divide(a, b);
        ASSERT_NOTNULL(r, "true_divide i32 -> f64");
        ASSERT_EQ_ARR((double*)r->data, ((double[]){1.5, 2.0, 2.5}), 3, TOL_F64, "true_divide values");
        ASSERT_DTYPE(r, FLOAT64, "true_divide output dtype");
        free_a(r); free_a(b); free_a(a);
    }

    /* ── edge case: divide by zero ────────────────────────────────── */
    TEST_SECTION("divide by zero (no crash)");

    {
        Array *a = make_f64_1d((double[]){1.0, 2.0, 3.0}, 3);
        Array *b = make_f64_1d((double[]){0.0, 0.0, 0.0}, 3);
        Array *r = true_divide(a, b);
        /* true_divide uses floating division, returning inf for x/0 */
        ASSERT_NOTNULL(r, "true_divide by zero returns array");
        free_a(r); free_a(b); free_a(a);
    }

    TEST_SUMMARY();
    return (g_fail > 0) ? 1 : 0;
}
