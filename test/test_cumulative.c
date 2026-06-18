/**
 * test_cumulative.c — Tests for cumulative functions:
 *   cumsum, cumprod, diff, ediff1d
 * NOTE: cumsum/cumprod/diff/ediff1d preserve input dtype.
 *       INT32 input -> INT32 output data (read as int*).
 *       FLOAT64 input -> FLOAT64 output data (read as double*).
 */

#include "test_utils.h"

int main(void) {
    TEST_MAIN("Cumulative");

    /* ── cumsum ───────────────────────────────────────────────────── */
    TEST_SECTION("cumsum");

    {
        /* cumsum: [1,2,3,4] -> [1,3,6,10] */
        double data[] = {1, 2, 3, 4};
        Array *a = make_f64_1d(data, 4);
        Array *r = cumsum(a, -1);
        ASSERT_NOTNULL(r, "cumsum 1D not null");
        ASSERT_NDIM(r, 1, "cumsum 1D ndim=1");
        ASSERT_SIZE(r, 4, "cumsum 1D size=4");
        double expected[] = {1, 3, 6, 10};
        ASSERT_EQ_ARR((double *)r->data, expected, 4, TOL_F64, "cumsum [1,2,3,4] = [1,3,6,10]");
        free_a(a); free_a(r);
    }
    {
        /* cumsum: INT32 input -> INT32 output */
        int data[] = {5, 10, 15, 20};
        Array *a = make_i32_1d(data, 4);
        Array *r = cumsum(a, -1);
        ASSERT_NOTNULL(r, "cumsum INT32 not null");
        ASSERT_SIZE(r, 4, "cumsum INT32 size=4");
        int expected[] = {5, 15, 30, 50};
        ASSERT_EQ_IARR((int *)r->data, expected, 4, "cumsum INT32 [5,10,15,20]");
        free_a(a); free_a(r);
    }
    {
        /* cumsum: single element */
        double data[] = {7};
        Array *a = make_f64_1d(data, 1);
        Array *r = cumsum(a, -1);
        ASSERT_NOTNULL(r, "cumsum single not null");
        double *rd = (double *)r->data;
        ASSERT_EQ_DBL(rd[0], 7.0, TOL_F64, "cumsum [7] = [7]");
        free_a(a); free_a(r);
    }
    {
        /* cumsum: 2D [[1,2],[3,4]] axis=0 -> [[1,2],[4,6]] */
        double data[] = {1, 2, 3, 4};
        Array *a = make_f64_2d(data, 2, 2);
        Array *r = cumsum(a, 0);
        ASSERT_NOTNULL(r, "cumsum 2D axis=0 not null");
        ASSERT_NDIM(r, 2, "cumsum 2D axis=0 ndim=2");
        ASSERT_SIZE(r, 4, "cumsum 2D axis=0 size=4");
        double expected[] = {1, 2, 4, 6};
        ASSERT_EQ_ARR((double *)r->data, expected, 4, TOL_F64, "cumsum 2D axis=0 = [[1,2],[4,6]]");
        free_a(a); free_a(r);
    }
    {
        /* cumsum: 2D [[1,2],[3,4]] axis=1 -> [[1,3],[3,7]] */
        double data[] = {1, 2, 3, 4};
        Array *a = make_f64_2d(data, 2, 2);
        Array *r = cumsum(a, 1);
        ASSERT_NOTNULL(r, "cumsum 2D axis=1 not null");
        double expected[] = {1, 3, 3, 7};
        ASSERT_EQ_ARR((double *)r->data, expected, 4, TOL_F64, "cumsum 2D axis=1 = [[1,3],[3,7]]");
        free_a(a); free_a(r);
    }

    /* ── cumprod ──────────────────────────────────────────────────── */
    TEST_SECTION("cumprod");

    {
        /* cumprod: [1,2,3,4] -> [1,2,6,24] */
        double data[] = {1, 2, 3, 4};
        Array *a = make_f64_1d(data, 4);
        Array *r = cumprod(a, -1);
        ASSERT_NOTNULL(r, "cumprod 1D not null");
        ASSERT_SIZE(r, 4, "cumprod 1D size=4");
        double expected[] = {1, 2, 6, 24};
        ASSERT_EQ_ARR((double *)r->data, expected, 4, TOL_F64, "cumprod [1,2,3,4] = [1,2,6,24]");
        free_a(a); free_a(r);
    }
    {
        /* cumprod: INT32 input -> INT32 output */
        int data[] = {2, 3, 4};
        Array *a = make_i32_1d(data, 3);
        Array *r = cumprod(a, -1);
        ASSERT_NOTNULL(r, "cumprod INT32 not null");
        int expected[] = {2, 6, 24};
        ASSERT_EQ_IARR((int *)r->data, expected, 3, "cumprod INT32 [2,3,4]");
        free_a(a); free_a(r);
    }
    {
        /* cumprod: includes zero */
        double data[] = {2, 0, 5};
        Array *a = make_f64_1d(data, 3);
        Array *r = cumprod(a, -1);
        ASSERT_NOTNULL(r, "cumprod with zero not null");
        double expected[] = {2, 0, 0};
        ASSERT_EQ_ARR((double *)r->data, expected, 3, TOL_F64, "cumprod [2,0,5] = [2,0,0]");
        free_a(a); free_a(r);
    }
    {
        /* cumprod: single element */
        double data[] = {99};
        Array *a = make_f64_1d(data, 1);
        Array *r = cumprod(a, -1);
        ASSERT_NOTNULL(r, "cumprod single not null");
        double *rd = (double *)r->data;
        ASSERT_EQ_DBL(rd[0], 99.0, TOL_F64, "cumprod [99] = [99]");
        free_a(a); free_a(r);
    }

    /* ── diff ─────────────────────────────────────────────────────── */
    TEST_SECTION("diff");

    {
        double data[] = {1, 3, 6, 10};
        Array *a = make_f64_1d(data, 4);
        Array *r = diff(a, 1, -1);
        ASSERT_NOTNULL(r, "diff n=1 not null");
        ASSERT_SIZE(r, 3, "diff n=1 size=3");
        double expected[] = {2, 3, 4};
        ASSERT_EQ_ARR((double *)r->data, expected, 3, TOL_F64, "diff [1,3,6,10] n=1 = [2,3,4]");
        free_a(a); free_a(r);
    }
    {
        double data[] = {1, 4, 9, 16};
        Array *a = make_f64_1d(data, 4);
        Array *r = diff(a, 2, -1);
        ASSERT_NOTNULL(r, "diff n=2 not null");
        ASSERT_SIZE(r, 2, "diff n=2 size=2");
        double expected[] = {2, 2};
        ASSERT_EQ_ARR((double *)r->data, expected, 2, TOL_F64, "diff [1,4,9,16] n=2 = [2,2]");
        free_a(a); free_a(r);
    }
    {
        /* diff: INT32 input -> INT32 output */
        int data[] = {10, 20, 35};
        Array *a = make_i32_1d(data, 3);
        Array *r = diff(a, 1, -1);
        ASSERT_NOTNULL(r, "diff INT32 not null");
        int expected[] = {10, 15};
        ASSERT_EQ_IARR((int *)r->data, expected, 2, "diff INT32 [10,20,35] n=1");
        free_a(a); free_a(r);
    }
    {
        double data[] = {1, 2, 3, 4};
        Array *a = make_f64_2d(data, 2, 2);
        Array *r = diff(a, 1, 0);
        ASSERT_NOTNULL(r, "diff 2D axis=0 not null");
        ASSERT_NDIM(r, 2, "diff 2D axis=0 ndim=2");
        ASSERT_EQ_INT(r->shape[0], 1, "diff 2D axis=0 shape[0]=1");
        ASSERT_EQ_INT(r->shape[1], 2, "diff 2D axis=0 shape[1]=2");
        double expected[] = {2, 2};
        ASSERT_EQ_ARR((double *)r->data, expected, 2, TOL_F64, "diff 2D axis=0 = [[2,2]]");
        free_a(a); free_a(r);
    }
    {
        double data[] = {1, 2, 3, 4};
        Array *a = make_f64_2d(data, 2, 2);
        Array *r = diff(a, 1, 1);
        ASSERT_NOTNULL(r, "diff 2D axis=1 not null");
        ASSERT_EQ_INT(r->shape[0], 2, "diff 2D axis=1 shape[0]=2");
        ASSERT_EQ_INT(r->shape[1], 1, "diff 2D axis=1 shape[1]=1");
        double expected[] = {1, 1};
        ASSERT_EQ_ARR((double *)r->data, expected, 2, TOL_F64, "diff 2D axis=1 = [[1],[1]]");
        free_a(a); free_a(r);
    }

    /* ── ediff1d ──────────────────────────────────────────────────── */
    TEST_SECTION("ediff1d");

    {
        double data[] = {1, 2, 4, 7};
        Array *a = make_f64_1d(data, 4);
        Array *r = ediff1d(a, NULL, NULL);
        ASSERT_NOTNULL(r, "ediff1d no extra not null");
        ASSERT_SIZE(r, 3, "ediff1d no extra size=3");
        double expected[] = {1, 2, 3};
        ASSERT_EQ_ARR((double *)r->data, expected, 3, TOL_F64, "ediff1d [1,2,4,7] = [1,2,3]");
        free_a(a); free_a(r);
    }
    {
        double data[] = {1, 2, 4, 7};
        double to_begin_data[] = {0};
        double to_end_data[] = {10};
        Array *a = make_f64_1d(data, 4);
        Array *to_begin = make_f64_1d(to_begin_data, 1);
        Array *to_end = make_f64_1d(to_end_data, 1);
        Array *r = ediff1d(a, to_begin, to_end);
        ASSERT_NOTNULL(r, "ediff1d with extras not null");
        ASSERT_SIZE(r, 5, "ediff1d with extras size=5");
        double expected[] = {0, 1, 2, 3, 10};
        ASSERT_EQ_ARR((double *)r->data, expected, 5, TOL_F64, "ediff1d + [0],[10] = [0,1,2,3,10]");
        free_a(a); free_a(to_begin); free_a(to_end); free_a(r);
    }
    {
        /* ediff1d: INT32 input -> INT32 output */
        int data[] = {10, 20, 50};
        Array *a = make_i32_1d(data, 3);
        Array *r = ediff1d(a, NULL, NULL);
        ASSERT_NOTNULL(r, "ediff1d INT32 not null");
        int expected[] = {10, 30};
        ASSERT_EQ_IARR((int *)r->data, expected, 2, "ediff1d INT32 [10,20,50] = [10,30]");
        free_a(a); free_a(r);
    }

    TEST_SUMMARY();
    return (g_fail > 0) ? 1 : 0;
}
