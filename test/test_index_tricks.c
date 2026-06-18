/**
 * test_index_tricks.c — Tests for ix_, fill_diagonal
 */
#include "test_utils.h"

int main(void) {
    TEST_MAIN("ix_ / fill_diagonal");

    /* ── ix_ with two arrays ────────────────────────────────────────── */
    {
        Array *a = make_i32_1d((int[]){1, 2}, 2);
        Array *b = make_i32_1d((int[]){3, 4, 5}, 3);
        Array *in[] = {a, b};
        Array **out = NULL;
        int ret = ix_(in, 2, &out);
        ASSERT_EQ_INT(ret, 0, "ix_ returns 0");
        ASSERT_NOTNULL(out, "ix_ out not NULL");
        if (out) {
            /* out[0] should have shape (2, 1) and out[1] should have shape (1, 3) */
            ASSERT_NOTNULL(out[0], "ix_ out[0] not NULL");
            ASSERT_NOTNULL(out[1], "ix_ out[1] not NULL");
            if (out[0] && out[1]) {
                ASSERT_NDIM(out[0], 2, "ix_ out[0] ndim=2");
                ASSERT_NDIM(out[1], 2, "ix_ out[1] ndim=2");
                ASSERT_EQ_INT(out[0]->shape[0], 2, "ix_ out[0] shape[0]=2");
                ASSERT_EQ_INT(out[0]->shape[1], 1, "ix_ out[0] shape[1]=1");
                ASSERT_EQ_INT(out[1]->shape[0], 1, "ix_ out[1] shape[0]=1");
                ASSERT_EQ_INT(out[1]->shape[1], 3, "ix_ out[1] shape[1]=3");
                /* out[0] should be [[1],[2]] */
                ASSERT_EQ_INT(((int*)out[0]->data)[0], 1, "ix_ out[0][0,0]=1");
                ASSERT_EQ_INT(((int*)out[0]->data)[1], 2, "ix_ out[0][1,0]=2");
                /* out[1] should be [[3,4,5]] */
                ASSERT_EQ_INT(((int*)out[1]->data)[0], 3, "ix_ out[1][0,0]=3");
                ASSERT_EQ_INT(((int*)out[1]->data)[1], 4, "ix_ out[1][0,1]=4");
                ASSERT_EQ_INT(((int*)out[1]->data)[2], 5, "ix_ out[1][0,2]=5");
            }
            free_a(out[0]); free_a(out[1]);
            free(out);
        }
        free_a(a); free_a(b);
    }

    /* ── ix_ with three arrays ──────────────────────────────────────── */
    {
        Array *a = make_i32_1d((int[]){0, 1}, 2);
        Array *b = make_i32_1d((int[]){2, 3}, 2);
        Array *c = make_i32_1d((int[]){4, 5}, 2);
        Array *in[] = {a, b, c};
        Array **out = NULL;
        int ret = ix_(in, 3, &out);
        ASSERT_EQ_INT(ret, 0, "ix_ 3 arrays returns 0");
        ASSERT_NOTNULL(out, "ix_ 3 arrays out not NULL");
        if (out) {
            ASSERT_NOTNULL(out[0], "ix_ out[0] not NULL");
            ASSERT_NOTNULL(out[1], "ix_ out[1] not NULL");
            ASSERT_NOTNULL(out[2], "ix_ out[2] not NULL");
            for (int i = 0; i < 3; i++) free_a(out[i]);
            free(out);
        }
        free_a(a); free_a(b); free_a(c);
    }

    /* ── ix_ with single array ──────────────────────────────────────── */
    {
        Array *a = make_i32_1d((int[]){1, 2, 3}, 3);
        Array *in[] = {a};
        Array **out = NULL;
        int ret = ix_(in, 1, &out);
        ASSERT_EQ_INT(ret, 0, "ix_ single array returns 0");
        if (out) {
            ASSERT_NOTNULL(out[0], "ix_ single out[0] not NULL");
            free_a(out[0]); free(out);
        }
        free_a(a);
    }

    /* ── fill_diagonal 3x3 zeros ────────────────────────────────────── */
    {
        int shape[] = {3, 3};
        Array *a = zeros(shape, 2, INT32);
        ASSERT_NOTNULL(a, "zeros 3x3 created");
        if (a) {
            int val = 42;
            fill_diagonal(a, &val);
            int *d = (int*)a->data;
            ASSERT_EQ_INT(d[0], 42, "fill_diagonal [0,0]=42");
            ASSERT_EQ_INT(d[1], 0,  "fill_diagonal [0,1]=0");
            ASSERT_EQ_INT(d[2], 0,  "fill_diagonal [0,2]=0");
            ASSERT_EQ_INT(d[3], 0,  "fill_diagonal [1,0]=0");
            ASSERT_EQ_INT(d[4], 42, "fill_diagonal [1,1]=42");
            ASSERT_EQ_INT(d[5], 0,  "fill_diagonal [1,2]=0");
            ASSERT_EQ_INT(d[6], 0,  "fill_diagonal [2,0]=0");
            ASSERT_EQ_INT(d[7], 0,  "fill_diagonal [2,1]=0");
            ASSERT_EQ_INT(d[8], 42, "fill_diagonal [2,2]=42");
        }
        free_a(a);
    }

    /* ── fill_diagonal 3x3 FLOAT64 ──────────────────────────────────── */
    {
        int shape[] = {3, 3};
        Array *a = zeros(shape, 2, FLOAT64);
        ASSERT_NOTNULL(a, "zeros 3x3 FLOAT64 created");
        if (a) {
            double val = 3.14;
            fill_diagonal(a, &val);
            double *d = (double*)a->data;
            ASSERT_EQ_DBL(d[0], 3.14, TOL_F64, "fill_diagonal FLOAT64 [0,0]");
            ASSERT_EQ_DBL(d[4], 3.14, TOL_F64, "fill_diagonal FLOAT64 [1,1]");
            ASSERT_EQ_DBL(d[8], 3.14, TOL_F64, "fill_diagonal FLOAT64 [2,2]");
        }
        free_a(a);
    }

    /* ── fill_diagonal 4x3 (non-square) ─────────────────────────────── */
    {
        int shape[] = {4, 3};
        Array *a = zeros(shape, 2, INT32);
        ASSERT_NOTNULL(a, "zeros 4x3 created");
        if (a) {
            int val = 7;
            fill_diagonal(a, &val);
            ASSERT_TRUE(1, "fill_diagonal non-square (no crash)");
        }
        free_a(a);
    }

    TEST_SUMMARY();
    return (g_fail > 0) ? 1 : 0;
}
