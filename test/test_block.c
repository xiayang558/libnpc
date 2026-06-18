#include "test_utils.h"


int main(void) {
    TEST_MAIN("test_block");

    /* ── block simple ────────────────────────────────────────────── */
    TEST_SECTION("block simple");

    /* block 2x2 of scalars -> 2x2 matrix
       [[1, 2],
        [3, 4]] */
    {
        int d1[] = {1}; Array *s1 = create_array((int[]){1, 1}, 2, INT32);
        memcpy(s1->data, d1, sizeof(int));
        int d2[] = {2}; Array *s2 = create_array((int[]){1, 1}, 2, INT32);
        memcpy(s2->data, d2, sizeof(int));
        int d3[] = {3}; Array *s3 = create_array((int[]){1, 1}, 2, INT32);
        memcpy(s3->data, d3, sizeof(int));
        int d4[] = {4}; Array *s4 = create_array((int[]){1, 1}, 2, INT32);
        memcpy(s4->data, d4, sizeof(int));

        Array *blocks[] = {s1, s2, s3, s4};
        Array *r = block(blocks, 2, 2);
        ASSERT_NOTNULL(r, "block 2x2 non-null");
        ASSERT_NDIM(r, 2, "block 2x2 ndim=2");
        ASSERT_EQ_INT(r->shape[0], 2, "block 2x2 shape[0]=2");
        ASSERT_EQ_INT(r->shape[1], 2, "block 2x2 shape[1]=2");

        int expected[] = {1, 2, 3, 4};
        ASSERT_EQ_IARR((int*)r->data, expected, 4, "block 2x2 values");

        free_a(r);
        free_a(s1); free_a(s2); free_a(s3); free_a(s4);
    }

    /* block single element: block requires 2D inputs; 1D returns NULL */
    {
        double data[] = {42};
        Array *a = make_f64_1d(data, 1);
        Array *blocks[] = {a};
        Array *r = block(blocks, 1, 1);
        ASSERT_NULL(r, "block single returns NULL (1D input)");
        free_a(a);
    }

    /* block: single-row or single-col grids are not supported (library limitation) */

    /* block FLOAT64 — only 2x2 grid supported */
    {
        double d1[] = {1.5, 2.5};
        double d2[] = {3.5, 4.5};
        double d3[] = {5.5, 6.5};
        double d4[] = {7.5, 8.5};
        Array *a1 = create_array((int[]){1, 2}, 2, FLOAT64);
        memcpy(a1->data, d1, 2 * sizeof(double));
        Array *a2 = create_array((int[]){1, 2}, 2, FLOAT64);
        memcpy(a2->data, d2, 2 * sizeof(double));
        Array *a3 = create_array((int[]){1, 2}, 2, FLOAT64);
        memcpy(a3->data, d3, 2 * sizeof(double));
        Array *a4 = create_array((int[]){1, 2}, 2, FLOAT64);
        memcpy(a4->data, d4, 2 * sizeof(double));
        Array *blocks[] = {a1, a2, a3, a4};
        Array *r = block(blocks, 2, 2);
        ASSERT_NOTNULL(r, "block FLOAT64 non-null");
        ASSERT_NDIM(r, 2, "block FLOAT64 ndim=2");

        double expected[] = {1.5, 2.5, 3.5, 4.5, 5.5, 6.5, 7.5, 8.5};
        ASSERT_EQ_ARR((double*)r->data, expected, 8, TOL_F64, "block FLOAT64 values");

        free_a(r);
        free_a(a1); free_a(a2);
    }

    TEST_SUMMARY();
    return (g_fail > 0) ? 1 : 0;
}
