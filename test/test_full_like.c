/**
 * test_full_like.c — Tests for full_like function:
 *   full_like with same dtype, different fill values, shape preservation,
 *   and dtype override
 */

#include "test_utils.h"

int main(void) {
    TEST_MAIN("full_like");

    /* ── full_like: same dtype, FLOAT64 with int fill value 7 ───── */
    TEST_SECTION("full_like: FLOAT64, int fill=7");

    {
        double data[] = {1.0, 2.0, 3.0, 4.0};
        Array *orig = create_array((int[]){4}, 1, FLOAT64);
        memcpy(orig->data, data, 4 * sizeof(double));

        int fill = 7;
        Array *a = full_like(orig, &fill, INT32);
        ASSERT_NOTNULL(a, "full_like not null");
        ASSERT_DTYPE(a, INT32, "full_like dtype=INT32");
        ASSERT_NDIM(a, 1, "full_like ndim=1");
        ASSERT_SIZE(a, 4, "full_like size=4");
        /* shape preserved */
        ASSERT_EQ_INT(a->shape[0], 4, "full_like shape[0]=4");

        int *d = (int *)a->data;
        int expected[] = {7, 7, 7, 7};
        ASSERT_EQ_IARR(d, expected, 4, "full_like all elements=7");

        orig = free_a(orig);
        a = free_a(a);
    }

    /* ── full_like: INT32 with float fill value 3.14 ─────────────── */
    TEST_SECTION("full_like: INT32, float fill=3.14");

    {
        int data[] = {1, 2, 3};
        Array *orig = create_array((int[]){3}, 1, INT32);
        memcpy(orig->data, data, 3 * sizeof(int));

        double fill = 3.14;
        Array *a = full_like(orig, &fill, FLOAT64);
        ASSERT_NOTNULL(a, "full_like float not null");
        ASSERT_DTYPE(a, FLOAT64, "full_like float dtype=FLOAT64");
        ASSERT_NDIM(a, 1, "full_like float ndim=1");
        ASSERT_SIZE(a, 3, "full_like float size=3");

        double *d = (double *)a->data;
        double expected[] = {3.14, 3.14, 3.14};
        ASSERT_EQ_ARR(d, expected, 3, TOL_F64, "full_like all elements=3.14");

        orig = free_a(orig);
        a = free_a(a);
    }

    /* ── full_like: 2D shape preserved ──────────────────────────── */
    TEST_SECTION("full_like: 2D shape");

    {
        double data[] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
        Array *orig = create_array((int[]){2, 3}, 2, FLOAT64);
        memcpy(orig->data, data, 6 * sizeof(double));

        int fill = 99;
        Array *a = full_like(orig, &fill, INT32);
        ASSERT_NOTNULL(a, "full_like 2D not null");
        ASSERT_NDIM(a, 2, "full_like 2D ndim=2");
        ASSERT_SIZE(a, 6, "full_like 2D size=6");
        ASSERT_EQ_INT(a->shape[0], 2, "full_like 2D shape[0]=2");
        ASSERT_EQ_INT(a->shape[1], 3, "full_like 2D shape[1]=3");

        int *d = (int *)a->data;
        int expected[] = {99, 99, 99, 99, 99, 99};
        ASSERT_EQ_IARR(d, expected, 6, "full_like 2D all 99");

        orig = free_a(orig);
        a = free_a(a);
    }

    /* ── full_like: dtype override ───────────────────────────────── */
    TEST_SECTION("full_like: dtype override");

    {
        /* create INT32 array, full_like specifying FLOAT64 */
        int data[] = {1, 2, 3, 4};
        Array *orig = create_array((int[]){4}, 1, INT32);
        memcpy(orig->data, data, 4 * sizeof(int));

        double fill = -3.5;
        Array *a = full_like(orig, &fill, FLOAT64);
        ASSERT_NOTNULL(a, "full_like override not null");
        ASSERT_DTYPE(a, FLOAT64, "full_like override dtype=FLOAT64");
        ASSERT_NDIM(a, 1, "full_like override ndim=1");
        ASSERT_SIZE(a, 4, "full_like override size=4");
        ASSERT_EQ_INT(a->shape[0], 4, "full_like override shape[0]=4");

        double *d = (double *)a->data;
        double expected[] = {-3.5, -3.5, -3.5, -3.5};
        ASSERT_EQ_ARR(d, expected, 4, TOL_F64, "full_like override all -3.5");

        orig = free_a(orig);
        a = free_a(a);
    }

    /* full_like with 0D not applicable (create_array rejects ndim=0) */

    /* ── full_like: negative fill value ─────────────────────────── */
    TEST_SECTION("full_like: negative fill");

    {
        int data[] = {10, 20};
        Array *orig = create_array((int[]){2}, 1, INT32);
        memcpy(orig->data, data, 2 * sizeof(int));

        int fill = -1;
        Array *a = full_like(orig, &fill, INT32);
        ASSERT_NOTNULL(a, "full_like neg not null");
        ASSERT_SIZE(a, 2, "full_like neg size=2");

        int *d = (int *)a->data;
        int expected[] = {-1, -1};
        ASSERT_EQ_IARR(d, expected, 2, "full_like all -1");

        orig = free_a(orig);
        a = free_a(a);
    }

    TEST_SUMMARY();
    return (g_fail > 0) ? 1 : 0;
}
