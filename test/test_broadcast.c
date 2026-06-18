#include "test_utils.h"


int main(void) {
    TEST_MAIN("test_broadcast");

    /* ── broadcast_to ────────────────────────────────────────────── */
    TEST_SECTION("broadcast_to");

    /* broadcast_to: scalar [42] -> (3,) */
    {
        double data[] = {42};
        double expected[] = {42, 42, 42};
        Array *a = make_f64_1d(data, 1);
        int shape[] = {3};
        Array *r = broadcast_to(a, shape, 1);
        ASSERT_NOTNULL(r, "broadcast_to scalar non-null");
        ASSERT_NDIM(r, 1, "broadcast_to scalar ndim=1");
        ASSERT_SIZE(r, 3, "broadcast_to scalar size=3");
        ASSERT_EQ_ARR((double*)r->data, expected, 3, TOL_F64, "broadcast_to scalar values");
        free_a(r);
        free_a(a);
    }

    /* broadcast_to: [1,2,3] -> (2,3) */
    {
        double data[] = {1, 2, 3};
        double expected[] = {1, 2, 3, 1, 2, 3};
        Array *a = make_f64_1d(data, 3);
        int shape[] = {2, 3};
        Array *r = broadcast_to(a, shape, 2);
        ASSERT_NOTNULL(r, "broadcast_to 1D->2D non-null");
        ASSERT_NDIM(r, 2, "broadcast_to 1D->2D ndim=2");
        ASSERT_EQ_INT(r->shape[0], 2, "broadcast_to shape[0]=2");
        ASSERT_EQ_INT(r->shape[1], 3, "broadcast_to shape[1]=3");
        ASSERT_EQ_ARR((double*)r->data, expected, 6, TOL_F64, "broadcast_to 1D->2D values");
        free_a(r);
        free_a(a);
    }

    /* broadcast_to: [1,2,3] -> (2,3) INT32 */
    {
        int data[] = {10, 20, 30};
        int expected[] = {10, 20, 30, 10, 20, 30};
        Array *a = make_i32_1d(data, 3);
        int shape[] = {2, 3};
        Array *r = broadcast_to(a, shape, 2);
        ASSERT_NOTNULL(r, "broadcast_to INT32 non-null");
        ASSERT_NDIM(r, 2, "broadcast_to INT32 ndim=2");
        ASSERT_EQ_IARR((int*)r->data, expected, 6, "broadcast_to INT32 values");
        free_a(r);
        free_a(a);
    }

    /* broadcast_to: 2D (1,3) -> (4,3) */
    {
        double data[] = {1, 2, 3};
        double expected[] = {1, 2, 3, 1, 2, 3, 1, 2, 3, 1, 2, 3};
        Array *a = make_f64_2d(data, 1, 3);
        int shape[] = {4, 3};
        Array *r = broadcast_to(a, shape, 2);
        ASSERT_NOTNULL(r, "broadcast_to (1,3)->(4,3) non-null");
        ASSERT_NDIM(r, 2, "broadcast_to (1,3)->(4,3) ndim=2");
        ASSERT_EQ_INT(r->shape[0], 4, "broadcast_to shape[0]=4");
        ASSERT_EQ_INT(r->shape[1], 3, "broadcast_to shape[1]=3");
        ASSERT_EQ_ARR((double*)r->data, expected, 12, TOL_F64, "broadcast_to (1,3)->(4,3) values");
        free_a(r);
        free_a(a);
    }

    /* ── broadcast_arrays ────────────────────────────────────────── */
    TEST_SECTION("broadcast_arrays");

    /* broadcast_arrays: [1,2,3] and [[1],[2]] -> both 2x3 */
    {
        double d1[] = {1, 2, 3};
        double d2[] = {1, 2}; /* column vector */
        Array *a1 = make_f64_1d(d1, 3);
        Array *a2 = make_f64_2d(d2, 2, 1);
        Array* arrs[] = {a1, a2};
        int out_ndim = 0;
        int *out_shape = NULL;
        Array **r = broadcast_arrays(arrs, 2, &out_ndim, &out_shape);
        ASSERT_NOTNULL(r, "broadcast_arrays non-null");
        ASSERT_EQ_INT(out_ndim, 2, "broadcast_arrays out_ndim=2");

        /* a1 broadcast: [[1,2,3],[1,2,3]] */
        double ex1[] = {1, 2, 3, 1, 2, 3};
        ASSERT_EQ_ARR((double*)r[0]->data, ex1, 6, TOL_F64, "broadcast_arrays r[0] values");

        /* a2 broadcast: [[1,1,1],[2,2,2]] */
        double ex2[] = {1, 1, 1, 2, 2, 2};
        ASSERT_EQ_ARR((double*)r[1]->data, ex2, 6, TOL_F64, "broadcast_arrays r[1] values");

        free_a(r[0]); free_a(r[1]); free(r);
        if (out_shape) free(out_shape);
        free_a(a1); free_a(a2);
    }

    /* broadcast_arrays INT32 */
    {
        int d1[] = {10, 20};
        int d2[] = {30}; /* scalar-like */
        Array *a1 = make_i32_1d(d1, 2);
        Array *a2 = make_i32_1d(d2, 1);
        Array* arrs[] = {a1, a2};
        int out_ndim = 0;
        int *out_shape = NULL;
        Array **r = broadcast_arrays(arrs, 2, &out_ndim, &out_shape);
        ASSERT_NOTNULL(r, "broadcast_arrays INT32 non-null");
        ASSERT_EQ_INT(out_ndim, 1, "broadcast_arrays INT32 out_ndim=1");

        int ex1[] = {10, 20};
        ASSERT_EQ_IARR((int*)r[0]->data, ex1, 2, "broadcast_arrays INT32 r[0]");

        int ex2[] = {30, 30};
        ASSERT_EQ_IARR((int*)r[1]->data, ex2, 2, "broadcast_arrays INT32 r[1]");

        free_a(r[0]); free_a(r[1]); free(r);
        if (out_shape) free(out_shape);
        free_a(a1); free_a(a2);
    }

    /* broadcast_arrays same shape -> unchanged */
    {
        double d1[] = {1, 2, 3};
        double d2[] = {4, 5, 6};
        Array *a1 = make_f64_1d(d1, 3);
        Array *a2 = make_f64_1d(d2, 3);
        Array* arrs[] = {a1, a2};
        int out_ndim = 0;
        int *out_shape = NULL;
        Array **r = broadcast_arrays(arrs, 2, &out_ndim, &out_shape);
        ASSERT_NOTNULL(r, "broadcast_arrays same shape non-null");
        ASSERT_EQ_INT(out_ndim, 1, "broadcast_arrays same shape out_ndim=1");
        ASSERT_EQ_ARR((double*)r[0]->data, d1, 3, TOL_F64, "broadcast same shape r[0]");
        ASSERT_EQ_ARR((double*)r[1]->data, d2, 3, TOL_F64, "broadcast same shape r[1]");
        free_a(r[0]); free_a(r[1]); free(r);
        if (out_shape) free(out_shape);
        free_a(a1); free_a(a2);
    }

    TEST_SUMMARY();
    return (g_fail > 0) ? 1 : 0;
}
