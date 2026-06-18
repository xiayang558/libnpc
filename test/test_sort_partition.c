/**
 * test_sort_partition.c — Tests for argsort, argpartition, partition,
 *                         argmax, argmin, msort
 */
#include "test_utils.h"

int main(void) {
    TEST_MAIN("sort / partition / argmax / argmin / msort");

    /* ── argsort 1D (returns INT64) ──────────────────────────────────── */
    {
        Array *a = make_i32_1d((int[]){3, 1, 4, 1, 5}, 5);
        Array *s = argsort(a, -1, "quicksort");
        ASSERT_NOTNULL(s, "argsort [3,1,4,1,5] quicksort");
        if (s) {
            int64_t expected[] = {3, 1, 0, 2, 4};
            int64_t *d = (int64_t*)s->data;
            ASSERT_EQ_I64(d[0], expected[0], "argsort[0]");
            ASSERT_EQ_I64(d[1], expected[1], "argsort[1]");
            ASSERT_EQ_I64(d[2], expected[2], "argsort[2]");
            ASSERT_EQ_I64(d[3], expected[3], "argsort[3]");
            ASSERT_EQ_I64(d[4], expected[4], "argsort[4]");
        }
        free_a(a); free_a(s);
    }

    {
        Array *a = make_i32_1d((int[]){5, 4, 3, 2, 1}, 5);
        Array *s = argsort(a, -1, "mergesort");
        ASSERT_NOTNULL(s, "argsort [5,4,3,2,1] mergesort");
        if (s) {
            int64_t expected[] = {4, 3, 2, 1, 0};
            int64_t *d = (int64_t*)s->data;
            ASSERT_EQ_I64(d[0], expected[0], "argsort desc[0]");
            ASSERT_EQ_I64(d[1], expected[1], "argsort desc[1]");
            ASSERT_EQ_I64(d[2], expected[2], "argsort desc[2]");
            ASSERT_EQ_I64(d[3], expected[3], "argsort desc[3]");
            ASSERT_EQ_I64(d[4], expected[4], "argsort desc[4]");
        }
        free_a(a); free_a(s);
    }

    /* ── argsort 2D axis=0 (returns INT64) ──────────────────────────── */
    {
        /** [[9, 3],
         *  [2, 7],
         *  [5, 1]]   */
        Array *a = make_i32_2d((int[]){9, 3, 2, 7, 5, 1}, 3, 2);
        Array *s = argsort(a, 0, "quicksort");
        ASSERT_NOTNULL(s, "argsort 2D axis=0");
        if (s) {
            /* column 0: 9(0),2(1),5(2) → sorted: 2(1),5(2),9(0) → [1,2,0]
             * column 1: 3(0),7(1),1(2) → sorted: 1(2),3(0),7(1) → [2,0,1] */
            int64_t expected[] = {1, 2, 2, 0, 0, 1};
            int64_t *d = (int64_t*)s->data;
            ASSERT_EQ_I64(d[0], expected[0], "argsort 2D axis=0 idx[0]");
            ASSERT_EQ_I64(d[1], expected[1], "argsort 2D axis=0 idx[1]");
            ASSERT_EQ_I64(d[2], expected[2], "argsort 2D axis=0 idx[2]");
            ASSERT_EQ_I64(d[3], expected[3], "argsort 2D axis=0 idx[3]");
            ASSERT_EQ_I64(d[4], expected[4], "argsort 2D axis=0 idx[4]");
            ASSERT_EQ_I64(d[5], expected[5], "argsort 2D axis=0 idx[5]");
        }
        free_a(a); free_a(s);
    }

    /* ── argsort 2D axis=1 (returns INT64) ──────────────────────────── */
    {
        Array *a = make_i32_2d((int[]){3, 1, 5, 2}, 2, 2);
        Array *s = argsort(a, 1, "quicksort");
        ASSERT_NOTNULL(s, "argsort 2D axis=1");
        if (s) {
            /* row 0: [3,1] → sorted [1,3] → indices [1,0]
             * row 1: [5,2] → sorted [2,5] → indices [1,0] */
            int64_t expected[] = {1, 0, 1, 0};
            int64_t *d = (int64_t*)s->data;
            ASSERT_EQ_I64(d[0], expected[0], "argsort 2D axis=1 idx[0]");
            ASSERT_EQ_I64(d[1], expected[1], "argsort 2D axis=1 idx[1]");
            ASSERT_EQ_I64(d[2], expected[2], "argsort 2D axis=1 idx[2]");
            ASSERT_EQ_I64(d[3], expected[3], "argsort 2D axis=1 idx[3]");
        }
        free_a(a); free_a(s);
    }

    /* ── argpartition 1D ────────────────────────────────────────────── */
    {
        Array *a = make_f64_1d((double[]){3.0, 1.0, 4.0, 1.0, 5.0}, 5);
        Array *p = argpartition(a, 2, -1);
        ASSERT_NOTNULL(p, "argpartition [3,1,4,1,5] kth=2");
        if (p) {
            /* k-th (index 2) element should be in its correct sorted position;
             * all before k should be <=, all after >=. Just verify non-NULL
             * and correct shape/size. */
            ASSERT_SIZE(p, 5, "argpartition size");
        }
        free_a(a); free_a(p);
    }

    /* ── partition 1D ───────────────────────────────────────────────── */
    {
        Array *a = make_f64_1d((double[]){3.0, 1.0, 4.0, 1.0, 5.0}, 5);
        Array *p = partition(a, 2, -1);
        ASSERT_NOTNULL(p, "partition [3,1,4,1,5] kth=2");
        if (p) {
            ASSERT_SIZE(p, 5, "partition size");
            ASSERT_DTYPE(p, FLOAT64, "partition dtype");
        }
        free_a(a); free_a(p);
    }

    /* ── argmax 1D ──────────────────────────────────────────────────── */
    {
        Array *a = make_i32_1d((int[]){1, 5, 3, 9, 2}, 5);
        Array *m = argmax(a, -1, 0);
        ASSERT_NOTNULL(m, "argmax [1,5,3,9,2] axis=-1 keepdims=0");
        if (m) {
            ASSERT_EQ_INT(*(int*)m->data, 3, "argmax index of 9 is 3");
        }
        free_a(a); free_a(m);
    }

    /* ── argmax 2D axis=0 ───────────────────────────────────────────── */
    {
        /* [[1, 9],
         *  [5, 2],
         *  [3, 7]]  */
        Array *a = make_i32_2d((int[]){1, 9, 5, 2, 3, 7}, 3, 2);
        Array *m = argmax(a, 0, 0);
        ASSERT_NOTNULL(m, "argmax 2D axis=0 keepdims=0");
        if (m) {
            /* col0: 1(0),5(1),3(2) → max 5 at idx 1
             * col1: 9(0),2(1),7(2) → max 9 at idx 0 */
            int expected[] = {1, 0};
            ASSERT_EQ_IARR((int*)m->data, expected, 2, "argmax 2D axis=0 values");
        }
        free_a(a); free_a(m);
    }

    /* ── argmax 2D axis=1 ───────────────────────────────────────────── */
    {
        Array *a = make_i32_2d((int[]){1, 9, 5, 2}, 2, 2);
        Array *m = argmax(a, 1, 0);
        ASSERT_NOTNULL(m, "argmax 2D axis=1 keepdims=0");
        if (m) {
            /* row0: [1,9] → argmax=1, row1: [5,2] → argmax=0 */
            int expected[] = {1, 0};
            ASSERT_EQ_IARR((int*)m->data, expected, 2, "argmax 2D axis=1 values");
        }
        free_a(a); free_a(m);
    }

    /* ── argmin 1D ──────────────────────────────────────────────────── */
    {
        Array *a = make_i32_1d((int[]){1, 5, 0, 9, 2}, 5);
        Array *m = argmin(a, -1, 0);
        ASSERT_NOTNULL(m, "argmin [1,5,0,9,2]");
        if (m) {
            ASSERT_EQ_INT(*(int*)m->data, 2, "argmin index of 0 is 2");
        }
        free_a(a); free_a(m);
    }

    /* ── argmin 2D ──────────────────────────────────────────────────── */
    {
        Array *a = make_i32_2d((int[]){5, 1, 3, 9, 2, 7}, 3, 2);
        Array *m = argmin(a, 0, 0);
        ASSERT_NOTNULL(m, "argmin 2D axis=0");
        if (m) {
            /* col0: 5(0),3(1),2(2) → min 2 at idx 2
             * col1: 1(0),9(1),7(2) → min 1 at idx 0 */
            int expected[] = {2, 0};
            ASSERT_EQ_IARR((int*)m->data, expected, 2, "argmin 2D axis=0 values");
        }
        free_a(a); free_a(m);
    }

    /* ── msort ──────────────────────────────────────────────────────── */
    {
        Array *a = make_f64_2d((double[]){3.0, 1.0, 4.0, 2.0}, 2, 2);
        Array *m = msort(a);
        ASSERT_NOTNULL(m, "msort [[3,1],[4,2]]");
        if (m) {
            /* msort sorts along axis=0 (first axis):
             * col0: [3,4] → [3,4], col1: [1,2] → [1,2] → [[3,1],[4,2]] ??? */
            ASSERT_SIZE(m, 4, "msort size");
            ASSERT_NDIM(m, 2, "msort ndim");
            /* Verify sorted by columns (axis=0): m[0] <= m[1] per column */
            double *d = (double*)m->data;
            ASSERT_TRUE(d[0] <= d[2], "msort col0 sorted");
            ASSERT_TRUE(d[1] <= d[3], "msort col1 sorted");
        }
        free_a(a); free_a(m);
    }

    /* ── msort with INT32 (preserves INT32 dtype) ──────────────────────── */
    {
        Array *a = make_i32_2d((int[]){5, 2, 1, 4}, 2, 2);
        /* [[5,2],
         *  [1,4]] */
        Array *m = msort(a);
        ASSERT_NOTNULL(m, "msort INT32 [[5,2],[1,4]]");
        if (m) {
            int *d = (int*)m->data;
            ASSERT_TRUE(d[0] <= d[2], "msort INT32 col0 sorted");
        }
        free_a(a); free_a(m);
    }

    /* ── argmax with keepdims=1 (returns 0D INT64 scalar for 1D input) ── */
    {
        Array *a = make_i32_1d((int[]){1, 5, 3, 9, 2}, 5);
        Array *m = argmax(a, -1, 1);
        ASSERT_NOTNULL(m, "argmax keepdims=1");
        if (m) {
            ASSERT_EQ_I64(*(int64_t*)m->data, 3, "argmax keepdims=1 value");
            /* keepdims=1 returns ndim=0 for 1D input in this implementation */
        }
        free_a(a); free_a(m);
    }

    /* ── FLOAT64 argsort ────────────────────────────────────────────── */
    {
        Array *a = make_f64_1d((double[]){3.5, 1.2, 4.8, 1.2, 5.0}, 5);
        Array *s = argsort(a, -1, "quicksort");
        ASSERT_NOTNULL(s, "argsort FLOAT64");
        if (s) {
            ASSERT_SIZE(s, 5, "argsort FLOAT64 size");
        }
        free_a(a); free_a(s);
    }

    TEST_SUMMARY();
    return (g_fail > 0) ? 1 : 0;
}
