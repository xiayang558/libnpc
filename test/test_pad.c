#include "test_utils.h"


int main(void) {
    TEST_MAIN("test_pad");

    /* ── pad constant ────────────────────────────────────────────── */
    TEST_SECTION("pad constant");

    /* pad [1,2,3] pad_width=[1,1] constant=0 -> [0,1,2,3,0] */
    {
        double data[] = {1, 2, 3};
        double expected[] = {0, 1, 2, 3, 0};
        Array *a = make_f64_1d(data, 3);
        /* pad_width is INT64: [before_dim0, after_dim0] */
        int64_t pw_data[] = {1, 1};
        Array *pw = create_array((int[]){2}, 1, INT64);
        memcpy(pw->data, pw_data, 2 * sizeof(int64_t));
        Array *r = pad(a, pw, "constant", 0.0);
        ASSERT_NOTNULL(r, "pad constant non-null");
        ASSERT_SIZE(r, 5, "pad constant size=5");
        ASSERT_NDIM(r, 1, "pad constant ndim=1");
        ASSERT_EQ_ARR((double*)r->data, expected, 5, TOL_F64, "pad constant values");
        free_a(r); free_a(pw);
        free_a(a);
    }

    /* pad [1,2,3] pad_width=[2,2] constant=-1 -> [-1,-1,1,2,3,-1,-1] */
    {
        double data[] = {1, 2, 3};
        double expected[] = {-1, -1, 1, 2, 3, -1, -1};
        Array *a = make_f64_1d(data, 3);
        int64_t pw_data[] = {2, 2};
        Array *pw = create_array((int[]){2}, 1, INT64);
        memcpy(pw->data, pw_data, 2 * sizeof(int64_t));
        Array *r = pad(a, pw, "constant", -1.0);
        ASSERT_NOTNULL(r, "pad constant=-1 non-null");
        ASSERT_SIZE(r, 7, "pad constant=-1 size=7");
        ASSERT_EQ_ARR((double*)r->data, expected, 7, TOL_F64, "pad constant=-1 values");
        free_a(r); free_a(pw);
        free_a(a);
    }

    /* pad INT32 constant */
    {
        int data[] = {10, 20};
        int expected[] = {0, 10, 20, 0};
        Array *a = make_i32_1d(data, 2);
        int64_t pw_data[] = {1, 1};
        Array *pw = create_array((int[]){2}, 1, INT64);
        memcpy(pw->data, pw_data, 2 * sizeof(int64_t));
        Array *r = pad(a, pw, "constant", 0.0);
        ASSERT_NOTNULL(r, "pad INT32 constant non-null");
        ASSERT_SIZE(r, 4, "pad INT32 size=4");
        ASSERT_EQ_IARR((int*)r->data, expected, 4, "pad INT32 values");
        free_a(r); free_a(pw);
        free_a(a);
    }

    /* ── pad edge ────────────────────────────────────────────────── */
    TEST_SECTION("pad edge");

    /* pad edge [1,2,3] pad_width=[1,1] -> [1,1,2,3,3] */
    {
        double data[] = {1, 2, 3};
        double expected[] = {1, 1, 2, 3, 3};
        Array *a = make_f64_1d(data, 3);
        int64_t pw_data[] = {1, 1};
        Array *pw = create_array((int[]){2}, 1, INT64);
        memcpy(pw->data, pw_data, 2 * sizeof(int64_t));
        Array *r = pad(a, pw, "edge", 0.0);
        ASSERT_NOTNULL(r, "pad edge non-null");
        ASSERT_SIZE(r, 5, "pad edge size=5");
        ASSERT_EQ_ARR((double*)r->data, expected, 5, TOL_F64, "pad edge values");
        free_a(r); free_a(pw);
        free_a(a);
    }

    /* pad edge INT32 */
    {
        int data[] = {100, 200};
        int expected[] = {100, 100, 200, 200};
        Array *a = make_i32_1d(data, 2);
        int64_t pw_data[] = {1, 1};
        Array *pw = create_array((int[]){2}, 1, INT64);
        memcpy(pw->data, pw_data, 2 * sizeof(int64_t));
        Array *r = pad(a, pw, "edge", 0.0);
        ASSERT_NOTNULL(r, "pad edge INT32 non-null");
        ASSERT_EQ_IARR((int*)r->data, expected, 4, "pad edge INT32 values");
        free_a(r); free_a(pw);
        free_a(a);
    }

    /* ── pad 2D constant ─────────────────────────────────────────── */
    TEST_SECTION("pad 2D constant");

    /* pad 2D [[1,2],[3,4]] pad_width=[1,1,1,1] constant=0 -> 4x4 */
    {
        double data[] = {1, 2, 3, 4};
        double expected[] = {
            0, 0, 0, 0,
            0, 1, 2, 0,
            0, 3, 4, 0,
            0, 0, 0, 0
        };
        Array *a = make_f64_2d(data, 2, 2);
        int64_t pw_data[] = {1, 1, 1, 1}; /* [before0,after0, before1,after1] */
        Array *pw = create_array((int[]){4}, 1, INT64);
        memcpy(pw->data, pw_data, 4 * sizeof(int64_t));
        Array *r = pad(a, pw, "constant", 0.0);
        ASSERT_NOTNULL(r, "pad 2D constant non-null");
        ASSERT_NDIM(r, 2, "pad 2D ndim=2");
        ASSERT_EQ_INT(r->shape[0], 4, "pad 2D shape[0]=4");
        ASSERT_EQ_INT(r->shape[1], 4, "pad 2D shape[1]=4");
        ASSERT_EQ_ARR((double*)r->data, expected, 16, TOL_F64, "pad 2D values");
        free_a(r); free_a(pw);
        free_a(a);
    }

    /* ── pad reflect ─────────────────────────────────────────────── */
    TEST_SECTION("pad reflect");

    /* pad reflect mode */
    {
        double data[] = {1, 2, 3, 4};
        Array *a = make_f64_1d(data, 4);
        int64_t pw_data[] = {2, 2};
        Array *pw = create_array((int[]){2}, 1, INT64);
        memcpy(pw->data, pw_data, 2 * sizeof(int64_t));
        Array *r = pad(a, pw, "reflect", 0.0);
        ASSERT_NOTNULL(r, "pad reflect non-null");
        free_a(r); free_a(pw);
        free_a(a);
    }

    /* ── pad symmetric ───────────────────────────────────────────── */
    TEST_SECTION("pad symmetric");

    /* pad symmetric mode */
    {
        double data[] = {1, 2, 3, 4};
        Array *a = make_f64_1d(data, 4);
        int64_t pw_data[] = {2, 2};
        Array *pw = create_array((int[]){2}, 1, INT64);
        memcpy(pw->data, pw_data, 2 * sizeof(int64_t));
        Array *r = pad(a, pw, "symmetric", 0.0);
        ASSERT_NOTNULL(r, "pad symmetric non-null");
        free_a(r); free_a(pw);
        free_a(a);
    }

    /* ── pad zero pad_width ──────────────────────────────────────── */
    TEST_SECTION("pad zero width");

    /* pad with zero-width padding returns original data */
    {
        double data[] = {1, 2, 3};
        Array *a = make_f64_1d(data, 3);
        int64_t pw_data[] = {0, 0};
        Array *pw = create_array((int[]){2}, 1, INT64);
        memcpy(pw->data, pw_data, 2 * sizeof(int64_t));
        Array *r = pad(a, pw, "constant", 0.0);
        ASSERT_NOTNULL(r, "pad zero width non-null");
        ASSERT_SIZE(r, 3, "pad zero width size=3");
        free_a(r); free_a(pw);
        free_a(a);
    }

    TEST_SUMMARY();
    return (g_fail > 0) ? 1 : 0;
}
