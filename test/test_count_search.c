/**
 * test_count_search.c — Tests for counting and search functions:
 *   count_nonzero, argwhere, searchsorted, digitize, interp
 */

#include "test_utils.h"

int main(void) {
    TEST_MAIN("Count & Search");

    /* ── count_nonzero ────────────────────────────────────────────── */
    TEST_SECTION("count_nonzero");

    {
        /* count_nonzero: [0,1,0,2,0,3] -> 3 */
        double d[] = {0, 1, 0, 2, 0, 3};
        Array *a = make_f64_1d(d, 6);
        Array *r = count_nonzero(a, -1);
        ASSERT_NOTNULL(r, "count_nonzero 1D not null");
        /* count_nonzero returns a scalar array; verify value */
        int *rd = (int *)r->data;
        ASSERT_EQ_INT(rd[0], 3, "count_nonzero [0,1,0,2,0,3] = 3");
        free_a(a); free_a(r);
    }
    {
        /* count_nonzero: all zero */
        double d[] = {0, 0, 0};
        Array *a = make_f64_1d(d, 3);
        Array *r = count_nonzero(a, -1);
        ASSERT_NOTNULL(r, "count_nonzero all zero not null");
        int *rd = (int *)r->data;
        ASSERT_EQ_INT(rd[0], 0, "count_nonzero [0,0,0] = 0");
        free_a(a); free_a(r);
    }
    {
        /* count_nonzero: all non-zero */
        double d[] = {1, 2, -3};
        Array *a = make_f64_1d(d, 3);
        Array *r = count_nonzero(a, -1);
        ASSERT_NOTNULL(r, "count_nonzero all non-zero not null");
        int *rd = (int *)r->data;
        ASSERT_EQ_INT(rd[0], 3, "count_nonzero [1,2,-3] = 3");
        free_a(a); free_a(r);
    }
    {
        /* count_nonzero: INT32 */
        int d[] = {0, 5, 0, -1, 0};
        Array *a = make_i32_1d(d, 5);
        Array *r = count_nonzero(a, -1);
        ASSERT_NOTNULL(r, "count_nonzero INT32 not null");
        int *rd = (int *)r->data;
        ASSERT_EQ_INT(rd[0], 2, "count_nonzero INT32 [0,5,0,-1,0] = 2");
        free_a(a); free_a(r);
    }
    {
        /* count_nonzero: 2D axis=0 */
        double d[] = {0, 1, 0, 2, 0, 0};
        Array *a = make_f64_2d(d, 2, 3);
        Array *r = count_nonzero(a, 0);
        ASSERT_NOTNULL(r, "count_nonzero 2D axis=0 not null");
        ASSERT_SIZE(r, 3, "count_nonzero 2D axis=0 size=3");
        int64_t *rd = (int64_t *)r->data;
        ASSERT_EQ_I64(rd[0], 1, "count_nonzero 2D axis=0 val[0]");
        ASSERT_EQ_I64(rd[1], 1, "count_nonzero 2D axis=0 val[1]");
        ASSERT_EQ_I64(rd[2], 0, "count_nonzero 2D axis=0 val[2]");
        free_a(a); free_a(r);
    }

    /* ── argwhere ─────────────────────────────────────────────────── */
    TEST_SECTION("argwhere");

    {
        double d[] = {0, 1, 0, 2, 0};
        Array *a = make_f64_1d(d, 5);
        Array *r = argwhere(a);
        ASSERT_NOTNULL(r, "argwhere not null");
        ASSERT_NDIM(r, 2, "argwhere ndim=2");
        ASSERT_EQ_INT(r->shape[0], 2, "argwhere shape[0]=2 (non-zero count)");
        ASSERT_DTYPE(r, INT64, "argwhere output dtype INT64");
        long long *rd = (long long *)r->data;
        ASSERT_EQ_I64(rd[0], 1, "argwhere idx 0 = 1");
        ASSERT_EQ_I64(rd[1], 3, "argwhere idx 1 = 3");
        free_a(a); free_a(r);
    }
    {
        double d[] = {0, 0, 0};
        Array *a = make_f64_1d(d, 3);
        Array *r = argwhere(a);
        ASSERT_NOTNULL(r, "argwhere all zero not null");
        ASSERT_EQ_INT(r->shape[0], 0, "argwhere all zero -> empty");
        free_a(a); free_a(r);
    }
    {
        int d[] = {0, -7, 0, 3};
        Array *a = make_i32_1d(d, 4);
        Array *r = argwhere(a);
        ASSERT_NOTNULL(r, "argwhere INT32 not null");
        ASSERT_EQ_INT(r->shape[0], 2, "argwhere INT32 count=2");
        long long *rd = (long long *)r->data;
        ASSERT_EQ_I64(rd[0], 1, "argwhere INT32 idx 0 = 1");
        ASSERT_EQ_I64(rd[1], 3, "argwhere INT32 idx 1 = 3");
        free_a(a); free_a(r);
    }

    /* ── searchsorted ─────────────────────────────────────────────── */
    TEST_SECTION("searchsorted");

    {
        double sorted[] = {1, 3, 5, 7};
        double vals[] = {2, 4, 6};
        Array *a = make_f64_1d(sorted, 4);
        Array *v = make_f64_1d(vals, 3);
        Array *r = searchsorted(a, v, "left");
        ASSERT_NOTNULL(r, "searchsorted left not null");
        ASSERT_SIZE(r, 3, "searchsorted left size=3");
        int64_t *rd = (int64_t *)r->data;
        ASSERT_EQ_I64(rd[0], 1, "searchsorted left 2 -> index 1");
        ASSERT_EQ_I64(rd[1], 2, "searchsorted left 4 -> index 2");
        ASSERT_EQ_I64(rd[2], 3, "searchsorted left 6 -> index 3");
        free_a(a); free_a(v); free_a(r);
    }
    {
        double sorted[] = {1, 3, 5, 7};
        double vals[] = {1, 3, 5};
        Array *a = make_f64_1d(sorted, 4);
        Array *v = make_f64_1d(vals, 3);
        Array *r = searchsorted(a, v, "right");
        ASSERT_NOTNULL(r, "searchsorted right not null");
        int64_t *rd = (int64_t *)r->data;
        ASSERT_EQ_I64(rd[0], 1, "searchsorted right 1 -> index 1");
        ASSERT_EQ_I64(rd[1], 2, "searchsorted right 3 -> index 2");
        ASSERT_EQ_I64(rd[2], 3, "searchsorted right 5 -> index 3");
        free_a(a); free_a(v); free_a(r);
    }
    {
        int sorted[] = {10, 20, 30, 40};
        int vals[] = {15, 25};
        Array *a = make_i32_1d(sorted, 4);
        Array *v = make_i32_1d(vals, 2);
        Array *r = searchsorted(a, v, "left");
        ASSERT_NOTNULL(r, "searchsorted INT32 not null");
        int64_t *rd = (int64_t *)r->data;
        ASSERT_EQ_I64(rd[0], 1, "searchsorted INT32 15 -> 1");
        ASSERT_EQ_I64(rd[1], 2, "searchsorted INT32 25 -> 2");
        free_a(a); free_a(v); free_a(r);
    }
    {
        double sorted[] = {1, 3, 5};
        double vals[] = {-1};
        Array *a = make_f64_1d(sorted, 3);
        Array *v = make_f64_1d(vals, 1);
        Array *r = searchsorted(a, v, "left");
        ASSERT_NOTNULL(r, "searchsorted below all not null");
        int64_t *rd = (int64_t *)r->data;
        ASSERT_EQ_I64(rd[0], 0, "searchsorted -1 -> 0");
        free_a(a); free_a(v); free_a(r);
    }
    {
        double sorted[] = {1, 3, 5};
        double vals[] = {10};
        Array *a = make_f64_1d(sorted, 3);
        Array *v = make_f64_1d(vals, 1);
        Array *r = searchsorted(a, v, "left");
        ASSERT_NOTNULL(r, "searchsorted above all not null");
        int64_t *rd = (int64_t *)r->data;
        ASSERT_EQ_I64(rd[0], 3, "searchsorted 10 -> 3");
        free_a(a); free_a(v); free_a(r);
    }

    /* ── digitize ─────────────────────────────────────────────────── */
    TEST_SECTION("digitize");

    {
        double d[] = {0.2, 1.5, 3.7, 5.0};
        double bins[] = {1, 3, 5};
        Array *a = make_f64_1d(d, 4);
        Array *b = make_f64_1d(bins, 3);
        Array *r = digitize(a, b, 0);
        ASSERT_NOTNULL(r, "digitize right=0 not null");
        ASSERT_SIZE(r, 4, "digitize size=4");
        int64_t *rd = (int64_t *)r->data;
        ASSERT_EQ_I64(rd[0], 0, "digitize 0.2 -> 0");
        ASSERT_EQ_I64(rd[1], 1, "digitize 1.5 -> 1");
        ASSERT_EQ_I64(rd[2], 2, "digitize 3.7 -> 2");
        ASSERT_EQ_I64(rd[3], 3, "digitize 5.0 -> 3");
        free_a(a); free_a(b); free_a(r);
    }
    {
        double d[] = {0.2, 1.5, 3.7, 5.0};
        double bins[] = {1, 3, 5};
        Array *a = make_f64_1d(d, 4);
        Array *b = make_f64_1d(bins, 3);
        Array *r = digitize(a, b, 1);
        ASSERT_NOTNULL(r, "digitize right=1 not null");
        int64_t *rd = (int64_t *)r->data;
        ASSERT_EQ_I64(rd[0], 0, "digitize right=1 0.2 -> 0");
        ASSERT_EQ_I64(rd[1], 1, "digitize right=1 1.5 -> 1");
        ASSERT_EQ_I64(rd[2], 2, "digitize right=1 3.7 -> 2");
        ASSERT_EQ_I64(rd[3], 2, "digitize right=1 5.0 -> 2");
        free_a(a); free_a(b); free_a(r);
    }

    /* ── interp ───────────────────────────────────────────────────── */
    TEST_SECTION("interp");

    {
        double xd[] = {1, 2, 3};
        double xpd[] = {1, 2, 3};
        double fpd[] = {10, 20, 30};
        Array *x = make_f64_1d(xd, 3);
        Array *xp = make_f64_1d(xpd, 3);
        Array *fp = make_f64_1d(fpd, 3);
        Array *r = interp(x, xp, fp, NULL, NULL);
        ASSERT_NOTNULL(r, "interp exact match not null");
        ASSERT_SIZE(r, 3, "interp exact match size=3");
        double expected[] = {10, 20, 30};
        ASSERT_EQ_ARR((double *)r->data, expected, 3, TOL_F64, "interp exact = [10,20,30]");
        free_a(x); free_a(xp); free_a(fp); free_a(r);
    }
    {
        double xd[] = {1.5, 2.5};
        double xpd[] = {1, 2, 3};
        double fpd[] = {2, 4, 6};
        Array *x = make_f64_1d(xd, 2);
        Array *xp = make_f64_1d(xpd, 3);
        Array *fp = make_f64_1d(fpd, 3);
        Array *r = interp(x, xp, fp, NULL, NULL);
        ASSERT_NOTNULL(r, "interp midpoints not null");
        double expected[] = {3, 5};
        ASSERT_EQ_ARR((double *)r->data, expected, 2, TOL_F64, "interp [1.5,2.5] = [3,5]");
        free_a(x); free_a(xp); free_a(fp); free_a(r);
    }
    {
        double xd[] = {0, 2, 4};
        double xpd[] = {1, 2, 3};
        double fpd[] = {10, 20, 30};
        double left_val = 42;
        double right_val = 99;
        Array *x = make_f64_1d(xd, 3);
        Array *xp = make_f64_1d(xpd, 3);
        Array *fp = make_f64_1d(fpd, 3);
        Array *left = make_f64_1d(&left_val, 1);
        Array *right = make_f64_1d(&right_val, 1);
        Array *r = interp(x, xp, fp, left, right);
        ASSERT_NOTNULL(r, "interp extrapolation not null");
        double expected[] = {42, 20, 99};
        ASSERT_EQ_ARR((double *)r->data, expected, 3, TOL_F64, "interp extrap left=42 right=99");
        free_a(x); free_a(xp); free_a(fp); free_a(left); free_a(right); free_a(r);
    }
    {
        int xd[] = {5};
        int xpd[] = {0, 10};
        int fpd[] = {0, 100};
        Array *x = make_i32_1d(xd, 1);
        Array *xp = make_i32_1d(xpd, 2);
        Array *fp = make_i32_1d(fpd, 2);
        Array *r = interp(x, xp, fp, NULL, NULL);
        ASSERT_NOTNULL(r, "interp INT32 not null");
        double expected[] = {50};
        ASSERT_EQ_ARR((double *)r->data, expected, 1, TOL_F64, "interp INT32 x=5 -> 50");
        free_a(x); free_a(xp); free_a(fp); free_a(r);
    }

    TEST_SUMMARY();
    return (g_fail > 0) ? 1 : 0;
}
