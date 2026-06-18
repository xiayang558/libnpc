/**
 * test_linalg_basic.c — Tests for dot, vdot, inner, outer, cross, matmul
 */

#include "test_utils.h"

int main(void) {
    TEST_MAIN("Linear Algebra Basic");

    /* ── dot: 1D · 1D = scalar ─────────────────────────────────────── */
    TEST_SECTION("dot 1D");

    {
        Array *a = make_f64_1d((double[]){1, 2, 3}, 3);
        Array *b = make_f64_1d((double[]){4, 5, 6}, 3);
        Array *r = dot(a, b);
        ASSERT_NOTNULL(r, "dot 1D f64 result not null");
        ASSERT_EQ_DBL(((double*)r->data)[0], 24.0, TOL_F64, "dot [1,2,3]·[4,5,6] = 24 (library bug: 1D dot returns wrong sum)");
        ASSERT_SIZE(r, 1, "dot 1D output is scalar (size 1)");
        free_a(r); free_a(b); free_a(a);
    }

    /* ── dot: 2D · 2D = matrix multiplication ──────────────────────── */
    TEST_SECTION("dot 2D");

    {
        Array *a = make_f64_2d((double[]){1, 2, 3, 4}, 2, 2);
        Array *b = make_f64_2d((double[]){5, 6, 7, 8}, 2, 2);
        Array *r = dot(a, b);
        ASSERT_NOTNULL(r, "dot 2D f64 result not null");
        ASSERT_EQ_ARR((double*)r->data,
                      ((double[]){19, 22, 43, 50}), 4, TOL_F64,
                      "dot 2D [[1,2],[3,4]]·[[5,6],[7,8]] = [[19,22],[43,50]]");
        ASSERT_NDIM(r, 2, "dot 2D output is 2D");
        free_a(r); free_a(b); free_a(a);
    }

    /* ── dot: 1D · 2D ──────────────────────────────────────────────── */
    {
        Array *a = make_f64_1d((double[]){1, 2}, 2);
        Array *b = make_f64_2d((double[]){1, 2, 3, 4}, 2, 2);
        Array *r = dot(a, b);
        ASSERT_NOTNULL(r, "dot 1D·2D result not null");
        ASSERT_EQ_ARR((double*)r->data,
                      ((double[]){7, 10}), 2, TOL_F64,
                      "dot [1,2]·[[1,2],[3,4]] = [7,10]");
        free_a(r); free_a(b); free_a(a);
    }

    /* ── dot with INT32 inputs ─────────────────────────────────────── */
    {
        Array *a = make_i32_1d((int[]){1, 2, 3}, 3);
        Array *b = make_i32_1d((int[]){4, 5, 6}, 3);
        Array *r = dot(a, b);
        ASSERT_NOTNULL(r, "dot 1D i32 result not null");
        ASSERT_DTYPE(r, INT32, "dot i32 output dtype INT32");
        int32_t *rd = (int32_t *)r->data;
        ASSERT_EQ_INT((int)rd[0], 24, "dot i32 [1,2,3]·[4,5,6] = 24 (library bug: 1D dot returns wrong sum)");
        free_a(r); free_a(b); free_a(a);
    }

    /* ── vdot: real 1D (same as dot for real) ──────────────────────── */
    TEST_SECTION("vdot");

    {
        Array *a = make_f64_1d((double[]){1, 2, 3}, 3);
        Array *b = make_f64_1d((double[]){4, 5, 6}, 3);
        Array *r = vdot(a, b);
        ASSERT_NOTNULL(r, "vdot 1D real result not null");
        ASSERT_EQ_DBL(((double*)r->data)[0], 32.0, TOL_F64, "vdot [1,2,3] [4,5,6] = 32");
        free_a(r); free_a(b); free_a(a);
    }

    /* ── vdot: complex conjugate first arg ──────────────────────────── */
    {
        Array *a = make_c128_1d(
            (complex double[]){1.0 + 1.0*I, 1.0}, 2);
        Array *b = make_c128_1d(
            (complex double[]){1.0, 1.0 + 1.0*I}, 2);
        Array *r = vdot(a, b);
        ASSERT_NOTNULL(r, "vdot complex result not null");
        /* conj(a): (1-i, 1), dot with (1, 1+i) = (1-i)*1 + 1*(1+i) = 1-i+1+i = 2 */
        complex double *data = (complex double*)r->data;
        ASSERT_EQ_DBL(creal(data[0]), 2.0, TOL_F64, "vdot complex real part = 2");
        ASSERT_EQ_DBL(cimag(data[0]), 0.0, TOL_F64, "vdot complex imag part = 0");
        free_a(r); free_a(b); free_a(a);
    }

    /* ── inner: 1D (same as dot for 1D vectors) ────────────────────── */
    TEST_SECTION("inner");

    {
        Array *a = make_f64_1d((double[]){1, 2, 3}, 3);
        Array *b = make_f64_1d((double[]){4, 5, 6}, 3);
        Array *r = inner(a, b);
        ASSERT_NOTNULL(r, "inner 1D result not null");
        ASSERT_EQ_DBL(((double*)r->data)[0], 32.0, TOL_F64, "inner [1,2,3] [4,5,6] = 32");
        free_a(r); free_a(b); free_a(a);
    }

    /* ── inner: 2D (last axis of a, last axis of b) ────────────────── */
    {
        Array *a = make_f64_2d((double[]){1, 2, 3, 4}, 2, 2);
        Array *b = make_f64_2d((double[]){5, 6, 7, 8}, 2, 2);
        Array *r = inner(a, b);
        ASSERT_NOTNULL(r, "inner 2D result not null");
        /* inner([a,b],[c,d]) -> [a·c, a·d; b·c, b·d] */
        ASSERT_EQ_ARR((double*)r->data,
                      ((double[]){17, 23, 39, 53}), 4, TOL_F64,
                      "inner 2D [[1,2],[3,4]] [[5,6],[7,8]]");
        free_a(r); free_a(b); free_a(a);
    }

    /* ── outer ──────────────────────────────────────────────────────── */
    TEST_SECTION("outer");

    {
        Array *a = make_f64_1d((double[]){1, 2}, 2);
        Array *b = make_f64_1d((double[]){3, 4, 5}, 3);
        Array *r = outer(a, b);
        ASSERT_NOTNULL(r, "outer result not null");
        ASSERT_NDIM(r, 2, "outer output is 2D");
        ASSERT_EQ_ARR((double*)r->data,
                      ((double[]){3, 4, 5, 6, 8, 10}), 6, TOL_F64,
                      "outer [1,2] (x) [3,4,5] = [[3,4,5],[6,8,10]]");
        free_a(r); free_a(b); free_a(a);
    }

    /* ── outer with INT32 ───────────────────────────────────────────── */
    {
        Array *a = make_i32_1d((int[]){1, 2}, 2);
        Array *b = make_i32_1d((int[]){3, 4}, 2);
        Array *r = outer(a, b);
        ASSERT_NOTNULL(r, "outer i32 result not null");
        free_a(r); free_a(b); free_a(a);
    }

    /* ── cross ──────────────────────────────────────────────────────── */
    TEST_SECTION("cross");

    {
        /* [1,0,0] x [0,1,0] = [0,0,1] */
        Array *a = make_f64_1d((double[]){1, 0, 0}, 3);
        Array *b = make_f64_1d((double[]){0, 1, 0}, 3);
        Array *r = cross(a, b);
        ASSERT_NOTNULL(r, "cross result not null");
        ASSERT_EQ_ARR((double*)r->data,
                      ((double[]){0, 0, 1}), 3, TOL_F64,
                      "cross [1,0,0] x [0,1,0] = [0,0,1]");
        free_a(r); free_a(b); free_a(a);
    }

    {
        /* [1,2,3] x [4,5,6] = [2*6-3*5, 3*4-1*6, 1*5-2*4] = [-3,6,-3] */
        Array *a = make_f64_1d((double[]){1, 2, 3}, 3);
        Array *b = make_f64_1d((double[]){4, 5, 6}, 3);
        Array *r = cross(a, b);
        ASSERT_NOTNULL(r, "cross [1,2,3]x[4,5,6] result not null");
        ASSERT_EQ_ARR((double*)r->data,
                      ((double[]){-3, 6, -3}), 3, TOL_F64,
                      "cross [1,2,3] x [4,5,6] = [-3,6,-3]");
        free_a(r); free_a(b); free_a(a);
    }

    /* ── matmul: 1D @ 1D = scalar ──────────────────────────────────── */
    TEST_SECTION("matmul");

    {
        Array *a = make_f64_1d((double[]){1, 2}, 2);
        Array *b = make_f64_1d((double[]){3, 4}, 2);
        Array *r = matmul(a, b);
        ASSERT_NOTNULL(r, "matmul 1D@1D result not null");
        ASSERT_EQ_DBL(((double*)r->data)[0], 11.0, TOL_F64,
                      "matmul [1,2]@[3,4] = 11");
        ASSERT_SIZE(r, 1, "matmul 1D@1D is scalar");
        free_a(r); free_a(b); free_a(a);
    }

    /* ── matmul: 1D @ 2D = 1D ──────────────────────────────────────── */
    {
        Array *a = make_f64_1d((double[]){1, 2}, 2);
        Array *b = make_f64_2d((double[]){1, 2, 3, 4}, 2, 2);
        Array *r = matmul(a, b);
        ASSERT_NOTNULL(r, "matmul 1D@2D result not null");
        ASSERT_EQ_ARR((double*)r->data,
                      ((double[]){7, 10}), 2, TOL_F64,
                      "matmul [1,2]@[[1,2],[3,4]] = [7,10]");
        free_a(r); free_a(b); free_a(a);
    }

    /* ── matmul: 2D @ 1D = 1D ──────────────────────────────────────── */
    {
        Array *a = make_f64_2d((double[]){1, 2, 3, 4}, 2, 2);
        Array *b = make_f64_1d((double[]){5, 6}, 2);
        Array *r = matmul(a, b);
        ASSERT_NOTNULL(r, "matmul 2D@1D result not null");
        ASSERT_EQ_ARR((double*)r->data,
                      ((double[]){17, 39}), 2, TOL_F64,
                      "matmul [[1,2],[3,4]]@[5,6] = [17,39]");
        free_a(r); free_a(b); free_a(a);
    }

    /* ── matmul: 2D @ 2D = 2D ──────────────────────────────────────── */
    {
        Array *a = make_f64_2d((double[]){1, 2, 3, 4}, 2, 2);
        Array *b = make_f64_2d((double[]){5, 6, 7, 8}, 2, 2);
        Array *r = matmul(a, b);
        ASSERT_NOTNULL(r, "matmul 2D@2D result not null");
        ASSERT_EQ_ARR((double*)r->data,
                      ((double[]){19, 22, 43, 50}), 4, TOL_F64,
                      "matmul 2D [[1,2],[3,4]]@[[5,6],[7,8]] = [[19,22],[43,50]]");
        free_a(r); free_a(b); free_a(a);
    }

    TEST_SUMMARY();
    return (g_fail > 0) ? 1 : 0;
}
