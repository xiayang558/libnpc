#include "test_utils.h"


int main(void) {
    TEST_MAIN("test_meshgrid");

    /* ── meshgrid xy ─────────────────────────────────────────────── */
    TEST_SECTION("meshgrid xy");

    /* meshgrid [1,2] and [3,4,5] indexing="xy"
       X shape (ny, nx) = (3,2): [[1,2],[1,2],[1,2]]
       Y shape (ny, nx) = (3,2): [[3,3],[4,4],[5,5]] */
    {
        double d1[] = {1, 2};     /* x */
        double d2[] = {3, 4, 5};  /* y */
        Array *a1 = make_f64_1d(d1, 2);
        Array *a2 = make_f64_1d(d2, 3);
        Array* inputs[] = {a1, a2};
        Array **out = NULL;
        int ret = meshgrid(inputs, 2, "xy", &out);
        ASSERT_EQ_INT(ret, 0, "meshgrid xy ret=0");
        ASSERT_NOTNULL(out, "meshgrid xy out non-null");

        /* X: (3,2) */
        double ex_x[] = {1, 2, 1, 2, 1, 2};
        ASSERT_NDIM(out[0], 2, "meshgrid xy X ndim=2");
        ASSERT_EQ_INT(out[0]->shape[0], 3, "meshgrid xy X shape[0]=3");
        ASSERT_EQ_INT(out[0]->shape[1], 2, "meshgrid xy X shape[1]=2");
        ASSERT_EQ_ARR((double*)out[0]->data, ex_x, 6, TOL_F64, "meshgrid xy X values");

        /* Y: (3,2) */
        double ex_y[] = {3, 3, 4, 4, 5, 5};
        ASSERT_NDIM(out[1], 2, "meshgrid xy Y ndim=2");
        ASSERT_EQ_INT(out[1]->shape[0], 3, "meshgrid xy Y shape[0]=3");
        ASSERT_EQ_INT(out[1]->shape[1], 2, "meshgrid xy Y shape[1]=2");
        ASSERT_EQ_ARR((double*)out[1]->data, ex_y, 6, TOL_F64, "meshgrid xy Y values");

        free_a(out[0]); free_a(out[1]); free(out);
        free_a(a1); free_a(a2);
    }

    /* ── meshgrid ij ─────────────────────────────────────────────── */
    TEST_SECTION("meshgrid ij");

    /* meshgrid [1,2] and [3,4,5] indexing="ij"
       X shape (nx, ny) = (2,3): [[1,1,1],[2,2,2]]
       Y shape (nx, ny) = (2,3): [[3,4,5],[3,4,5]] */
    {
        double d1[] = {1, 2};
        double d2[] = {3, 4, 5};
        Array *a1 = make_f64_1d(d1, 2);
        Array *a2 = make_f64_1d(d2, 3);
        Array* inputs[] = {a1, a2};
        Array **out = NULL;
        int ret = meshgrid(inputs, 2, "ij", &out);
        ASSERT_EQ_INT(ret, 0, "meshgrid ij ret=0");
        ASSERT_NOTNULL(out, "meshgrid ij out non-null");

        /* X: (2,3) */
        double ex_x[] = {1, 1, 1, 2, 2, 2};
        ASSERT_NDIM(out[0], 2, "meshgrid ij X ndim=2");
        ASSERT_EQ_INT(out[0]->shape[0], 2, "meshgrid ij X shape[0]=2");
        ASSERT_EQ_INT(out[0]->shape[1], 3, "meshgrid ij X shape[1]=3");
        ASSERT_EQ_ARR((double*)out[0]->data, ex_x, 6, TOL_F64, "meshgrid ij X values");

        /* Y: (2,3) */
        double ex_y[] = {3, 4, 5, 3, 4, 5};
        ASSERT_NDIM(out[1], 2, "meshgrid ij Y ndim=2");
        ASSERT_EQ_INT(out[1]->shape[0], 2, "meshgrid ij Y shape[0]=2");
        ASSERT_EQ_INT(out[1]->shape[1], 3, "meshgrid ij Y shape[1]=3");
        ASSERT_EQ_ARR((double*)out[1]->data, ex_y, 6, TOL_F64, "meshgrid ij Y values");

        free_a(out[0]); free_a(out[1]); free(out);
        free_a(a1); free_a(a2);
    }

    /* ── meshgrid INT32 ──────────────────────────────────────────── */
    TEST_SECTION("meshgrid INT32");

    /* meshgrid INT32 indexing="xy" */
    {
        int d1[] = {10, 20};
        int d2[] = {30, 40, 50};
        Array *a1 = make_i32_1d(d1, 2);
        Array *a2 = make_i32_1d(d2, 3);
        Array* inputs[] = {a1, a2};
        Array **out = NULL;
        int ret = meshgrid(inputs, 2, "xy", &out);
        ASSERT_EQ_INT(ret, 0, "meshgrid INT32 xy ret=0");
        ASSERT_NOTNULL(out, "meshgrid INT32 xy out non-null");

        int ex_x[] = {10, 20, 10, 20, 10, 20};
        ASSERT_EQ_IARR((int*)out[0]->data, ex_x, 6, "meshgrid INT32 X values");

        int ex_y[] = {30, 30, 40, 40, 50, 50};
        ASSERT_EQ_IARR((int*)out[1]->data, ex_y, 6, "meshgrid INT32 Y values");

        free_a(out[0]); free_a(out[1]); free(out);
        free_a(a1); free_a(a2);
    }

    /* ── meshgrid single array ───────────────────────────────────── */
    TEST_SECTION("meshgrid single");

    /* meshgrid with single array indexing="xy" -> X = Y = input */
    {
        double d1[] = {1, 2, 3};
        Array *a1 = make_f64_1d(d1, 3);
        Array* inputs[] = {a1};
        Array **out = NULL;
        int ret = meshgrid(inputs, 1, "xy", &out);
        ASSERT_EQ_INT(ret, 0, "meshgrid single ret=0");
        ASSERT_NOTNULL(out, "meshgrid single out non-null");
        ASSERT_NDIM(out[0], 1, "meshgrid single ndim=1");
        ASSERT_EQ_ARR((double*)out[0]->data, d1, 3, TOL_F64, "meshgrid single values");
        free_a(out[0]); free(out);
        free_a(a1);
    }

    TEST_SUMMARY();
    return (g_fail > 0) ? 1 : 0;
}
