#include "test_utils.h"


int main(void) {
    TEST_MAIN("test_eye_vander");

    /* ── eye ─────────────────────────────────────────────────────── */
    TEST_SECTION("eye");

    /* eye N=3 M=3 k=0 -> [[1,0,0],[0,1,0],[0,0,1]] */
    {
        double expected[] = {1, 0, 0, 0, 1, 0, 0, 0, 1};
        Array *r = eye(3, 3, 0, FLOAT64);
        ASSERT_NOTNULL(r, "eye(3,3,0) non-null");
        ASSERT_NDIM(r, 2, "eye ndim=2");
        ASSERT_EQ_INT(r->shape[0], 3, "eye shape[0]=3");
        ASSERT_EQ_INT(r->shape[1], 3, "eye shape[1]=3");
        ASSERT_EQ_ARR((double*)r->data, expected, 9, TOL_F64, "eye values");
        free_a(r);
    }

    /* eye N=3 M=4 k=0 -> [[1,0,0,0],[0,1,0,0],[0,0,1,0]] */
    {
        double expected[] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0};
        Array *r = eye(3, 4, 0, FLOAT64);
        ASSERT_NOTNULL(r, "eye(3,4,0) non-null");
        ASSERT_NDIM(r, 2, "eye(3,4) ndim=2");
        ASSERT_EQ_INT(r->shape[0], 3, "eye(3,4) shape[0]=3");
        ASSERT_EQ_INT(r->shape[1], 4, "eye(3,4) shape[1]=4");
        ASSERT_EQ_ARR((double*)r->data, expected, 12, TOL_F64, "eye(3,4) values");
        free_a(r);
    }

    /* eye N=3 M=3 k=1 -> [[0,1,0],[0,0,1],[0,0,0]] */
    {
        double expected[] = {0, 1, 0, 0, 0, 1, 0, 0, 0};
        Array *r = eye(3, 3, 1, FLOAT64);
        ASSERT_NOTNULL(r, "eye(3,3,1) non-null");
        ASSERT_EQ_ARR((double*)r->data, expected, 9, TOL_F64, "eye(3,3,1) values");
        free_a(r);
    }

    /* eye N=3 M=3 k=-1 -> [[0,0,0],[1,0,0],[0,1,0]] */
    {
        double expected[] = {0, 0, 0, 1, 0, 0, 0, 1, 0};
        Array *r = eye(3, 3, -1, FLOAT64);
        ASSERT_NOTNULL(r, "eye(3,3,-1) non-null");
        ASSERT_EQ_ARR((double*)r->data, expected, 9, TOL_F64, "eye(3,3,-1) values");
        free_a(r);
    }

    /* eye INT32 */
    {
        int expected[] = {1, 0, 0, 1};
        Array *r = eye(2, 2, 0, INT32);
        ASSERT_NOTNULL(r, "eye INT32 non-null");
        ASSERT_EQ_IARR((int*)r->data, expected, 4, "eye INT32 values");
        free_a(r);
    }

    /* eye N=1 M=1 k=0 */
    {
        double expected[] = {1};
        Array *r = eye(1, 1, 0, FLOAT64);
        ASSERT_NOTNULL(r, "eye(1,1) non-null");
        ASSERT_SIZE(r, 1, "eye(1,1) size=1");
        ASSERT_EQ_ARR((double*)r->data, expected, 1, TOL_F64, "eye(1,1) value");
        free_a(r);
    }

    /* ── identity ────────────────────────────────────────────────── */
    TEST_SECTION("identity");

    /* identity n=3 -> same as eye(3,3,0) */
    {
        double expected[] = {1, 0, 0, 0, 1, 0, 0, 0, 1};
        Array *r = identity(3, FLOAT64);
        ASSERT_NOTNULL(r, "identity(3) non-null");
        ASSERT_NDIM(r, 2, "identity ndim=2");
        ASSERT_EQ_INT(r->shape[0], 3, "identity shape[0]=3");
        ASSERT_EQ_INT(r->shape[1], 3, "identity shape[1]=3");
        ASSERT_EQ_ARR((double*)r->data, expected, 9, TOL_F64, "identity values");
        free_a(r);
    }

    /* identity INT32 */
    {
        int expected[] = {1, 0, 0, 1};
        Array *r = identity(2, INT32);
        ASSERT_NOTNULL(r, "identity INT32 non-null");
        ASSERT_EQ_IARR((int*)r->data, expected, 4, "identity INT32 values");
        free_a(r);
    }

    /* identity n=1 */
    {
        double expected[] = {1};
        Array *r = identity(1, FLOAT64);
        ASSERT_NOTNULL(r, "identity(1) non-null");
        ASSERT_EQ_ARR((double*)r->data, expected, 1, TOL_F64, "identity(1) value");
        free_a(r);
    }

    /* ── vander ──────────────────────────────────────────────────── */
    TEST_SECTION("vander");

    /* vander [1,2,3] N=3 increasing=0 -> [[1,1,1],[4,2,1],[9,3,1]] */
    {
        double data[] = {1, 2, 3};
        double expected[] = {1, 1, 1, 4, 2, 1, 9, 3, 1};
        Array *x = make_f64_1d(data, 3);
        Array *r = vander(x, 3, 0);
        ASSERT_NOTNULL(r, "vander increasing=0 non-null");
        ASSERT_NDIM(r, 2, "vander ndim=2");
        ASSERT_EQ_INT(r->shape[0], 3, "vander shape[0]=3");
        ASSERT_EQ_INT(r->shape[1], 3, "vander shape[1]=3");
        ASSERT_EQ_ARR((double*)r->data, expected, 9, TOL_F64, "vander increasing=0 values");
        free_a(r);
        free_a(x);
    }

    /* vander [1,2,3] N=3 increasing=1 -> [[1,1,1],[1,2,4],[1,3,9]] */
    {
        double data[] = {1, 2, 3};
        double expected[] = {1, 1, 1, 1, 2, 4, 1, 3, 9};
        Array *x = make_f64_1d(data, 3);
        Array *r = vander(x, 3, 1);
        ASSERT_NOTNULL(r, "vander increasing=1 non-null");
        ASSERT_EQ_ARR((double*)r->data, expected, 9, TOL_F64, "vander increasing=1 values");
        free_a(r);
        free_a(x);
    }

    /* vander with different N */
    {
        double data[] = {1, 2};
        double expected[] = {1, 1, 2, 1};
        /* N=2: columns are x^1, x^0 */
        Array *x = make_f64_1d(data, 2);
        Array *r = vander(x, 2, 0);
        ASSERT_NOTNULL(r, "vander N=2 non-null");
        ASSERT_NDIM(r, 2, "vander N=2 ndim=2");
        ASSERT_EQ_INT(r->shape[0], 2, "vander N=2 shape[0]");
        ASSERT_EQ_INT(r->shape[1], 2, "vander N=2 shape[1]");
        ASSERT_EQ_ARR((double*)r->data, expected, 4, TOL_F64, "vander N=2 values");
        free_a(r);
        free_a(x);
    }

    /* vander INT32 input -> FLOAT64 output */
    {
        int data[] = {1, 2, 3};
        Array *x = make_i32_1d(data, 3);
        Array *r = vander(x, 3, 0);
        ASSERT_NOTNULL(r, "vander INT32 input non-null");
        ASSERT_DTYPE(r, FLOAT64, "vander output dtype FLOAT64");
        free_a(r);
        free_a(x);
    }

    /* vander N=4 (more cols than rows) increasing=1 */
    {
        double data[] = {1, 2};
        /* increasing=1, N=4: x^0, x^1, x^2, x^3 */
        /* [1] -> [1,1,1,1], [2] -> [1,2,4,8] */
        double expected[] = {1, 1, 1, 1, 1, 2, 4, 8};
        Array *x = make_f64_1d(data, 2);
        Array *r = vander(x, 4, 1);
        ASSERT_NOTNULL(r, "vander N=4 increasing=1 non-null");
        ASSERT_EQ_INT(r->shape[0], 2, "vander N=4 shape[0]=2");
        ASSERT_EQ_INT(r->shape[1], 4, "vander N=4 shape[1]=4");
        ASSERT_EQ_ARR((double*)r->data, expected, 8, TOL_F64, "vander N=4 values");
        free_a(r);
        free_a(x);
    }

    TEST_SUMMARY();
    return (g_fail > 0) ? 1 : 0;
}
