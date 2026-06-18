#include "test_utils.h"
#include <stdint.h>


Array* get_scalar_iarr(Array *arr, int idx) {
    return create_scalar_array(((int*)arr->data) + idx, arr->dtype);
}

Array* get_scalar_farr(Array *arr, int idx) {
    return create_scalar_array(((double*)arr->data) + idx, arr->dtype);
}

int main(void) {
    TEST_MAIN("test_shape_manip");

    /* ── reshape ─────────────────────────────────────────────────── */
    TEST_SECTION("reshape");

    /* reshape (6,) -> (2,3) */
    {
        double data[] = {1, 2, 3, 4, 5, 6};
        double expected[] = {1, 2, 3, 4, 5, 6};
        Array *a = make_f64_1d(data, 6);
        int new_shape[] = {2, 3};
        Array *r = reshape(a, new_shape, 2);
        ASSERT_NOTNULL(r, "reshape (6)->(2,3) non-null");
        ASSERT_NDIM(r, 2, "reshape ndim=2");
        ASSERT_SIZE(r, 6, "reshape size preserved");
        ASSERT_EQ_ARR((double*)r->data, expected, 6, TOL_F64, "reshape values");
        ASSERT_EQ_INT(r->shape[0], 2, "reshape shape[0]=2");
        ASSERT_EQ_INT(r->shape[1], 3, "reshape shape[1]=3");
        free_a(r);
        free_a(a);
    }

    /* reshape INT32 */
    {
        int data[] = {10, 20, 30, 40, 50, 60};
        int expected[] = {10, 20, 30, 40, 50, 60};
        Array *a = make_i32_1d(data, 6);
        int new_shape[] = {3, 2};
        Array *r = reshape(a, new_shape, 2);
        ASSERT_NOTNULL(r, "reshape INT32 (6)->(3,2) non-null");
        ASSERT_NDIM(r, 2, "reshape INT32 ndim=2");
        ASSERT_EQ_INT(r->shape[0], 3, "reshape INT32 shape[0]=3");
        ASSERT_EQ_INT(r->shape[1], 2, "reshape INT32 shape[1]=2");
        ASSERT_EQ_IARR((int*)r->data, expected, 6, "reshape INT32 values");
        free_a(r);
        free_a(a);
    }

    /* reshape with wrong size */
    {
        double data[] = {1, 2, 3, 4, 5, 6};
        Array *a = make_f64_1d(data, 6);
        int new_shape[] = {4, 4}; /* 16 != 6 */
        Array *r = reshape(a, new_shape, 2);
        /* may return NULL */
        if (r != NULL) free_a(r);
        free_a(a);
        /* Just ensure no crash; no assertion */
    }

    /* ── resize ──────────────────────────────────────────────────── */
    TEST_SECTION("resize");

    /* resize (3)->(5): grow */
    {
        double data[] = {1, 2, 3};
        Array *a = make_f64_1d(data, 3);
        int new_shape[] = {5};
        Array *r = resize(a, new_shape, 1);
        ASSERT_NOTNULL(r, "resize (3)->(5) non-null");
        ASSERT_SIZE(r, 5, "resize size=5");
        ASSERT_NDIM(r, 1, "resize ndim=1");
        /* first 3 elements should match */
        double *rd = (double*)r->data;
        ASSERT_EQ_DBL(rd[0], 1.0, TOL_F64, "resize grow [0]=1");
        ASSERT_EQ_DBL(rd[1], 2.0, TOL_F64, "resize grow [1]=2");
        ASSERT_EQ_DBL(rd[2], 3.0, TOL_F64, "resize grow [2]=3");
        free_a(r);
        free_a(a);
    }

    /* resize (4)->(2): shrink */
    {
        double data[] = {10, 20, 30, 40};
        Array *a = make_f64_1d(data, 4);
        int new_shape[] = {2};
        Array *r = resize(a, new_shape, 1);
        if (r != NULL) {
            ASSERT_SIZE(r, 2, "resize shrink size=2");
            double *rd = (double*)r->data;
            ASSERT_EQ_DBL(rd[0], 10.0, TOL_F64, "resize shrink [0]");
            ASSERT_EQ_DBL(rd[1], 20.0, TOL_F64, "resize shrink [1]");
            free_a(r);
        }
        free_a(a);
    }

    /* resize INT32 */
    {
        int data[] = {100, 200};
        Array *a = make_i32_1d(data, 2);
        int new_shape[] = {4};
        Array *r = resize(a, new_shape, 1);
        ASSERT_NOTNULL(r, "resize INT32 non-null");
        ASSERT_SIZE(r, 4, "resize INT32 size=4");
        free_a(r);
        free_a(a);
    }

    /* ── transpose ────────────────────────────────────────────────── */
    TEST_SECTION("transpose");

    /* transpose 2x3 -> 3x2 */
    {
        double data[] = {1, 2, 3, 4, 5, 6};
        double expected[] = {1, 4, 2, 5, 3, 6};
        Array *a = make_f64_2d(data, 2, 3);
        Array *t = transpose(a);
        ASSERT_NOTNULL(t, "transpose non-null");
        ASSERT_NDIM(t, 2, "transpose ndim=2");
        ASSERT_EQ_INT(t->shape[0], 3, "transpose shape[0]=3");
        ASSERT_EQ_INT(t->shape[1], 2, "transpose shape[1]=2");
        ASSERT_EQ_ARR((double*)t->data, expected, 6, TOL_F64, "transpose values");
        free_a(t);
        free_a(a);
    }

    /* transpose INT32 2x3 -> 3x2 */
    {
        int data[] = {10, 20, 30, 40, 50, 60};
        int expected[] = {10, 40, 20, 50, 30, 60};
        Array *a = make_i32_2d(data, 2, 3);
        Array *t = transpose(a);
        ASSERT_NOTNULL(t, "transpose INT32 non-null");
        ASSERT_NDIM(t, 2, "transpose INT32 ndim=2");
        ASSERT_EQ_INT(t->shape[0], 3, "transpose INT32 shape[0]");
        ASSERT_EQ_INT(t->shape[1], 2, "transpose INT32 shape[1]");
        ASSERT_EQ_IARR((int*)t->data, expected, 6, "transpose INT32 values");
        free_a(t);
        free_a(a);
    }

    /* transpose square 2x2 */
    {
        double data[] = {1, 2, 3, 4};
        double expected[] = {1, 3, 2, 4};
        Array *a = make_f64_2d(data, 2, 2);
        Array *t = transpose(a);
        ASSERT_NOTNULL(t, "transpose 2x2 non-null");
        ASSERT_EQ_ARR((double*)t->data, expected, 4, TOL_F64, "transpose 2x2 values");
        free_a(t);
        free_a(a);
    }

    /* ── flatten ──────────────────────────────────────────────────── */
    TEST_SECTION("flatten");

    /* flatten 2x2 -> [1,2,3,4] */
    {
        double data[] = {1, 2, 3, 4};
        double expected[] = {1, 2, 3, 4};
        Array *a = make_f64_2d(data, 2, 2);
        Array *f = flatten(a);
        ASSERT_NOTNULL(f, "flatten non-null");
        ASSERT_NDIM(f, 1, "flatten ndim=1");
        ASSERT_SIZE(f, 4, "flatten size=4");
        ASSERT_EQ_ARR((double*)f->data, expected, 4, TOL_F64, "flatten values");
        free_a(f);
        free_a(a);
    }

    /* flatten INT32 3x2 */
    {
        int data[] = {1, 2, 3, 4, 5, 6};
        int expected[] = {1, 2, 3, 4, 5, 6};
        Array *a = make_i32_2d(data, 3, 2);
        Array *f = flatten(a);
        ASSERT_NOTNULL(f, "flatten INT32 non-null");
        ASSERT_NDIM(f, 1, "flatten INT32 ndim=1");
        ASSERT_EQ_IARR((int*)f->data, expected, 6, "flatten INT32 values");
        free_a(f);
        free_a(a);
    }

    /* flatten already 1D */
    {
        double data[] = {5, 6, 7};
        double expected[] = {5, 6, 7};
        Array *a = make_f64_1d(data, 3);
        Array *f = flatten(a);
        ASSERT_NOTNULL(f, "flatten 1D non-null");
        ASSERT_NDIM(f, 1, "flatten 1D ndim=1");
        ASSERT_EQ_ARR((double*)f->data, expected, 3, TOL_F64, "flatten 1D values");
        free_a(f);
        free_a(a);
    }

    /* ── ravel ────────────────────────────────────────────────────── */
    TEST_SECTION("ravel");

    /* ravel with default order */
    {
        double data[] = {1, 2, 3, 4};
        double expected[] = {1, 2, 3, 4};
        Array *a = make_f64_2d(data, 2, 2);
        Array *r = ravel(a, "C");
        ASSERT_NOTNULL(r, "ravel C-order non-null");
        ASSERT_NDIM(r, 1, "ravel ndim=1");
        ASSERT_EQ_ARR((double*)r->data, expected, 4, TOL_F64, "ravel C-order values");
        free_a(r);
        free_a(a);
    }

    /* ravel INT32 */
    {
        int data[] = {10, 20, 30, 40, 50, 60};
        int expected[] = {10, 20, 30, 40, 50, 60};
        Array *a = make_i32_2d(data, 2, 3);
        Array *r = ravel(a, "C");
        ASSERT_NOTNULL(r, "ravel INT32 non-null");
        ASSERT_NDIM(r, 1, "ravel INT32 ndim=1");
        ASSERT_EQ_IARR((int*)r->data, expected, 6, "ravel INT32 values");
        free_a(r);
        free_a(a);
    }

    /* ── flatnonzero (returns INT64) ─────────────────────────────────── */
    TEST_SECTION("flatnonzero");

    /* flatnonzero [0,1,0,2,3,0] -> [1,3,4] */
    {
        double data[] = {0, 1, 0, 2, 3, 0};
        int64_t expected[] = {1, 3, 4};
        Array *a = make_f64_1d(data, 6);
        Array *fnz = flatnonzero(a);
        ASSERT_NOTNULL(fnz, "flatnonzero non-null");
        ASSERT_SIZE(fnz, 3, "flatnonzero size=3");
        if (fnz->data) {
            int64_t *d = (int64_t*)fnz->data;
            ASSERT_EQ_I64(d[0], expected[0], "flatnonzero[0]");
            ASSERT_EQ_I64(d[1], expected[1], "flatnonzero[1]");
            ASSERT_EQ_I64(d[2], expected[2], "flatnonzero[2]");
        }
        free_a(fnz);
        free_a(a);
    }

    /* flatnonzero INT32 [0,0,0,1] -> [3] (returns INT64) */
    {
        int data[] = {0, 0, 0, 1};
        Array *a = make_i32_1d(data, 4);
        Array *fnz = flatnonzero(a);
        ASSERT_NOTNULL(fnz, "flatnonzero INT32 non-null");
        ASSERT_EQ_I64(((int64_t*)fnz->data)[0], 3, "flatnonzero INT32 [3]");
        free_a(fnz);
        free_a(a);
    }

    /* flatnonzero all zeros */
    {
        double data[] = {0, 0, 0};
        Array *a = make_f64_1d(data, 3);
        Array *fnz = flatnonzero(a);
        /* should exist, possibly empty */
        ASSERT_NOTNULL(fnz, "flatnonzero all-zeros non-null");
        ASSERT_SIZE(fnz, 0, "flatnonzero all-zeros empty");
        free_a(fnz);
        free_a(a);
    }

    /* ── expand_dims ──────────────────────────────────────────────── */
    TEST_SECTION("expand_dims");

    /* expand_dims axis=0 */
    {
        double data[] = {1, 2, 3};
        double expected[] = {1, 2, 3};
        Array *a = make_f64_1d(data, 3);
        Array *e = expand_dims(a, 0);
        ASSERT_NOTNULL(e, "expand_dims axis=0 non-null");
        ASSERT_NDIM(e, 2, "expand_dims axis=0 ndim=2");
        ASSERT_EQ_INT(e->shape[0], 1, "expand_dims axis=0 shape[0]=1");
        ASSERT_EQ_INT(e->shape[1], 3, "expand_dims axis=0 shape[1]=3");
        ASSERT_EQ_ARR((double*)e->data, expected, 3, TOL_F64, "expand_dims axis=0 values");
        free_a(e);
        free_a(a);
    }

    /* expand_dims axis=1 */
    {
        double data[] = {1, 2, 3};
        double expected[] = {1, 2, 3};
        Array *a = make_f64_1d(data, 3);
        Array *e = expand_dims(a, 1);
        ASSERT_NOTNULL(e, "expand_dims axis=1 non-null");
        ASSERT_NDIM(e, 2, "expand_dims axis=1 ndim=2");
        ASSERT_EQ_INT(e->shape[0], 3, "expand_dims axis=1 shape[0]=3");
        ASSERT_EQ_INT(e->shape[1], 1, "expand_dims axis=1 shape[1]=1");
        ASSERT_EQ_ARR((double*)e->data, expected, 3, TOL_F64, "expand_dims axis=1 values");
        free_a(e);
        free_a(a);
    }

    /* expand_dims INT32 axis=0 */
    {
        int data[] = {10, 20};
        int expected[] = {10, 20};
        Array *a = make_i32_1d(data, 2);
        Array *e = expand_dims(a, 0);
        ASSERT_NOTNULL(e, "expand_dims INT32 axis=0 non-null");
        ASSERT_NDIM(e, 2, "expand_dims INT32 ndim=2");
        ASSERT_EQ_INT(e->shape[0], 1, "expand_dims INT32 shape[0]");
        ASSERT_EQ_INT(e->shape[1], 2, "expand_dims INT32 shape[1]");
        ASSERT_EQ_IARR((int*)e->data, expected, 2, "expand_dims INT32 values");
        free_a(e);
        free_a(a);
    }

    /* ── squeeze ──────────────────────────────────────────────────── */
    TEST_SECTION("squeeze");

    /* squeeze shape (1,3,1) axis=0 -> (3,1) */
    {
        double data[] = {1, 2, 3};
        Array *a = create_array((int[]){1, 3, 1}, 3, FLOAT64);
        memcpy(a->data, data, 3 * sizeof(double));
        Array *s = squeeze(a, 0);
        ASSERT_NOTNULL(s, "squeeze axis=0 non-null");
        ASSERT_NDIM(s, 2, "squeeze axis=0 ndim=2");
        ASSERT_EQ_INT(s->shape[0], 3, "squeeze axis=0 shape[0]=3");
        ASSERT_EQ_INT(s->shape[1], 1, "squeeze axis=0 shape[1]=1");
        free_a(s);
        free_a(a);
    }

    /* squeeze shape (1,3,1) axis=2 -> (1,3) */
    {
        double data[] = {4, 5, 6};
        Array *a = create_array((int[]){1, 3, 1}, 3, FLOAT64);
        memcpy(a->data, data, 3 * sizeof(double));
        Array *s = squeeze(a, 2);
        ASSERT_NOTNULL(s, "squeeze axis=2 non-null");
        ASSERT_NDIM(s, 2, "squeeze axis=2 ndim=2");
        ASSERT_EQ_INT(s->shape[0], 1, "squeeze axis=2 shape[0]=1");
        ASSERT_EQ_INT(s->shape[1], 3, "squeeze axis=2 shape[1]=3");
        free_a(s);
        free_a(a);
    }

    /* squeeze INT32 (1,4,1) axis=1 doesn't remove */
    {
        int data[] = {1, 2, 3, 4};
        Array *a = create_array((int[]){1, 4, 1}, 3, INT32);
        memcpy(a->data, data, 4 * sizeof(int));
        /* squeeze axis=1: dim is 4, not 1, so it stays? or returns NULL? */
        Array *s = squeeze(a, 1);
        /* If the API only squeezes dimensions of size 1, this may be
           a no-op or return something. Just check it doesn't crash. */
        if (s != NULL) free_a(s);
        free_a(a);
    }

    TEST_SUMMARY();
    return (g_fail > 0) ? 1 : 0;
}
