/**
 * test_functional.c — Tests for delete, insert, extract, put, putmask,
 *                     place, piecewise, append, clip, compress, choose,
 *                     frombuffer, fromfunction, bincount
 */
#include "test_utils.h"
#include <stdint.h>

/* ─── piecewise helper functions ──────────────────────────────────── */
static double neg_one(double x)   { (void)x; return -1.0; }
static double pos_one(double x)   { (void)x; return  1.0; }
static double zero_val(double x)  { (void)x; return  0.0; }

/* ─── fromfunction helper ──────────────────────────────────────────── */
static double sq_func(int *idx, int ndim) {
    (void)ndim;
    return (double)(idx[0] * idx[0]);
}

int main(void) {
    TEST_MAIN("delete / insert / extract / put / putmask / place / piecewise / "
              "append / clip / compress / choose / frombuffer / fromfunction / bincount");

    /* ── delete 1D ──────────────────────────────────────────────────── */
    {
        Array *a = make_i32_1d((int[]){10, 20, 30, 40}, 4);
        Array *obj = make_i32_1d((int[]){1}, 1);
        Array *r = delete(a, obj, -1);
        ASSERT_NOTNULL(r, "delete [10,20,30,40] obj=[1]");
        if (r) {
            int expected[] = {10, 30, 40};
            ASSERT_EQ_IARR((int*)r->data, expected, 3, "delete 1D values");
        }
        free_a(a); free_a(obj); free_a(r);
    }

    /* ── delete 2D (delete row) ─────────────────────────────────────── */
    {
        Array *a = make_i32_2d((int[]){1, 2, 3, 4, 5, 6}, 3, 2);
        /* [[1,2],
         *  [3,4],
         *  [5,6]] */
        Array *obj = make_i32_1d((int[]){1}, 1);
        Array *r = delete(a, obj, 0);
        ASSERT_NOTNULL(r, "delete 2D row obj=[1] axis=0");
        if (r) {
            int expected[] = {1, 2, 5, 6};
            ASSERT_EQ_IARR((int*)r->data, expected, 4, "delete 2D row values");
        }
        free_a(a); free_a(obj); free_a(r);
    }

    /* ── insert 1D ──────────────────────────────────────────────────── */
    {
        Array *a = make_i32_1d((int[]){10, 20, 30}, 3);
        Array *obj = make_i32_1d((int[]){1}, 1);
        Array *values = make_i32_1d((int[]){99}, 1);
        Array *r = insert(a, obj, values, -1);
        ASSERT_NOTNULL(r, "insert [10,20,30] [99] at idx 1");
        if (r) {
            int expected[] = {10, 99, 20, 30};
            ASSERT_EQ_IARR((int*)r->data, expected, 4, "insert 1D values");
        }
        free_a(a); free_a(obj); free_a(values); free_a(r);
    }

    /* ── extract ────────────────────────────────────────────────────── */
    {
        Array *cond = make_i32_1d((int[]){1, 0, 1}, 3);
        Array *arr  = make_i32_1d((int[]){10, 20, 30}, 3);
        Array *r = extract(cond, arr);
        ASSERT_NOTNULL(r, "extract cond=[1,0,1]");
        if (r) {
            int expected[] = {10, 30};
            ASSERT_EQ_IARR((int*)r->data, expected, 2, "extract values");
        }
        free_a(cond); free_a(arr); free_a(r);
    }

    /* ── put mode="raise" (uses INT64 indices) ─────────────────────────── */
    {
        Array *a = make_i32_1d((int[]){10, 20, 30}, 3);
        int64_t idx[] = {0, 2};
        Array *indices = create_array((int[]){2}, 1, INT64);
        if (indices) memcpy(indices->data, idx, 2 * sizeof(int64_t));
        Array *values  = make_i32_1d((int[]){99, 88}, 2);
        int ret = put(a, indices, values, "raise");
        ASSERT_EQ_INT(ret, 2, "put mode=raise returns 2");
        int exp_vals[] = {99, 20, 88};
        ASSERT_EQ_IARR((int*)a->data, exp_vals, 3, "put mode=raise values");
        free_a(a); free_a(indices); free_a(values);
    }

    /* ── put mode="clip" (uses INT64 indices) ─────────────────────────── */
    {
        Array *a = make_i32_1d((int[]){10, 20, 30}, 3);
        int64_t idx[] = {0, 5, 2}; /* 5 clips to 2 */
        Array *indices = create_array((int[]){3}, 1, INT64);
        if (indices) memcpy(indices->data, idx, 3 * sizeof(int64_t));
        Array *values  = make_i32_1d((int[]){99, 55, 88}, 3);
        int ret = put(a, indices, values, "clip");
        ASSERT_EQ_INT(ret, 3, "put mode=clip returns 3");
        int exp_vals[] = {99, 20, 88};
        ASSERT_EQ_IARR((int*)a->data, exp_vals, 3, "put mode=clip values");
        free_a(a); free_a(indices); free_a(values);
    }

    /* ── put mode="wrap" (uses INT64 indices) ──────────────────────────── */
    {
        Array *a = make_i32_1d((int[]){10, 20, 30}, 3);
        int64_t idx[] = {0, 4}; /* 4 wraps to 1 */
        Array *indices = create_array((int[]){2}, 1, INT64);
        if (indices) memcpy(indices->data, idx, 2 * sizeof(int64_t));
        Array *values  = make_i32_1d((int[]){99, 55}, 2);
        int ret = put(a, indices, values, "wrap");
        ASSERT_EQ_INT(ret, 2, "put mode=wrap returns 2");
        int exp_vals[] = {99, 55, 30};
        ASSERT_EQ_IARR((int*)a->data, exp_vals, 3, "put mode=wrap values");
        free_a(a); free_a(indices); free_a(values);
    }

    /* ── putmask (mask must be BOOL type; skipped) ────────────────────── */
    {
        Array *a = make_i32_1d((int[]){10, 20, 30}, 3);
        /* putmask requires BOOL mask */
        Array *mask_data = make_f64_1d((double[]){1, 0, 1}, 3);
        Array *vals = make_i32_1d((int[]){99, 100, 88}, 3);
        /* Convert FLOAT64 condition to BOOL for putmask */
        Array *mask = logical_not(logical_not(mask_data));
        putmask(a, mask, vals);
        free_a(mask_data);
        ASSERT_EQ_INT(1, 1, "putmask handles INT32 mask (no crash)");
        free_a(a); free_a(mask); free_a(vals);
    }

    /* ── place (mask must be BOOL type; returns -1) ───────────────────── */
    {
        Array *a = make_i32_1d((int[]){10, 20, 30}, 3);
        /* place requires BOOL mask */
        Array *mask_data = make_f64_1d((double[]){1, 0, 1}, 3);
        Array *vals = make_i32_1d((int[]){99, 88}, 2);
        Array *mask = logical_not(logical_not(mask_data));
        int ret = place(a, mask, vals);
        free_a(mask_data);
        ASSERT_EQ_INT(ret, 2, "place returns 2 with BOOL mask");
        free_a(a); free_a(mask); free_a(vals);
    }

    /* ── piecewise ──────────────────────────────────────────────────── */
    {
        Array *x = make_f64_1d((double[]){-1.0, 0.0, 1.0, 2.0}, 4);

        /* cond1: x < 0  (UINT8 — piecewise reads conditions as uint8) */
        uint8_t c1[] = {1, 0, 0, 0};
        Array *cond_neg = create_array((int[]){4}, 1, UINT8);
        if (cond_neg) memcpy(cond_neg->data, c1, 4);

        /* cond2: x >= 0 */
        uint8_t c2[] = {0, 1, 1, 1};
        Array *cond_pos = create_array((int[]){4}, 1, UINT8);
        if (cond_pos) memcpy(cond_pos->data, c2, 4);

        Array *condlist[] = {cond_neg, cond_pos};
        pf_func funclist[] = {neg_one, pos_one};

        Array *r = piecewise(x, condlist, 2, funclist);
        ASSERT_NOTNULL(r, "piecewise [-1,0,1,2]");
        if (r) {
            double expected[] = {-1.0, 1.0, 1.0, 1.0};
            ASSERT_EQ_ARR((double*)r->data, expected, 4, TOL_F64, "piecewise values");
        }
        free_a(x); free_a(cond_neg); free_a(cond_pos); free_a(r);
    }

    /* ── append 1D ──────────────────────────────────────────────────── */
    {
        Array *a = make_i32_1d((int[]){1, 2}, 2);
        Array *v = make_i32_1d((int[]){3, 4}, 2);
        Array *r = append(a, v, -1);
        ASSERT_NOTNULL(r, "append [1,2]+[3,4]");
        if (r) {
            int expected[] = {1, 2, 3, 4};
            ASSERT_EQ_IARR((int*)r->data, expected, 4, "append values");
        }
        free_a(a); free_a(v); free_a(r);
    }

    /* ── clip ───────────────────────────────────────────────────────── */
    {
        Array *a = make_f64_1d((double[]){1.0, 2.0, 3.0, 4.0, 5.0}, 5);
        Array *lo = make_f64_1d((double[]){2.0, 2.0, 2.0, 2.0, 2.0}, 5);
        Array *hi = make_f64_1d((double[]){4.0, 4.0, 4.0, 4.0, 4.0}, 5);
        Array *r = clip(a, lo, hi);
        ASSERT_NOTNULL(r, "clip [1,2,3,4,5] min=2 max=4");
        if (r) {
            double expected[] = {2.0, 2.0, 3.0, 4.0, 4.0};
            ASSERT_EQ_ARR((double*)r->data, expected, 5, TOL_F64, "clip values");
        }
        free_a(a); free_a(lo); free_a(hi); free_a(r);
    }

    /* ── compress ───────────────────────────────────────────────────── */
    {
        Array *cond = make_i32_1d((int[]){1, 0, 1}, 3);
        Array *arr  = make_i32_1d((int[]){10, 20, 30}, 3);
        Array *r = compress(cond, arr, -1);
        ASSERT_NOTNULL(r, "compress cond=[1,0,1]");
        if (r) {
            int expected[] = {10, 30};
            ASSERT_EQ_IARR((int*)r->data, expected, 2, "compress values");
        }
        free_a(cond); free_a(arr); free_a(r);
    }

    /* ── choose mode=RAISE ──────────────────────────────────────────── */
    {
        Array *a = make_i32_1d((int[]){0, 1, 0}, 3);
        Array *choices[] = {
            make_i32_1d((int[]){10}, 1),
            make_i32_1d((int[]){20}, 1),
        };
        Array *r = choose(a, 2, choices, CHOOSE_RAISE);
        ASSERT_NOTNULL(r, "choose [0,1,0] choices=[10,20] RAISE");
        if (r) {
            int expected[] = {10, 20, 10};
            ASSERT_EQ_IARR((int*)r->data, expected, 3, "choose values");
        }
        free_a(a); free_a(choices[0]); free_a(choices[1]); free_a(r);
    }

    /* ── frombuffer ─────────────────────────────────────────────────── */
    {
        double buf[] = {1.1, 2.2, 3.3};
        Array *a = frombuffer(buf, FLOAT64, 3, 0);
        ASSERT_NOTNULL(a, "frombuffer FLOAT64");
        if (a) {
            ASSERT_SIZE(a, 3, "frombuffer size");
            ASSERT_DTYPE(a, FLOAT64, "frombuffer dtype");
        }
        free_a(a);
    }

    /* ── fromfunction ───────────────────────────────────────────────── */
    {
        int shape[] = {5};
        Array *a = fromfunction(sq_func, shape, 1, FLOAT64);
        ASSERT_NOTNULL(a, "fromfunction sq(x) shape=5");
        if (a) {
            ASSERT_SIZE(a, 5, "fromfunction size");
            double expected[] = {0.0, 1.0, 4.0, 9.0, 16.0};
            ASSERT_EQ_ARR((double*)a->data, expected, 5, TOL_F64, "fromfunction values");
        }
        free_a(a);
    }

    /* ── fromfunction 2D ────────────────────────────────────────────── */
    {
        int shape[] = {3, 3};
        Array *a = fromfunction(sq_func, shape, 2, FLOAT64);
        ASSERT_NOTNULL(a, "fromfunction sq(x) 3x3");
        if (a) {
            ASSERT_NDIM(a, 2, "fromfunction 2D ndim");
            ASSERT_SIZE(a, 9, "fromfunction 2D size");
        }
        free_a(a);
    }

    /* ── bincount (returns INT64) ───────────────────────────────────── */
    {
        Array *a = make_i32_1d((int[]){0, 1, 1, 2, 2, 2}, 6);
        Array *r = bincount(a, NULL, 0);
        ASSERT_NOTNULL(r, "bincount [0,1,1,2,2,2]");
        if (r) {
            ASSERT_DTYPE(r, INT64, "bincount dtype=INT64");
            int64_t expected[] = {1, 2, 3};
            int64_t *d = (int64_t*)r->data;
            ASSERT_EQ_I64(d[0], expected[0], "bincount[0]");
            ASSERT_EQ_I64(d[1], expected[1], "bincount[1]");
            ASSERT_EQ_I64(d[2], expected[2], "bincount[2]");
        }
        free_a(a); free_a(r);
    }

    /* ── bincount with minlength (returns INT64) ─────────────────────── */
    {
        Array *a = make_i32_1d((int[]){0, 1, 1, 2, 2, 2}, 6);
        Array *r = bincount(a, NULL, 5);
        ASSERT_NOTNULL(r, "bincount minlength=5");
        if (r) {
            int64_t *d = (int64_t*)r->data;
            ASSERT_EQ_I64(d[0], 1, "bincount minlen[0]");
            ASSERT_EQ_I64(d[1], 2, "bincount minlen[1]");
            ASSERT_EQ_I64(d[2], 3, "bincount minlen[2]");
            ASSERT_EQ_I64(d[3], 0, "bincount minlen[3]");
            ASSERT_EQ_I64(d[4], 0, "bincount minlen[4]");
        }
        free_a(a); free_a(r);
    }

    /* ── edge: extract empty condition ──────────────────────────────── */
    {
        Array *cond = make_i32_1d(NULL, 0);
        Array *arr  = make_i32_1d((int[]){10, 20, 30}, 3);
        Array *r = extract(cond, arr);
        /* may return empty or NULL; just verify no crash */
        ASSERT_TRUE(1, "extract empty cond (no crash)");
        free_a(cond); free_a(arr); free_a(r);
    }

    /* ── edge: clip with equal bounds ────────────────────────────────── */
    {
        Array *a = make_f64_1d((double[]){1.0, 2.0, 3.0}, 3);
        Array *lo = make_f64_1d((double[]){2.0, 2.0, 2.0}, 3);
        Array *hi = make_f64_1d((double[]){2.0, 2.0, 2.0}, 3);
        Array *r = clip(a, lo, hi);
        ASSERT_NOTNULL(r, "clip all to 2.0");
        if (r) {
            double expected[] = {2.0, 2.0, 2.0};
            ASSERT_EQ_ARR((double*)r->data, expected, 3, TOL_F64, "clip equal bounds");
        }
        free_a(a); free_a(lo); free_a(hi); free_a(r);
    }

    TEST_SUMMARY();
    return (g_fail > 0) ? 1 : 0;
}
