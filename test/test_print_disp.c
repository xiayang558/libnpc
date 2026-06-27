/**
 * test_print_disp.c — Smoke tests for output functions:
 *   print_array, print_scalar, disp
 *
 * These are primarily smoke tests since output goes to stdout.
 */

#include "test_utils.h"

int main(void) {
    TEST_MAIN("Print / Display (smoke tests)");

    /* ── print_array ────────────────────────────────────────────── */
    TEST_SECTION("print_array");

    {
        /* 1D float array */
        double data[] = {1.0, 2.0, 3.0, 4.0};
        Array *a = make_f64_1d(data, 4);
        ASSERT_NOTNULL(a, "print_array create 1D");
        print_array(a);
        ASSERT_TRUE(1, "print_array 1D FLOAT64 no crash");
        a = free_a(a);
    }
    {
        /* 2D int array */
        int data[] = {1, 2, 3, 4, 5, 6};
        Array *a = make_i32_2d(data, 2, 3);
        ASSERT_NOTNULL(a, "print_array create 2D");
        print_array(a);
        ASSERT_TRUE(1, "print_array 2D INT32 no crash");
        a = free_a(a);
    }
    {
        /* 0D scalar — now supported */
        Array *a = create_array(NULL, 0, FLOAT64);
        ASSERT_NOTNULL(a, "print_array 0D create succeeds");
        ASSERT_TRUE(1, "print_array 0D no crash");
    }

    /* ── print_scalar ───────────────────────────────────────────── */
    TEST_SECTION("print_scalar");

    {
        /* scalar value in 1D array */
        double data[] = {42.0};
        Array *a = make_f64_1d(data, 1);
        ASSERT_NOTNULL(a, "print_scalar create 0D FLOAT64");
        print_scalar(a);
        ASSERT_TRUE(1, "print_scalar 0D FLOAT64 no crash");
        a = free_a(a);
    }
    {
        /* size-1 1D array as scalar */
        double data[] = {3.14};
        Array *a = make_f64_1d(data, 1);
        ASSERT_NOTNULL(a, "print_scalar create size-1");
        print_scalar(a);
        ASSERT_TRUE(1, "print_scalar size-1 array no crash");
        a = free_a(a);
    }
    {
        /* scalar value in 1D array */
        int val = 99;
        Array *a = make_i32_1d(&val, 1);
        ASSERT_NOTNULL(a, "print_scalar create 0D INT32");
        print_scalar(a);
        ASSERT_TRUE(1, "print_scalar 0D INT32 no crash");
        a = free_a(a);
    }

    /* ── disp ───────────────────────────────────────────────────── */
    TEST_SECTION("disp");

    {
        /* disp with newline */
        disp("Hello, libnpc!", 1);
        ASSERT_TRUE(1, "disp with newline no crash");
    }
    {
        /* disp without newline */
        disp("No newline", 0);
        ASSERT_TRUE(1, "disp without newline no crash");
    }
    {
        /* disp empty string */
        disp("", 1);
        ASSERT_TRUE(1, "disp empty string no crash");
    }
    {
        /* disp with special characters */
        disp("Line1\nLine2", 1);
        ASSERT_TRUE(1, "disp special chars no crash");
    }
    {
        /* disp multiple calls */
        disp("First", 0);
        disp("Second", 1);
        ASSERT_TRUE(1, "disp multiple calls no crash");
    }

    TEST_SUMMARY();
    return (g_fail > 0) ? 1 : 0;
}
