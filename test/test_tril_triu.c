#include "test_utils.h"


int main(void) {
    TEST_MAIN("test_tril_triu");

    /* ── tri ─────────────────────────────────────────────────────── */
    TEST_SECTION("tri");

    /* tri N=3 M=3 k=0 -> [[1,0,0],[1,1,0],[1,1,1]] */
    {
        double expected[] = {1, 0, 0, 1, 1, 0, 1, 1, 1};
        Array *r = tri(3, 3, 0, FLOAT64);
        ASSERT_NOTNULL(r, "tri k=0 non-null");
        ASSERT_NDIM(r, 2, "tri k=0 ndim=2");
        ASSERT_EQ_INT(r->shape[0], 3, "tri k=0 shape[0]=3");
        ASSERT_EQ_INT(r->shape[1], 3, "tri k=0 shape[1]=3");
        ASSERT_EQ_ARR((double*)r->data, expected, 9, TOL_F64, "tri k=0 values");
        free_a(r);
    }

    /* tri N=3 M=3 k=1 -> [[1,1,0],[1,1,1],[1,1,1]] */
    {
        double expected[] = {1, 1, 0, 1, 1, 1, 1, 1, 1};
        Array *r = tri(3, 3, 1, FLOAT64);
        ASSERT_NOTNULL(r, "tri k=1 non-null");
        ASSERT_EQ_ARR((double*)r->data, expected, 9, TOL_F64, "tri k=1 values");
        free_a(r);
    }

    /* tri N=3 M=3 k=-1 -> [[0,0,0],[1,0,0],[1,1,0]] */
    {
        double expected[] = {0, 0, 0, 1, 0, 0, 1, 1, 0};
        Array *r = tri(3, 3, -1, FLOAT64);
        ASSERT_NOTNULL(r, "tri k=-1 non-null");
        ASSERT_EQ_ARR((double*)r->data, expected, 9, TOL_F64, "tri k=-1 values");
        free_a(r);
    }

    /* tri INT32 N=2 M=3 k=0 */
    {
        int expected[] = {1, 0, 0, 1, 1, 0};
        Array *r = tri(2, 3, 0, INT32);
        ASSERT_NOTNULL(r, "tri INT32 non-null");
        ASSERT_NDIM(r, 2, "tri INT32 ndim=2");
        ASSERT_EQ_INT(r->shape[0], 2, "tri INT32 shape[0]=2");
        ASSERT_EQ_INT(r->shape[1], 3, "tri INT32 shape[1]=3");
        ASSERT_EQ_IARR((int*)r->data, expected, 6, "tri INT32 values");
        free_a(r);
    }

    /* ── tril ────────────────────────────────────────────────────── */
    TEST_SECTION("tril");

    /* tril [[1,2,3],[4,5,6],[7,8,9]] k=0 -> [[1,0,0],[4,5,0],[7,8,9]] */
    {
        double data[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
        double expected[] = {1, 0, 0, 4, 5, 0, 7, 8, 9};
        Array *a = make_f64_2d(data, 3, 3);
        Array *r = tril(a, 0);
        ASSERT_NOTNULL(r, "tril k=0 non-null");
        ASSERT_NDIM(r, 2, "tril k=0 ndim=2");
        ASSERT_EQ_ARR((double*)r->data, expected, 9, TOL_F64, "tril k=0 values");
        free_a(r);
        free_a(a);
    }

    /* tril k=1 -> [[1,2,0],[4,5,6],[7,8,9]] */
    {
        double data[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
        double expected[] = {1, 2, 0, 4, 5, 6, 7, 8, 9};
        Array *a = make_f64_2d(data, 3, 3);
        Array *r = tril(a, 1);
        ASSERT_NOTNULL(r, "tril k=1 non-null");
        ASSERT_EQ_ARR((double*)r->data, expected, 9, TOL_F64, "tril k=1 values");
        free_a(r);
        free_a(a);
    }

    /* tril INT32 k=0 — 2x2 [[10,20],[30,40]] -> [[10,0],[30,40]] */
    {
        int data[] = {10, 20, 30, 40};
        int expected[] = {10, 0, 30, 40};
        Array *a = make_i32_2d(data, 2, 2);
        Array *r = tril(a, 0);
        ASSERT_NOTNULL(r, "tril INT32 non-null");
        ASSERT_EQ_IARR((int*)r->data, expected, 4, "tril INT32 values");
        free_a(r);
        free_a(a);
    }

    /* ── triu ────────────────────────────────────────────────────── */
    TEST_SECTION("triu");

    /* triu [[1,2,3],[4,5,6],[7,8,9]] k=0 -> [[1,2,3],[0,5,6],[0,0,9]] */
    {
        double data[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
        double expected[] = {1, 2, 3, 0, 5, 6, 0, 0, 9};
        Array *a = make_f64_2d(data, 3, 3);
        Array *r = triu(a, 0);
        ASSERT_NOTNULL(r, "triu k=0 non-null");
        ASSERT_NDIM(r, 2, "triu k=0 ndim=2");
        ASSERT_EQ_ARR((double*)r->data, expected, 9, TOL_F64, "triu k=0 values");
        free_a(r);
        free_a(a);
    }

    /* triu k=-1 -> [[1,2,3],[4,5,6],[0,8,9]] */
    {
        double data[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
        double expected[] = {1, 2, 3, 4, 5, 6, 0, 8, 9};
        Array *a = make_f64_2d(data, 3, 3);
        Array *r = triu(a, -1);
        ASSERT_NOTNULL(r, "triu k=-1 non-null");
        ASSERT_EQ_ARR((double*)r->data, expected, 9, TOL_F64, "triu k=-1 values");
        free_a(r);
        free_a(a);
    }

    /* triu INT32 k=1 */
    {
        int data[] = {10, 20, 30, 40};
        int expected[] = {0, 20, 0, 0};
        Array *a = make_i32_2d(data, 2, 2);
        Array *r = triu(a, 1);
        ASSERT_NOTNULL(r, "triu INT32 k=1 non-null");
        ASSERT_EQ_IARR((int*)r->data, expected, 4, "triu INT32 k=1 values");
        free_a(r);
        free_a(a);
    }

    /* ── tril_indices ────────────────────────────────────────────── */
    TEST_SECTION("tril_indices");

    /* tril_indices n=3 k=0 m=3 */
    {
        Array *r = tril_indices(3, 0, 3);
        ASSERT_NOTNULL(r, "tril_indices non-null");
        /* Returns array of row and col indices */
        ASSERT_NDIM(r, 2, "tril_indices ndim=2");
        /* shape should be (2, n*(n+1)/2) = (2, 6) for k=0 */
        ASSERT_EQ_INT(r->shape[0], 2, "tril_indices shape[0]=2");
        ASSERT_EQ_INT(r->shape[1], 6, "tril_indices shape[1]=6");
        free_a(r);
    }

    /* tril_indices n=3 k=1 -> more indices */
    {
        Array *r = tril_indices(3, 1, 3);
        ASSERT_NOTNULL(r, "tril_indices k=1 non-null");
        ASSERT_NDIM(r, 2, "tril_indices k=1 ndim=2");
        free_a(r);
    }

    /* ── triu_indices ────────────────────────────────────────────── */
    TEST_SECTION("triu_indices");

    /* triu_indices n=3 k=0 m=3 */
    {
        Array *r = triu_indices(3, 0, 3);
        ASSERT_NOTNULL(r, "triu_indices non-null");
        ASSERT_NDIM(r, 2, "triu_indices ndim=2");
        ASSERT_EQ_INT(r->shape[0], 2, "triu_indices shape[0]=2");
        free_a(r);
    }

    /* ── tril_indices_from ───────────────────────────────────────── */
    TEST_SECTION("tril_indices_from");

    /* tril_indices_from 3x3 array */
    {
        double data[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
        Array *a = make_f64_2d(data, 3, 3);
        Array *r = tril_indices_from(a, 0);
        ASSERT_NOTNULL(r, "tril_indices_from k=0 non-null");
        ASSERT_NDIM(r, 2, "tril_indices_from ndim=2");
        free_a(r);
        free_a(a);
    }

    /* ── triu_indices_from ───────────────────────────────────────── */
    TEST_SECTION("triu_indices_from");

    /* triu_indices_from 3x3 array */
    {
        double data[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
        Array *a = make_f64_2d(data, 3, 3);
        Array *r = triu_indices_from(a, 0);
        ASSERT_NOTNULL(r, "triu_indices_from k=0 non-null");
        ASSERT_NDIM(r, 2, "triu_indices_from ndim=2");
        free_a(r);
        free_a(a);
    }

    /* triu_indices_from INT32 */
    {
        int data[] = {0, 0, 0, 0};
        Array *a = make_i32_2d(data, 2, 2);
        Array *r = triu_indices_from(a, 0);
        ASSERT_NOTNULL(r, "triu_indices_from INT32 non-null");
        free_a(r);
        free_a(a);
    }

    TEST_SUMMARY();
    return (g_fail > 0) ? 1 : 0;
}
