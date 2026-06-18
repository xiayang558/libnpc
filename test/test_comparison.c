/**
 * test_comparison.c — Tests for comparison functions:
 *   equal, greater, greater_equal, less, less_equal, isclose, where
 */

#include "test_utils.h"

int main(void) {
    TEST_MAIN("Comparisons");

    /* ── equal ────────────────────────────────────────────────────── */
    TEST_SECTION("equal");

    {
        /* equal: [1,2,3] vs [1,0,3] -> [1,0,1] */
        double d1[] = {1, 2, 3};
        double d2[] = {1, 0, 3};
        Array *a1 = make_f64_1d(d1, 3);
        Array *a2 = make_f64_1d(d2, 3);
        Array *r = equal(a1, a2);
        ASSERT_NOTNULL(r, "equal arrays not null");
        ASSERT_DTYPE(r, BOOL, "equal output dtype BOOL");
        ASSERT_SIZE(r, 3, "equal size=3");
        uint8_t expected[] = {1, 0, 1};
        ASSERT_EQ_IARR((uint8_t *)r->data, expected, 3, "equal [1,2,3]==[1,0,3]");
        free_a(a1); free_a(a2); free_a(r);
    }
    {
        /* equal: broadcast scalar vs array */
        double d1[] = {5, 5, 5};
        double d2[] = {5, 3, 5};
        Array *a1 = make_f64_1d(d1, 3);
        Array *a2 = make_f64_1d(d2, 3);
        Array *r = equal(a1, a2);
        ASSERT_NOTNULL(r, "equal broadcast not null");
        uint8_t expected[] = {1, 0, 1};
        ASSERT_EQ_IARR((uint8_t *)r->data, expected, 3, "equal broadcast [5,5,5]==[5,3,5]");
        free_a(a1); free_a(a2); free_a(r);
    }
    {
        /* equal: INT32 arrays */
        int d1[] = {10, 20, 30};
        int d2[] = {10, 99, 30};
        Array *a1 = make_i32_1d(d1, 3);
        Array *a2 = make_i32_1d(d2, 3);
        Array *r = equal(a1, a2);
        ASSERT_NOTNULL(r, "equal INT32 not null");
        uint8_t expected[] = {1, 0, 1};
        ASSERT_EQ_IARR((uint8_t *)r->data, expected, 3, "equal INT32 [10,20,30]==[10,99,30]");
        free_a(a1); free_a(a2); free_a(r);
    }
    {
        /* equal: all match */
        double d1[] = {1, 2, 3};
        double d2[] = {1, 2, 3};
        Array *a1 = make_f64_1d(d1, 3);
        Array *a2 = make_f64_1d(d2, 3);
        Array *r = equal(a1, a2);
        ASSERT_NOTNULL(r, "equal all match not null");
        uint8_t expected[] = {1, 1, 1};
        ASSERT_EQ_IARR((uint8_t *)r->data, expected, 3, "equal all match = [1,1,1]");
        free_a(a1); free_a(a2); free_a(r);
    }
    {
        /* equal: none match */
        double d1[] = {1, 2, 3};
        double d2[] = {4, 5, 6};
        Array *a1 = make_f64_1d(d1, 3);
        Array *a2 = make_f64_1d(d2, 3);
        Array *r = equal(a1, a2);
        ASSERT_NOTNULL(r, "equal none match not null");
        uint8_t expected[] = {0, 0, 0};
        ASSERT_EQ_IARR((uint8_t *)r->data, expected, 3, "equal none match = [0,0,0]");
        free_a(a1); free_a(a2); free_a(r);
    }

    /* ── greater ──────────────────────────────────────────────────── */
    TEST_SECTION("greater");

    {
        /* greater: [1,3,5] vs [2,2,2] -> [0,1,1] */
        double d1[] = {1, 3, 5};
        double d2[] = {2, 2, 2};
        Array *a1 = make_f64_1d(d1, 3);
        Array *a2 = make_f64_1d(d2, 3);
        Array *r = greater(a1, a2);
        ASSERT_NOTNULL(r, "greater not null");
        ASSERT_DTYPE(r, BOOL, "greater output dtype BOOL");
        uint8_t expected[] = {0, 1, 1};
        ASSERT_EQ_IARR((uint8_t *)r->data, expected, 3, "greater [1,3,5]>[2,2,2]");
        free_a(a1); free_a(a2); free_a(r);
    }
    {
        /* greater: INT32 */
        int d1[] = {5, 5, 5};
        int d2[] = {4, 6, 5};
        Array *a1 = make_i32_1d(d1, 3);
        Array *a2 = make_i32_1d(d2, 3);
        Array *r = greater(a1, a2);
        ASSERT_NOTNULL(r, "greater INT32 not null");
        uint8_t expected[] = {1, 0, 0};
        ASSERT_EQ_IARR((uint8_t *)r->data, expected, 3, "greater INT32 [5,5,5]>[4,6,5]");
        free_a(a1); free_a(a2); free_a(r);
    }

    /* ── greater_equal ────────────────────────────────────────────── */
    TEST_SECTION("greater_equal");

    {
        /* greater_equal: [1,2,3] vs [2,2,2] -> [0,1,1] */
        double d1[] = {1, 2, 3};
        double d2[] = {2, 2, 2};
        Array *a1 = make_f64_1d(d1, 3);
        Array *a2 = make_f64_1d(d2, 3);
        Array *r = greater_equal(a1, a2);
        ASSERT_NOTNULL(r, "greater_equal not null");
        uint8_t expected[] = {0, 1, 1};
        ASSERT_EQ_IARR((uint8_t *)r->data, expected, 3, "greater_equal [1,2,3]>=[2,2,2]");
        free_a(a1); free_a(a2); free_a(r);
    }
    {
        /* greater_equal: all equal */
        double d1[] = {7, 7, 7};
        double d2[] = {7, 7, 7};
        Array *a1 = make_f64_1d(d1, 3);
        Array *a2 = make_f64_1d(d2, 3);
        Array *r = greater_equal(a1, a2);
        ASSERT_NOTNULL(r, "greater_equal all equal not null");
        uint8_t expected[] = {1, 1, 1};
        ASSERT_EQ_IARR((uint8_t *)r->data, expected, 3, "greater_equal all equal = [1,1,1]");
        free_a(a1); free_a(a2); free_a(r);
    }

    /* ── less ─────────────────────────────────────────────────────── */
    TEST_SECTION("less");

    {
        /* less: [1,3,5] vs [2,2,2] -> [1,0,0] */
        double d1[] = {1, 3, 5};
        double d2[] = {2, 2, 2};
        Array *a1 = make_f64_1d(d1, 3);
        Array *a2 = make_f64_1d(d2, 3);
        Array *r = less(a1, a2);
        ASSERT_NOTNULL(r, "less not null");
        ASSERT_DTYPE(r, BOOL, "less output dtype BOOL");
        uint8_t expected[] = {1, 0, 0};
        ASSERT_EQ_IARR((uint8_t *)r->data, expected, 3, "less [1,3,5]<[2,2,2]");
        free_a(a1); free_a(a2); free_a(r);
    }
    {
        /* less: negative values */
        double d1[] = {-5, -2, 0};
        double d2[] = {-1, -1, -1};
        Array *a1 = make_f64_1d(d1, 3);
        Array *a2 = make_f64_1d(d2, 3);
        Array *r = less(a1, a2);
        ASSERT_NOTNULL(r, "less negatives not null");
        uint8_t expected[] = {1, 1, 0};
        ASSERT_EQ_IARR((uint8_t *)r->data, expected, 3, "less [-5,-2,0]<[-1,-1,-1]");
        free_a(a1); free_a(a2); free_a(r);
    }

    /* ── less_equal ───────────────────────────────────────────────── */
    TEST_SECTION("less_equal");

    {
        /* less_equal: [1,2,3] vs [2,2,2] -> [1,1,0] */
        double d1[] = {1, 2, 3};
        double d2[] = {2, 2, 2};
        Array *a1 = make_f64_1d(d1, 3);
        Array *a2 = make_f64_1d(d2, 3);
        Array *r = less_equal(a1, a2);
        ASSERT_NOTNULL(r, "less_equal not null");
        uint8_t expected[] = {1, 1, 0};
        ASSERT_EQ_IARR((uint8_t *)r->data, expected, 3, "less_equal [1,2,3]<=[2,2,2]");
        free_a(a1); free_a(a2); free_a(r);
    }
    {
        /* less_equal: all equal */
        double d1[] = {9, 9};
        double d2[] = {9, 9};
        Array *a1 = make_f64_1d(d1, 2);
        Array *a2 = make_f64_1d(d2, 2);
        Array *r = less_equal(a1, a2);
        ASSERT_NOTNULL(r, "less_equal all equal not null");
        uint8_t expected[] = {1, 1};
        ASSERT_EQ_IARR((uint8_t *)r->data, expected, 2, "less_equal all equal = [1,1]");
        free_a(a1); free_a(a2); free_a(r);
    }

    /* ── isclose ──────────────────────────────────────────────────── */
    TEST_SECTION("isclose");

    {
        /* isclose: [1.0, 2.0] vs [1.0+1e-7, 2.0] rtol=1e-5 atol=1e-8 -> [1, 1] */
        double d1[] = {1.0, 2.0};
        double d2[] = {1.0 + 1e-7, 2.0};
        Array *a1 = make_f64_1d(d1, 2);
        Array *a2 = make_f64_1d(d2, 2);
        Array *r = isclose(a1, a2, 1e-5, 1e-8, 0);
        ASSERT_NOTNULL(r, "isclose not null");
        ASSERT_DTYPE(r, BOOL, "isclose output dtype BOOL");
        uint8_t expected[] = {1, 1};
        ASSERT_EQ_IARR((uint8_t *)r->data, expected, 2, "isclose within tolerance");
        free_a(a1); free_a(a2); free_a(r);
    }
    {
        /* isclose: not close */
        double d1[] = {1.0, 2.0};
        double d2[] = {1.1, 2.1};
        Array *a1 = make_f64_1d(d1, 2);
        Array *a2 = make_f64_1d(d2, 2);
        Array *r = isclose(a1, a2, 1e-5, 1e-8, 0);
        ASSERT_NOTNULL(r, "isclose not close not null");
        uint8_t expected[] = {0, 0};
        ASSERT_EQ_IARR((uint8_t *)r->data, expected, 2, "isclose not close = [0,0]");
        free_a(a1); free_a(a2); free_a(r);
    }
    {
        /* isclose: equal_nan=1 [1.0, NAN] vs [1.0, NAN] -> [1, 1] */
        double d1[] = {1.0, NAN};
        double d2[] = {1.0, NAN};
        Array *a1 = make_f64_1d(d1, 2);
        Array *a2 = make_f64_1d(d2, 2);
        Array *r = isclose(a1, a2, 1e-5, 1e-8, 1);
        ASSERT_NOTNULL(r, "isclose equal_nan=1 not null");
        uint8_t expected[] = {1, 1};
        ASSERT_EQ_IARR((uint8_t *)r->data, expected, 2, "isclose equal_nan=1 NaN==NaN");
        free_a(a1); free_a(a2); free_a(r);
    }
    {
        /* isclose: equal_nan=0 [1.0, NAN] vs [1.0, NAN] -> [1, 0] */
        double d1[] = {1.0, NAN};
        double d2[] = {1.0, NAN};
        Array *a1 = make_f64_1d(d1, 2);
        Array *a2 = make_f64_1d(d2, 2);
        Array *r = isclose(a1, a2, 1e-5, 1e-8, 0);
        ASSERT_NOTNULL(r, "isclose equal_nan=0 not null");
        uint8_t expected[] = {1, 0};
        ASSERT_EQ_IARR((uint8_t *)r->data, expected, 2, "isclose equal_nan=0 NaN!=NaN");
        free_a(a1); free_a(a2); free_a(r);
    }

    /* ── where ────────────────────────────────────────────────────── */
    TEST_SECTION("where");

    {
        /* where: condition [1,0,1] x=[10,20,30] y=[100,200,300] -> [10,200,30] */
        double cdata[] = {1, 0, 1};
        double xdata[] = {10, 20, 30};
        double ydata[] = {100, 200, 300};
        Array *cond = make_f64_1d(cdata, 3);
        Array *x = make_f64_1d(xdata, 3);
        Array *y = make_f64_1d(ydata, 3);
        Array *r = where(cond, x, y);
        ASSERT_NOTNULL(r, "where not null");
        ASSERT_SIZE(r, 3, "where size=3");
        double expected[] = {10, 200, 30};
        ASSERT_EQ_ARR((double *)r->data, expected, 3, TOL_F64, "where [1,0,1] -> [10,200,30]");
        free_a(cond); free_a(x); free_a(y); free_a(r);
    }
    {
        /* where: INT32 inputs */
        int cdata[] = {0, 1, 0, 1};
        int xdata[] = {1, 2, 3, 4};
        int ydata[] = {9, 8, 7, 6};
        Array *cond = make_i32_1d(cdata, 4);
        Array *x = make_i32_1d(xdata, 4);
        Array *y = make_i32_1d(ydata, 4);
        Array *r = where(cond, x, y);
        ASSERT_NOTNULL(r, "where INT32 not null");
        ASSERT_DTYPE(r, INT32, "where INT32 output dtype");
        int expected[] = {9, 2, 7, 4};
        ASSERT_EQ_IARR((int *)r->data, expected, 4, "where INT32 [0,1,0,1]");
        free_a(cond); free_a(x); free_a(y); free_a(r);
    }
    {
        /* where: all true */
        double cdata[] = {1, 1, 1};
        double xdata[] = {100, 200, 300};
        double ydata[] = {1, 2, 3};
        Array *cond = make_f64_1d(cdata, 3);
        Array *x = make_f64_1d(xdata, 3);
        Array *y = make_f64_1d(ydata, 3);
        Array *r = where(cond, x, y);
        ASSERT_NOTNULL(r, "where all true not null");
        double expected[] = {100, 200, 300};
        ASSERT_EQ_ARR((double *)r->data, expected, 3, TOL_F64, "where all true = x");
        free_a(cond); free_a(x); free_a(y); free_a(r);
    }
    {
        /* where: all false */
        double cdata[] = {0, 0, 0};
        double xdata[] = {1, 2, 3};
        double ydata[] = {7, 8, 9};
        Array *cond = make_f64_1d(cdata, 3);
        Array *x = make_f64_1d(xdata, 3);
        Array *y = make_f64_1d(ydata, 3);
        Array *r = where(cond, x, y);
        ASSERT_NOTNULL(r, "where all false not null");
        double expected[] = {7, 8, 9};
        ASSERT_EQ_ARR((double *)r->data, expected, 3, TOL_F64, "where all false = y");
        free_a(cond); free_a(x); free_a(y); free_a(r);
    }

    TEST_SUMMARY();
    return (g_fail > 0) ? 1 : 0;
}
