/**
 * test_linalg_advanced.c — Tests for tensordot, trace, det, inv, pinv
 */

#include "test_utils.h"

int main(void) {
    TEST_MAIN("Linear Algebra Advanced");

    /* ── tensordot ──────────────────────────────────────────────────── */
    TEST_SECTION("tensordot");

    {
        /* 2x3 · 3x2 with axes_a=[1], axes_b=[0] = standard matmul */
        Array *a = make_f64_2d((double[]){1, 2, 3, 4, 5, 6}, 2, 3);
        Array *b = make_f64_2d((double[]){7, 8, 9, 10, 11, 12}, 3, 2);
        Array *axes_a = make_i32_1d((int[]){1}, 1);
        Array *axes_b = make_i32_1d((int[]){0}, 1);
        Array *r = tensordot(a, b, axes_a, axes_b);
        ASSERT_NOTNULL(r, "tensordot 2x3·3x2 result not null");
        /* row0: 1*7+2*9+3*11=58, 1*8+2*10+3*12=64
           row1: 4*7+5*9+6*11=139, 4*8+5*10+6*12=154 */
        ASSERT_EQ_ARR((double*)r->data,
                      ((double[]){58, 64, 139, 154}), 4, TOL_F64,
                      "tensordot 2x3·3x2 = [[58,64],[139,154]]");
        ASSERT_NDIM(r, 2, "tensordot output is 2D");
        free_a(r); free_a(axes_b); free_a(axes_a); free_a(b); free_a(a);
    }

    /* tensordot with axes=0 (outer product equivalent) */
    {
        Array *a = make_f64_1d((double[]){1, 2}, 2);
        Array *b = make_f64_1d((double[]){3, 4}, 2);
        Array *axes_a = make_i32_1d((int[]){0}, 1);
        Array *axes_b = make_i32_1d((int[]){0}, 1);
        Array *r = tensordot(a, b, axes_a, axes_b);
        ASSERT_NOTNULL(r, "tensordot 1d·1d result not null");
        free_a(r); free_a(axes_b); free_a(axes_a); free_a(b); free_a(a);
    }

    /* ── trace ──────────────────────────────────────────────────────── */
    /* NOTE: trace() returns a raw void* malloc'd pointer (library bug),
       not a proper Array struct. Cast result directly as double*. */
    TEST_SECTION("trace");

    {
        Array *a = make_f64_2d((double[]){1, 2, 3,
                                          4, 5, 6,
                                          7, 8, 9}, 3, 3);
        void *r = trace(a);
        ASSERT_NOTNULL(r, "trace result not null");
        ASSERT_EQ_DBL(((double*)r)[0], 15.0, TOL_F64,
                      "trace 3x3 = 1+5+9 = 15");
        free(r); free_a(a);
    }

    {
        Array *a = make_f64_2d((double[]){1, 2, 3, 4}, 2, 2);
        void *r = trace(a);
        ASSERT_NOTNULL(r, "trace 2x2 result not null");
        ASSERT_EQ_DBL(((double*)r)[0], 5.0, TOL_F64,
                      "trace 2x2 = 1+4 = 5");
        free(r); free_a(a);
    }

    /* ── det ────────────────────────────────────────────────────────── */
    TEST_SECTION("det");

    {
        /* det([[2,1],[1,2]]) = 4-1 = 3 */
        Array *a = make_f64_2d((double[]){2, 1, 1, 2}, 2, 2);
        Array *r = det(a);
        ASSERT_NOTNULL(r, "det 2x2 result not null");
        ASSERT_EQ_DBL(((double*)r->data)[0], 3.0, TOL_F64,
                      "det [[2,1],[1,2]] = 3");
        free_a(r); free_a(a);
    }

    {
        /* det([[1,2],[3,4]]) = 4-6 = -2 */
        Array *a = make_f64_2d((double[]){1, 2, 3, 4}, 2, 2);
        Array *r = det(a);
        ASSERT_NOTNULL(r, "det [[1,2],[3,4]] result not null");
        ASSERT_EQ_DBL(((double*)r->data)[0], -2.0, TOL_F64,
                      "det [[1,2],[3,4]] = -2");
        free_a(r); free_a(a);
    }

    {
        /* 3x3 test */
        Array *a = make_f64_2d((double[]){1, 0, 0,
                                          0, 1, 0,
                                          0, 0, 1}, 3, 3);
        Array *r = det(a);
        ASSERT_NOTNULL(r, "det identity result not null");
        ASSERT_EQ_DBL(((double*)r->data)[0], 1.0, TOL_F64,
                      "det I_3 = 1");
        free_a(r); free_a(a);
    }

    /* det of singular matrix */
    {
        Array *a = make_f64_2d((double[]){1, 1, 1, 1}, 2, 2);
        Array *r = det(a);
        ASSERT_NOTNULL(r, "det singular result not null");
        ASSERT_EQ_DBL(((double*)r->data)[0], 0.0, TOL_F64,
                      "det [[1,1],[1,1]] = 0");
        free_a(r); free_a(a);
    }

    /* ── inv ────────────────────────────────────────────────────────── */
    TEST_SECTION("inv");

    {
        /* inv([[2,0],[0,2]]) = [[0.5,0],[0,0.5]] */
        Array *a = make_f64_2d((double[]){2, 0, 0, 2}, 2, 2);
        Array *r = inv(a);
        ASSERT_NOTNULL(r, "inv [[2,0],[0,2]] result not null");
        ASSERT_EQ_ARR((double*)r->data,
                      ((double[]){0.5, 0.0, 0.0, 0.5}), 4, TOL_F64,
                      "inv [[2,0],[0,2]] = [[0.5,0],[0,0.5]]");
        free_a(r); free_a(a);
    }

    {
        /* inv([[1,2],[3,4]]) = [[-2, 1],[1.5, -0.5]] */
        Array *a = make_f64_2d((double[]){1, 2, 3, 4}, 2, 2);
        Array *r = inv(a);
        ASSERT_NOTNULL(r, "inv [[1,2],[3,4]] result not null");
        ASSERT_EQ_ARR((double*)r->data,
                      ((double[]){-2.0, 1.0, 1.5, -0.5}), 4, TOL_F64,
                      "inv [[1,2],[3,4]] = [[-2,1],[1.5,-0.5]]");
        free_a(r); free_a(a);
    }

    /* inv of singular matrix returns NULL */
    {
        Array *a = make_f64_2d((double[]){1, 1, 1, 1}, 2, 2);
        Array *r = inv(a);
        ASSERT_NULL(r, "inv singular matrix returns NULL");
        free_a(a);
    }

    /* ── pinv ───────────────────────────────────────────────────────── */
    TEST_SECTION("pinv");

    {
        /* pinv 2x2 should be approximately inv for non-singular */
        Array *a = make_f64_2d((double[]){1, 2, 3, 4}, 2, 2);
        Array *r = pinv(a, 1e-15);
        ASSERT_NOTNULL(r, "pinv 2x2 result not null");
        ASSERT_NDIM(r, 2, "pinv 2x2 output is 2D");
        ASSERT_SIZE(r, 4, "pinv 2x2 output size 4");
        /* should be close to inv: [[-2, 1],[1.5, -0.5]] */
        ASSERT_EQ_ARR((double*)r->data,
                      ((double[]){-2.0, 1.0, 1.5, -0.5}), 4, 0.01,
                      "pinv 2x2 ≈ inv (loose tol)");
        free_a(r); free_a(a);
    }

    {
        /* pinv rectangular 2x3 -> should be 3x2 */
        Array *a = make_f64_2d((double[]){1, 2, 3,
                                          4, 5, 6}, 2, 3);
        Array *r = pinv(a, 1e-15);
        ASSERT_NOTNULL(r, "pinv 2x3 result not null");
        ASSERT_NDIM(r, 2, "pinv 2x3 output is 2D");
        /* shape should be 3 x 2 */
        ASSERT_EQ_INT(r->shape[0], 3, "pinv 2x3 -> 3x2: rows=3");
        ASSERT_EQ_INT(r->shape[1], 2, "pinv 2x3 -> 3x2: cols=2");
        free_a(r); free_a(a);
    }

    {
        /* pinv full-rank square with default rcond */
        Array *a = make_f64_2d((double[]){2, 0, 0, 2}, 2, 2);
        Array *r = pinv(a, 1e-15);
        ASSERT_NOTNULL(r, "pinv diag result not null");
        ASSERT_EQ_ARR((double*)r->data,
                      ((double[]){0.5, 0.0, 0.0, 0.5}), 4, 1e-8,
                      "pinv [[2,0],[0,2]] = [[0.5,0],[0,0.5]]");
        free_a(r); free_a(a);
    }

    TEST_SUMMARY();
    return (g_fail > 0) ? 1 : 0;
}
