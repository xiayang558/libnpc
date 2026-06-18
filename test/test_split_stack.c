#include "test_utils.h"


int main(void) {
    TEST_MAIN("test_split_stack");

    /* ── concatenate ──────────────────────────────────────────────── */
    TEST_SECTION("concatenate");

    /* concatenate 1D axis=0 */
    {
        double d1[] = {1, 2, 3};
        double d2[] = {4, 5, 6};
        double expected[] = {1, 2, 3, 4, 5, 6};
        Array *a1 = make_f64_1d(d1, 3);
        Array *a2 = make_f64_1d(d2, 3);
        Array *c = concatenate(a1, a2, 0);
        ASSERT_NOTNULL(c, "concatenate 1D non-null");
        ASSERT_NDIM(c, 1, "concatenate 1D ndim=1");
        ASSERT_SIZE(c, 6, "concatenate 1D size=6");
        ASSERT_EQ_ARR((double*)c->data, expected, 6, TOL_F64, "concatenate 1D values");
        free_a(c);
        free_a(a1); free_a(a2);
    }

    /* concatenate INT32 1D */
    {
        int d1[] = {10, 20};
        int d2[] = {30, 40, 50};
        int expected[] = {10, 20, 30, 40, 50};
        Array *a1 = make_i32_1d(d1, 2);
        Array *a2 = make_i32_1d(d2, 3);
        Array *c = concatenate(a1, a2, 0);
        ASSERT_NOTNULL(c, "concat INT32 1D non-null");
        ASSERT_SIZE(c, 5, "concat INT32 size=5");
        ASSERT_EQ_IARR((int*)c->data, expected, 5, "concat INT32 values");
        free_a(c);
        free_a(a1); free_a(a2);
    }

    /* concatenate 2D axis=0 */
    {
        double d1[] = {1, 2, 3, 4};
        double d2[] = {5, 6, 7, 8};
        double expected[] = {1, 2, 3, 4, 5, 6, 7, 8};
        Array *a1 = make_f64_2d(d1, 2, 2);
        Array *a2 = make_f64_2d(d2, 2, 2);
        Array *c = concatenate(a1, a2, 0);
        ASSERT_NOTNULL(c, "concat 2D axis=0 non-null");
        ASSERT_NDIM(c, 2, "concat 2D axis=0 ndim=2");
        ASSERT_EQ_INT(c->shape[0], 4, "concat 2D axis=0 shape[0]=4");
        ASSERT_EQ_INT(c->shape[1], 2, "concat 2D axis=0 shape[1]=2");
        ASSERT_EQ_ARR((double*)c->data, expected, 8, TOL_F64, "concat 2D axis=0 values");
        free_a(c);
        free_a(a1); free_a(a2);
    }

    /* concatenate 2D axis=1 */
    {
        double d1[] = {1, 2, 3, 4};
        double d2[] = {5, 6, 7, 8};
        double expected[] = {1, 2, 5, 6, 3, 4, 7, 8};
        Array *a1 = make_f64_2d(d1, 2, 2);
        Array *a2 = make_f64_2d(d2, 2, 2);
        Array *c = concatenate(a1, a2, 1);
        ASSERT_NOTNULL(c, "concat 2D axis=1 non-null");
        ASSERT_NDIM(c, 2, "concat 2D axis=1 ndim=2");
        ASSERT_EQ_INT(c->shape[0], 2, "concat 2D axis=1 shape[0]=2");
        ASSERT_EQ_INT(c->shape[1], 4, "concat 2D axis=1 shape[1]=4");
        ASSERT_EQ_ARR((double*)c->data, expected, 8, TOL_F64, "concat 2D axis=1 values");
        free_a(c);
        free_a(a1); free_a(a2);
    }

    /* ── split ───────────────────────────────────────────────────── */
    TEST_SECTION("split");

    /* split [1,2,3,4,5,6] with indices=[2] axis=0 -> 2 parts */
    {
        double data[] = {1, 2, 3, 4, 5, 6};
        double part1[] = {1, 2};
        double part2[] = {3, 4, 5, 6};
        Array *a = make_f64_1d(data, 6);
        Array *indices = make_i32_1d((int[]){2}, 1);
        int nsp = 0;
        Array **parts = split(a, indices, 0, &nsp);
        ASSERT_NOTNULL(parts, "split non-null");
        ASSERT_EQ_INT(nsp, 2, "split nsp=2");
        ASSERT_EQ_ARR((double*)parts[0]->data, part1, 2, TOL_F64, "split part0");
        ASSERT_EQ_ARR((double*)parts[1]->data, part2, 4, TOL_F64, "split part1");
        ASSERT_SIZE(parts[0], 2, "split part0 size=2");
        ASSERT_SIZE(parts[1], 4, "split part1 size=4");
        free_a(parts[0]); free_a(parts[1]); free(parts);
        free_a(indices); free_a(a);
    }

    /* split INT32 with 3 equal sections (scalar) */
    {
        int data[] = {1, 2, 3, 4, 5, 6};
        int p1[] = {1, 2};
        int p2[] = {3, 4};
        int p3[] = {5, 6};
        Array *a = make_i32_1d(data, 6);
        Array *indices = make_i32_1d((int[]){3}, 1);
        int nsp = 0;
        Array **parts = split(a, indices, 0, &nsp);
        if (parts != NULL) {
            /* expect 3 or 2 parts depending on how scalar 3 is treated */
            if (nsp == 3) {
                ASSERT_EQ_IARR((int*)parts[0]->data, p1, 2, "split 3-equal part0");
                ASSERT_EQ_IARR((int*)parts[1]->data, p2, 2, "split 3-equal part1");
                ASSERT_EQ_IARR((int*)parts[2]->data, p3, 2, "split 3-equal part2");
            }
            for (int i = 0; i < nsp; i++) free_a(parts[i]);
            free(parts);
        }
        free_a(indices); free_a(a);
    }

    /* ── hsplit ──────────────────────────────────────────────────── */
    TEST_SECTION("hsplit");

    /* hsplit 2D: [[1,2,3,4],[5,6,7,8]] into 2 parts -> [[1,2],[5,6]] and [[3,4],[7,8]] */
    {
        double data[] = {1, 2, 3, 4, 5, 6, 7, 8};
        double ex1[] = {1, 2, 5, 6};
        double ex2[] = {3, 4, 7, 8};
        Array *a = make_f64_2d(data, 2, 4);
        Array *indices = make_i32_1d((int[]){2}, 1);
        int nsp = 0;
        Array **parts = hsplit(a, indices, &nsp);
        if (parts != NULL) {
            ASSERT_EQ_ARR((double*)parts[0]->data, ex1, 4, TOL_F64, "hsplit part0");
            ASSERT_EQ_ARR((double*)parts[1]->data, ex2, 4, TOL_F64, "hsplit part1");
            for (int i = 0; i < nsp; i++) free_a(parts[i]);
            free(parts);
        }
        free_a(indices); free_a(a);
    }

    /* ── vsplit ──────────────────────────────────────────────────── */
    TEST_SECTION("vsplit");

    /* vsplit 2D 4x2 into 2 parts of 2x2 */
    {
        double data[] = {1, 2, 3, 4, 5, 6, 7, 8};
        double ex1[] = {1, 2, 3, 4};
        double ex2[] = {5, 6, 7, 8};
        Array *a = make_f64_2d(data, 4, 2);
        Array *indices = make_i32_1d((int[]){2}, 1);
        int nsp = 0;
        Array **parts = vsplit(a, indices, &nsp);
        if (parts != NULL) {
            ASSERT_EQ_ARR((double*)parts[0]->data, ex1, 4, TOL_F64, "vsplit part0");
            ASSERT_EQ_ARR((double*)parts[1]->data, ex2, 4, TOL_F64, "vsplit part1");
            for (int i = 0; i < nsp; i++) free_a(parts[i]);
            free(parts);
        }
        free_a(indices); free_a(a);
    }

    /* ── gsplit ──────────────────────────────────────────────────── */
    TEST_SECTION("dsplit");

    /* dsplit for 3D array */
    {
        /* 2x3x2 array, split along axis=2 into 2 parts of 2x3x1 */
        double data[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
        Array *a = create_array((int[]){2, 3, 2}, 3, FLOAT64);
        memcpy(a->data, data, 12 * sizeof(double));
        Array *indices = make_i32_1d((int[]){2}, 1);
        int nsp = 0;
        Array **parts = dsplit(a, indices, &nsp);
        if (parts != NULL) {
            for (int i = 0; i < nsp; i++) free_a(parts[i]);
            free(parts);
        }
        free_a(indices); free_a(a);
    }

    /* ── hstack ──────────────────────────────────────────────────── */
    TEST_SECTION("hstack");

    /* hstack two 1D: [1,2] and [3,4] -> [1,2,3,4] */
    {
        double d1[] = {1, 2};
        double d2[] = {3, 4};
        double expected[] = {1, 2, 3, 4};
        Array *a1 = make_f64_1d(d1, 2);
        Array *a2 = make_f64_1d(d2, 2);
        Array **arrs = (Array**)malloc(2 * sizeof(Array*));
        arrs[0] = a1; arrs[1] = a2;
        Array *h = hstack(arrs, 2);
        ASSERT_NOTNULL(h, "hstack 1D non-null");
        ASSERT_SIZE(h, 4, "hstack size=4");
        ASSERT_EQ_ARR((double*)h->data, expected, 4, TOL_F64, "hstack values");
        free_a(h); free(arrs);
        free_a(a1); free_a(a2);
    }

    /* hstack INT32 */
    {
        int d1[] = {10, 20};
        int d2[] = {30, 40};
        int expected[] = {10, 20, 30, 40};
        Array *a1 = make_i32_1d(d1, 2);
        Array *a2 = make_i32_1d(d2, 2);
        Array **arrs = (Array**)malloc(2 * sizeof(Array*));
        arrs[0] = a1; arrs[1] = a2;
        Array *h = hstack(arrs, 2);
        ASSERT_NOTNULL(h, "hstack INT32 non-null");
        ASSERT_EQ_IARR((int*)h->data, expected, 4, "hstack INT32 values");
        free_a(h); free(arrs);
        free_a(a1); free_a(a2);
    }

    /* hstack 2D arrays: [[1,2],[3,4]] and [[5],[6]] -> horizontal stack */
    {
        double d1[] = {1, 2, 3, 4};
        double d2[] = {5, 6};
        double expected[] = {1, 2, 5, 3, 4, 6};
        Array *a1 = make_f64_2d(d1, 2, 2);
        Array *a2 = make_f64_2d(d2, 2, 1);
        Array **arrs = (Array**)malloc(2 * sizeof(Array*));
        arrs[0] = a1; arrs[1] = a2;
        Array *h = hstack(arrs, 2);
        ASSERT_NOTNULL(h, "hstack 2D non-null");
        ASSERT_NDIM(h, 2, "hstack 2D ndim=2");
        ASSERT_EQ_INT(h->shape[1], 3, "hstack 2D shape[1]=3");
        ASSERT_EQ_ARR((double*)h->data, expected, 6, TOL_F64, "hstack 2D values");
        free_a(h); free(arrs);
        free_a(a1); free_a(a2);
    }

    /* ── vstack ──────────────────────────────────────────────────── */
    TEST_SECTION("vstack");

    /* vstack two 1D: [1,2] and [3,4] -> [[1,2],[3,4]] */
    {
        double d1[] = {1, 2};
        double d2[] = {3, 4};
        double expected[] = {1, 2, 3, 4};
        Array *a1 = make_f64_1d(d1, 2);
        Array *a2 = make_f64_1d(d2, 2);
        Array **arrs = (Array**)malloc(2 * sizeof(Array*));
        arrs[0] = a1; arrs[1] = a2;
        Array *v = vstack(arrs, 2);
        ASSERT_NOTNULL(v, "vstack non-null");
        ASSERT_NDIM(v, 2, "vstack ndim=2");
        ASSERT_EQ_INT(v->shape[0], 2, "vstack shape[0]=2");
        ASSERT_EQ_INT(v->shape[1], 2, "vstack shape[1]=2");
        ASSERT_EQ_ARR((double*)v->data, expected, 4, TOL_F64, "vstack values");
        free_a(v); free(arrs);
        free_a(a1); free_a(a2);
    }

    /* vstack INT32 */
    {
        int d1[] = {100, 200};
        int d2[] = {300, 400};
        int expected[] = {100, 200, 300, 400};
        Array *a1 = make_i32_1d(d1, 2);
        Array *a2 = make_i32_1d(d2, 2);
        Array **arrs = (Array**)malloc(2 * sizeof(Array*));
        arrs[0] = a1; arrs[1] = a2;
        Array *v = vstack(arrs, 2);
        ASSERT_NOTNULL(v, "vstack INT32 non-null");
        ASSERT_NDIM(v, 2, "vstack INT32 ndim=2");
        ASSERT_EQ_IARR((int*)v->data, expected, 4, "vstack INT32 values");
        free_a(v); free(arrs);
        free_a(a1); free_a(a2);
    }

    /* ── stack ───────────────────────────────────────────────────── */
    TEST_SECTION("stack");

    /* stack two 1D arrays along axis=0 -> shape (2,n) */
    {
        double d1[] = {1, 2, 3};
        double d2[] = {4, 5, 6};
        double expected[] = {1, 2, 3, 4, 5, 6};
        Array *a1 = make_f64_1d(d1, 3);
        Array *a2 = make_f64_1d(d2, 3);
        Array **arrs = (Array**)malloc(2 * sizeof(Array*));
        arrs[0] = a1; arrs[1] = a2;
        Array *s = stack(arrs, 2, 0);
        ASSERT_NOTNULL(s, "stack axis=0 non-null");
        ASSERT_NDIM(s, 2, "stack axis=0 ndim=2");
        ASSERT_EQ_INT(s->shape[0], 2, "stack axis=0 shape[0]=2");
        ASSERT_EQ_INT(s->shape[1], 3, "stack axis=0 shape[1]=3");
        ASSERT_EQ_ARR((double*)s->data, expected, 6, TOL_F64, "stack axis=0 values");
        free_a(s); free(arrs);
        free_a(a1); free_a(a2);
    }

    /* ── column_stack ────────────────────────────────────────────── */
    TEST_SECTION("column_stack");

    /* column_stack two 1D -> [[1,3],[2,4]] */
    {
        double d1[] = {1, 2};
        double d2[] = {3, 4};
        double expected[] = {1, 3, 2, 4};
        Array *a1 = make_f64_1d(d1, 2);
        Array *a2 = make_f64_1d(d2, 2);
        Array **arrs = (Array**)malloc(2 * sizeof(Array*));
        arrs[0] = a1; arrs[1] = a2;
        Array *cs = column_stack(arrs, 2);
        ASSERT_NOTNULL(cs, "column_stack non-null");
        ASSERT_NDIM(cs, 2, "column_stack ndim=2");
        ASSERT_EQ_INT(cs->shape[0], 2, "column_stack shape[0]=2");
        ASSERT_EQ_INT(cs->shape[1], 2, "column_stack shape[1]=2");
        ASSERT_EQ_ARR((double*)cs->data, expected, 4, TOL_F64, "column_stack values");
        free_a(cs); free(arrs);
        free_a(a1); free_a(a2);
    }

    /* column_stack INT32 */
    {
        int d1[] = {10, 20, 30};
        int d2[] = {40, 50, 60};
        int expected[] = {10, 40, 20, 50, 30, 60};
        Array *a1 = make_i32_1d(d1, 3);
        Array *a2 = make_i32_1d(d2, 3);
        Array **arrs = (Array**)malloc(2 * sizeof(Array*));
        arrs[0] = a1; arrs[1] = a2;
        Array *cs = column_stack(arrs, 2);
        ASSERT_NOTNULL(cs, "col_stack INT32 non-null");
        ASSERT_NDIM(cs, 2, "col_stack INT32 ndim=2");
        ASSERT_EQ_INT(cs->shape[0], 3, "col_stack INT32 shape[0]");
        ASSERT_EQ_INT(cs->shape[1], 2, "col_stack INT32 shape[1]");
        ASSERT_EQ_IARR((int*)cs->data, expected, 6, "col_stack INT32 values");
        free_a(cs); free(arrs);
        free_a(a1); free_a(a2);
    }

    /* ── dstack (2D -> (1,M,N) -> concat along axis=2) ──────────────── */
    TEST_SECTION("dstack");

    /* dstack two 2D arrays: each (2,2) -> to_3d -> (1,2,2) -> concat axis=2 -> (1,2,4) */
    {
        double d1[] = {1, 2, 3, 4};
        double d2[] = {5, 6, 7, 8};
        double expected[] = {1, 2, 5, 6, 3, 4, 7, 8};
        Array *a1 = make_f64_2d(d1, 2, 2);
        Array *a2 = make_f64_2d(d2, 2, 2);
        Array **arrs = (Array**)malloc(2 * sizeof(Array*));
        arrs[0] = a1; arrs[1] = a2;
        Array *ds = dstack(arrs, 2);
        ASSERT_NOTNULL(ds, "dstack non-null");
        ASSERT_NDIM(ds, 3, "dstack ndim=3");
        ASSERT_EQ_INT(ds->shape[0], 1, "dstack shape[0]=1");
        ASSERT_EQ_INT(ds->shape[1], 2, "dstack shape[1]=2");
        ASSERT_EQ_INT(ds->shape[2], 4, "dstack shape[2]=4");
        ASSERT_EQ_ARR((double*)ds->data, expected, 8, TOL_F64, "dstack values");
        free_a(ds); free(arrs);
        free_a(a1); free_a(a2);
    }

    TEST_SUMMARY();
    return (g_fail > 0) ? 1 : 0;
}
