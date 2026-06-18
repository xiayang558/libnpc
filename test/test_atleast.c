#include "test_utils.h"


int main(void) {
    TEST_MAIN("test_atleast");

    /* ── atleast_1d ──────────────────────────────────────────────── */
    TEST_SECTION("atleast_1d");

    /* atleast_1d: scalar (1D size-1) -> still 1D */
    {
        double data[] = {42};
        double expected[] = {42};
        Array *a = make_f64_1d(data, 1);
        Array *r = atleast_1d(a);
        ASSERT_NOTNULL(r, "atleast_1d scalar non-null");
        ASSERT_NDIM(r, 1, "atleast_1d scalar ndim=1");
        ASSERT_SIZE(r, 1, "atleast_1d scalar size=1");
        ASSERT_EQ_ARR((double*)r->data, expected, 1, TOL_F64, "atleast_1d scalar value");
        free_a(r);
        free_a(a);
    }

    /* atleast_1d: 1D [1,2,3] -> unchanged */
    {
        double data[] = {1, 2, 3};
        double expected[] = {1, 2, 3};
        Array *a = make_f64_1d(data, 3);
        Array *r = atleast_1d(a);
        ASSERT_NOTNULL(r, "atleast_1d 1D non-null");
        ASSERT_NDIM(r, 1, "atleast_1d 1D ndim=1");
        ASSERT_SIZE(r, 3, "atleast_1d 1D size=3");
        ASSERT_EQ_ARR((double*)r->data, expected, 3, TOL_F64, "atleast_1d 1D values");
        free_a(r);
        free_a(a);
    }

    /* atleast_1d INT32 */
    {
        int data[] = {10, 20, 30};
        int expected[] = {10, 20, 30};
        Array *a = make_i32_1d(data, 3);
        Array *r = atleast_1d(a);
        ASSERT_NOTNULL(r, "atleast_1d INT32 non-null");
        ASSERT_NDIM(r, 1, "atleast_1d INT32 ndim=1");
        ASSERT_EQ_IARR((int*)r->data, expected, 3, "atleast_1d INT32 values");
        free_a(r);
        free_a(a);
    }

    /* ── atleast_2d ──────────────────────────────────────────────── */
    TEST_SECTION("atleast_2d");

    /* atleast_2d: 1D [1,2,3] -> [[1,2,3]] (1x3) */
    {
        double data[] = {1, 2, 3};
        double expected[] = {1, 2, 3};
        Array *a = make_f64_1d(data, 3);
        Array *r = atleast_2d(a);
        ASSERT_NOTNULL(r, "atleast_2d 1D non-null");
        ASSERT_NDIM(r, 2, "atleast_2d 1D ndim=2");
        ASSERT_EQ_INT(r->shape[0], 3, "atleast_2d 1D shape[0]=3");
        ASSERT_EQ_INT(r->shape[1], 1, "atleast_2d 1D shape[1]=1");
        ASSERT_EQ_ARR((double*)r->data, expected, 3, TOL_F64, "atleast_2d 1D values");
        free_a(r);
        free_a(a);
    }

    /* atleast_2d: 2D already -> unchanged */
    {
        double data[] = {1, 2, 3, 4};
        double expected[] = {1, 2, 3, 4};
        Array *a = make_f64_2d(data, 2, 2);
        Array *r = atleast_2d(a);
        ASSERT_NOTNULL(r, "atleast_2d 2D non-null");
        ASSERT_NDIM(r, 2, "atleast_2d 2D ndim=2");
        ASSERT_EQ_INT(r->shape[0], 2, "atleast_2d 2D shape[0]");
        ASSERT_EQ_INT(r->shape[1], 2, "atleast_2d 2D shape[1]");
        ASSERT_EQ_ARR((double*)r->data, expected, 4, TOL_F64, "atleast_2d 2D values");
        free_a(r);
        free_a(a);
    }

    /* atleast_2d INT32 */
    {
        int data[] = {10, 20, 30, 40};
        int expected[] = {10, 20, 30, 40};
        Array *a = make_i32_1d(data, 4);
        Array *r = atleast_2d(a);
        ASSERT_NOTNULL(r, "atleast_2d INT32 non-null");
        ASSERT_NDIM(r, 2, "atleast_2d INT32 ndim=2");
        ASSERT_EQ_INT(r->shape[0], 4, "atleast_2d INT32 shape[0]=4");
        ASSERT_EQ_INT(r->shape[1], 1, "atleast_2d INT32 shape[1]=1");
        ASSERT_EQ_IARR((int*)r->data, expected, 4, "atleast_2d INT32 values");
        free_a(r);
        free_a(a);
    }

    /* ── atleast_3d ──────────────────────────────────────────────── */
    TEST_SECTION("atleast_3d");

    /* atleast_3d: 1D -> 3D (1,1,n) */
    {
        double data[] = {1, 2, 3};
        double expected[] = {1, 2, 3};
        Array *a = make_f64_1d(data, 3);
        Array *r = atleast_3d(a);
        ASSERT_NOTNULL(r, "atleast_3d 1D non-null");
        ASSERT_NDIM(r, 3, "atleast_3d 1D ndim=3");
        ASSERT_EQ_INT(r->shape[0], 3, "atleast_3d 1D shape[0]=3");
        ASSERT_EQ_INT(r->shape[1], 1, "atleast_3d 1D shape[1]=1");
        ASSERT_EQ_INT(r->shape[2], 1, "atleast_3d 1D shape[2]=1");
        ASSERT_EQ_ARR((double*)r->data, expected, 3, TOL_F64, "atleast_3d 1D values");
        free_a(r);
        free_a(a);
    }

    /* atleast_3d: 2D -> 3D (1,m,n) */
    {
        double data[] = {1, 2, 3, 4, 5, 6};
        double expected[] = {1, 2, 3, 4, 5, 6};
        Array *a = make_f64_2d(data, 2, 3);
        Array *r = atleast_3d(a);
        ASSERT_NOTNULL(r, "atleast_3d 2D non-null");
        ASSERT_NDIM(r, 3, "atleast_3d 2D ndim=3");
        ASSERT_EQ_INT(r->shape[0], 2, "atleast_3d 2D shape[0]=2");
        ASSERT_EQ_INT(r->shape[1], 3, "atleast_3d 2D shape[1]=3");
        ASSERT_EQ_INT(r->shape[2], 1, "atleast_3d 2D shape[2]=1");
        ASSERT_EQ_ARR((double*)r->data, expected, 6, TOL_F64, "atleast_3d 2D values");
        free_a(r);
        free_a(a);
    }

    /* atleast_3d INT32 */
    {
        int data[] = {100, 200};
        int expected[] = {100, 200};
        Array *a = make_i32_1d(data, 2);
        Array *r = atleast_3d(a);
        ASSERT_NOTNULL(r, "atleast_3d INT32 non-null");
        ASSERT_NDIM(r, 3, "atleast_3d INT32 ndim=3");
        ASSERT_EQ_INT(r->shape[0], 2, "atleast_3d INT32 shape[0]");
        ASSERT_EQ_INT(r->shape[1], 1, "atleast_3d INT32 shape[1]");
        ASSERT_EQ_INT(r->shape[2], 1, "atleast_3d INT32 shape[2]");
        ASSERT_EQ_IARR((int*)r->data, expected, 2, "atleast_3d INT32 values");
        free_a(r);
        free_a(a);
    }

    TEST_SUMMARY();
    return (g_fail > 0) ? 1 : 0;
}
