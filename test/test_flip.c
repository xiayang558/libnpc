/**
 * test_flip.c — Tests for flip, fliplr, flipud
 */
#include "test_utils.h"

int main(void) {
    TEST_MAIN("flip / fliplr / flipud");

    /* ── flip 1D axis=0 ─────────────────────────────────────────────── */
    {
        Array *a = make_i32_1d((int[]){1, 2, 3, 4}, 4);
        Array *r = flip(a, 0);
        ASSERT_NOTNULL(r, "flip [1,2,3,4] axis=0");
        if (r) {
            int expected[] = {4, 3, 2, 1};
            ASSERT_EQ_IARR((int*)r->data, expected, 4, "flip 1D values");
        }
        free_a(a); free_a(r);
    }

    /* ── flip 2D axis=0 (vertical) ──────────────────────────────────── */
    {
        Array *a = make_i32_2d((int[]){1, 2, 3, 4}, 2, 2);
        Array *r = flip(a, 0);
        ASSERT_NOTNULL(r, "flip 2D axis=0");
        if (r) {
            int expected[] = {3, 4, 1, 2};
            ASSERT_EQ_IARR((int*)r->data, expected, 4, "flip 2D axis=0 values");
        }
        free_a(a); free_a(r);
    }

    /* ── flip 2D axis=1 (horizontal) ────────────────────────────────── */
    {
        Array *a = make_i32_2d((int[]){1, 2, 3, 4}, 2, 2);
        Array *r = flip(a, 1);
        ASSERT_NOTNULL(r, "flip 2D axis=1");
        if (r) {
            int expected[] = {2, 1, 4, 3};
            ASSERT_EQ_IARR((int*)r->data, expected, 4, "flip 2D axis=1 values");
        }
        free_a(a); free_a(r);
    }

    /* ── fliplr ─────────────────────────────────────────────────────── */
    {
        Array *a = make_i32_2d((int[]){1, 2, 3, 4, 5, 6}, 2, 3);
        Array *r = fliplr(a);
        ASSERT_NOTNULL(r, "fliplr 2x3");
        if (r) {
            int expected[] = {3, 2, 1, 6, 5, 4};
            ASSERT_EQ_IARR((int*)r->data, expected, 6, "fliplr values");
        }
        free_a(a); free_a(r);
    }

    /* ── flipud ─────────────────────────────────────────────────────── */
    {
        Array *a = make_i32_2d((int[]){1, 2, 3, 4, 5, 6}, 2, 3);
        Array *r = flipud(a);
        ASSERT_NOTNULL(r, "flipud 2x3");
        if (r) {
            int expected[] = {4, 5, 6, 1, 2, 3};
            ASSERT_EQ_IARR((int*)r->data, expected, 6, "flipud values");
        }
        free_a(a); free_a(r);
    }

    /* ── fliplr 1D (not supported, returns NULL) ──────────────────────── */
    {
        Array *a = make_i32_1d((int[]){1, 2, 3, 4}, 4);
        Array *r = fliplr(a);
        ASSERT_NULL(r, "fliplr 1D returns NULL (requires 2D)");
        free_a(a);
    }

    /* ── flipud 2D ──────────────────────────────────────────────────── */
    {
        Array *a = make_f64_2d((double[]){1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0}, 3, 3);
        Array *r = flipud(a);
        ASSERT_NOTNULL(r, "flipud FLOAT64 3x3");
        if (r) {
            ASSERT_NDIM(r, 2, "flipud ndim");
            ASSERT_SIZE(r, 9, "flipud size");
            double *d = (double*)r->data;
            double expected[] = {7.0, 8.0, 9.0, 4.0, 5.0, 6.0, 1.0, 2.0, 3.0};
            ASSERT_EQ_ARR(d, expected, 9, TOL_F64, "flipud FLOAT64 values");
        }
        free_a(a); free_a(r);
    }

    /* ── flip FLOAT64 1D ────────────────────────────────────────────── */
    {
        Array *a = make_f64_1d((double[]){1.5, 2.5, 3.5}, 3);
        Array *r = flip(a, 0);
        ASSERT_NOTNULL(r, "flip FLOAT64 1D");
        if (r) {
            double expected[] = {3.5, 2.5, 1.5};
            ASSERT_EQ_ARR((double*)r->data, expected, 3, TOL_F64, "flip FLOAT64 values");
        }
        free_a(a); free_a(r);
    }

    /* ── flip INT32 ─────────────────────────────────────────────────── */
    {
        Array *a = make_i32_1d((int[]){10, 20, 30}, 3);
        Array *r = flip(a, 0);
        ASSERT_NOTNULL(r, "flip INT32 1D");
        if (r) {
            int expected[] = {30, 20, 10};
            ASSERT_EQ_IARR((int*)r->data, expected, 3, "flip INT32 values");
        }
        free_a(a); free_a(r);
    }

    /* ── fliplr 3x3 INT32 ───────────────────────────────────────────── */
    {
        Array *a = make_i32_2d((int[]){1, 2, 3, 4, 5, 6, 7, 8, 9}, 3, 3);
        Array *r = fliplr(a);
        ASSERT_NOTNULL(r, "fliplr INT32 3x3");
        if (r) {
            int expected[] = {3, 2, 1, 6, 5, 4, 9, 8, 7};
            ASSERT_EQ_IARR((int*)r->data, expected, 9, "fliplr INT32 3x3 values");
        }
        free_a(a); free_a(r);
    }

    /* ── flip 2D FLOAT64 axis=1 ─────────────────────────────────────── */
    {
        Array *a = make_f64_2d((double[]){1.0, 2.0, 3.0, 4.0}, 2, 2);
        Array *r = flip(a, 1);
        ASSERT_NOTNULL(r, "flip FLOAT64 2D axis=1");
        if (r) {
            double expected[] = {2.0, 1.0, 4.0, 3.0};
            ASSERT_EQ_ARR((double*)r->data, expected, 4, TOL_F64, "flip FLOAT64 axis=1 values");
        }
        free_a(a); free_a(r);
    }

    TEST_SUMMARY();
    return (g_fail > 0) ? 1 : 0;
}
