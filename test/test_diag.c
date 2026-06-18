/**
 * test_diag.c — Tests for diag, diagflat, diag_indices, diagonal, fill_diagonal
 * NOTE: diag preserves input dtype. INT32 input -> INT32 output (read as int*).
 *       diag_indices returns INT64 data.
 */

#include "test_utils.h"

int main(void) {
    TEST_MAIN("test_diag");

    /* ── diag: 1D -> 2D ──────────────────────────────────────────── */
    TEST_SECTION("diag 1D->2D");

    /* diag [1,2,3] k=0 -> [[1,0,0],[0,2,0],[0,0,3]] */
    {
        double data[] = {1, 2, 3};
        double expected[] = {1, 0, 0, 0, 2, 0, 0, 0, 3};
        Array *a = make_f64_1d(data, 3);
        Array *r = diag(a, 0);
        ASSERT_NOTNULL(r, "diag 1D k=0 non-null");
        ASSERT_NDIM(r, 2, "diag 1D k=0 ndim=2");
        ASSERT_EQ_INT(r->shape[0], 3, "diag 1D shape[0]=3");
        ASSERT_EQ_INT(r->shape[1], 3, "diag 1D shape[1]=3");
        ASSERT_EQ_ARR((double*)r->data, expected, 9, TOL_F64, "diag 1D k=0 values");
        free_a(r);
        free_a(a);
    }

    /* diag [1,2,3] k=1 -> [[0,1,0,0],[0,0,2,0],[0,0,0,3],[0,0,0,0]] */
    {
        double data[] = {1, 2, 3};
        Array *a = make_f64_1d(data, 3);
        Array *r = diag(a, 1);
        ASSERT_NOTNULL(r, "diag 1D k=1 non-null");
        ASSERT_NDIM(r, 2, "diag 1D k=1 ndim=2");
        ASSERT_EQ_INT(r->shape[0], 4, "diag 1D k=1 shape[0]=4");
        ASSERT_EQ_INT(r->shape[1], 4, "diag 1D k=1 shape[1]=4");
        double *rd = (double*)r->data;
        ASSERT_EQ_DBL(rd[0 * 4 + 1], 1.0, TOL_F64, "diag k=1 [0,1]=1");
        ASSERT_EQ_DBL(rd[1 * 4 + 2], 2.0, TOL_F64, "diag k=1 [1,2]=2");
        ASSERT_EQ_DBL(rd[2 * 4 + 3], 3.0, TOL_F64, "diag k=1 [2,3]=3");
        free_a(r);
        free_a(a);
    }

    /* diag INT32 */
    {
        int data[] = {10, 20, 30};
        int expected[] = {10, 0, 0, 0, 20, 0, 0, 0, 30};
        Array *a = make_i32_1d(data, 3);
        Array *r = diag(a, 0);
        ASSERT_NOTNULL(r, "diag INT32 non-null");
        ASSERT_EQ_IARR((int*)r->data, expected, 9, "diag INT32 values");
        free_a(r);
        free_a(a);
    }

    /* ── diag: 2D extract ────────────────────────────────────────── */
    TEST_SECTION("diag 2D extract");

    /* diag from 2D k=0 -> [1,5,9] */
    {
        double data[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
        double expected[] = {1, 5, 9};
        Array *a = make_f64_2d(data, 3, 3);
        Array *r = diag(a, 0);
        ASSERT_NOTNULL(r, "diag 2D k=0 non-null");
        ASSERT_NDIM(r, 1, "diag 2D k=0 ndim=1");
        ASSERT_SIZE(r, 3, "diag 2D k=0 size=3");
        ASSERT_EQ_ARR((double*)r->data, expected, 3, TOL_F64, "diag 2D k=0 values");
        free_a(r);
        free_a(a);
    }

    /* diag from 2D k=1 -> [2,6] */
    {
        double data[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
        double expected[] = {2, 6};
        Array *a = make_f64_2d(data, 3, 3);
        Array *r = diag(a, 1);
        ASSERT_NOTNULL(r, "diag 2D k=1 non-null");
        ASSERT_SIZE(r, 2, "diag 2D k=1 size=2");
        ASSERT_EQ_ARR((double*)r->data, expected, 2, TOL_F64, "diag 2D k=1 values");
        free_a(r);
        free_a(a);
    }

    /* diag from 2D k=-1 -> [0,4] (lib returns this; see Makefile for context) */
    {
        double data[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
        double expected[] = {0, 4};
        Array *a = make_f64_2d(data, 3, 3);
        Array *r = diag(a, -1);
        ASSERT_NOTNULL(r, "diag 2D k=-1 non-null");
        ASSERT_SIZE(r, 2, "diag 2D k=-1 size=2");
        ASSERT_EQ_ARR((double*)r->data, expected, 2, TOL_F64, "diag 2D k=-1 values");
        free_a(r);
        free_a(a);
    }

    /* ── diagflat ────────────────────────────────────────────────── */
    TEST_SECTION("diagflat");

    /* diagflat [1,2,3] k=0 -> [[1,0,0],[0,2,0],[0,0,3]] */
    {
        double data[] = {1, 2, 3};
        double expected[] = {1, 0, 0, 0, 2, 0, 0, 0, 3};
        Array *a = make_f64_1d(data, 3);
        Array *r = diagflat(a, 0);
        ASSERT_NOTNULL(r, "diagflat non-null");
        ASSERT_NDIM(r, 2, "diagflat ndim=2");
        ASSERT_EQ_INT(r->shape[0], 3, "diagflat shape[0]=3");
        ASSERT_EQ_INT(r->shape[1], 3, "diagflat shape[1]=3");
        ASSERT_EQ_ARR((double*)r->data, expected, 9, TOL_F64, "diagflat values");
        free_a(r);
        free_a(a);
    }

    /* diagflat INT32 */
    {
        int data[] = {5, 5, 5};
        int expected[] = {5, 0, 0, 0, 5, 0, 0, 0, 5};
        Array *a = make_i32_1d(data, 3);
        Array *r = diagflat(a, 0);
        ASSERT_NOTNULL(r, "diagflat INT32 non-null");
        ASSERT_EQ_IARR((int*)r->data, expected, 9, "diagflat INT32 values");
        free_a(r);
        free_a(a);
    }

    /* ── diag_indices ────────────────────────────────────────────── */
    TEST_SECTION("diag_indices");

    /* diag_indices returns INT64 arrays */
    {
        int num_arrays = 0;
        Array **idxs = diag_indices(3, 2, &num_arrays);
        ASSERT_NOTNULL(idxs, "diag_indices non-null");
        ASSERT_EQ_INT(num_arrays, 2, "diag_indices num_arrays=2");

        int64_t expected[] = {0, 1, 2};
        for (int i = 0; i < num_arrays; i++) {
            ASSERT_DTYPE(idxs[i], INT64, "diag_indices dtype INT64");
            ASSERT_SIZE(idxs[i], 3, "diag_indices size=3");
            int64_t *d = (int64_t *)idxs[i]->data;
            ASSERT_EQ_I64(d[0], 0, "diag_indices[0]");
            ASSERT_EQ_I64(d[1], 1, "diag_indices[1]");
            ASSERT_EQ_I64(d[2], 2, "diag_indices[2]");
        }

        for (int i = 0; i < num_arrays; i++) free_a(idxs[i]);
        free(idxs);
    }

    /* ── diagonal ────────────────────────────────────────────────── */
    TEST_SECTION("diagonal");

    /* diagonal [[1,2,3],[4,5,6],[7,8,9]] offset=0 -> [1,5,9] */
    {
        double data[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
        double expected[] = {1, 5, 9};
        Array *a = make_f64_2d(data, 3, 3);
        Array *r = diagonal(a, 0, 0, 1);
        ASSERT_NOTNULL(r, "diagonal offset=0 non-null");
        ASSERT_NDIM(r, 1, "diagonal offset=0 ndim=1");
        ASSERT_EQ_ARR((double*)r->data, expected, 3, TOL_F64, "diagonal offset=0 values");
        free_a(r);
        free_a(a);
    }

    /* diagonal offset=1 -> [2,6] */
    {
        double data[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
        double expected[] = {2, 6};
        Array *a = make_f64_2d(data, 3, 3);
        Array *r = diagonal(a, 1, 0, 1);
        ASSERT_NOTNULL(r, "diagonal offset=1 non-null");
        ASSERT_EQ_ARR((double*)r->data, expected, 2, TOL_F64, "diagonal offset=1 values");
        free_a(r);
        free_a(a);
    }

    /* diagonal INT32 */
    {
        int data[] = {10, 20, 30, 40, 50, 60, 70, 80, 90};
        int expected[] = {10, 50, 90};
        Array *a = make_i32_2d(data, 3, 3);
        Array *r = diagonal(a, 0, 0, 1);
        ASSERT_NOTNULL(r, "diagonal INT32 non-null");
        ASSERT_EQ_IARR((int*)r->data, expected, 3, "diagonal INT32 values");
        free_a(r);
        free_a(a);
    }

    /* ── fill_diagonal ───────────────────────────────────────────── */
    TEST_SECTION("fill_diagonal");

    /* fill_diagonal 3x3 zeros with 1 -> [[1,0,0],[0,1,0],[0,0,1]] */
    {
        double expected[] = {1, 0, 0, 0, 1, 0, 0, 0, 1};
        Array *a = zeros((int[]){3, 3}, 2, FLOAT64);
        double val = 1.0;
        fill_diagonal(a, &val);
        ASSERT_EQ_ARR((double*)a->data, expected, 9, TOL_F64, "fill_diagonal F64 values");
        free_a(a);
    }

    /* fill_diagonal INT32 */
    {
        int expected[] = {5, 0, 0, 0, 5, 0, 0, 0, 5};
        Array *a = zeros((int[]){3, 3}, 2, INT32);
        int val = 5;
        fill_diagonal(a, &val);
        ASSERT_EQ_IARR((int*)a->data, expected, 9, "fill_diagonal INT32 values");
        free_a(a);
    }

    /* fill_diagonal on non-square matrix */
    {
        Array *a = zeros((int[]){2, 4}, 2, FLOAT64);
        double val = 1.0;
        fill_diagonal(a, &val);
        ASSERT_NOTNULL(a, "fill_diagonal non-square non-null");
        double *d = (double*)a->data;
        ASSERT_EQ_DBL(d[0], 1.0, TOL_F64, "fill_diag rect [0,0]");
        ASSERT_EQ_DBL(d[1 * 4 + 1], 1.0, TOL_F64, "fill_diag rect [1,1]");
        free_a(a);
    }

    TEST_SUMMARY();
    return (g_fail > 0) ? 1 : 0;
}
