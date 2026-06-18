#include "test_utils.h"


int main(void) {
    TEST_MAIN("test_kron");

    /* ── kron 1D x 1D ────────────────────────────────────────────── */
    TEST_SECTION("kron 1D");

    /* kron [1,2] and [3,4] -> [3,4,6,8] */
    {
        double d1[] = {1, 2};
        double d2[] = {3, 4};
        double expected[] = {3, 4, 6, 8};
        Array *a = make_f64_1d(d1, 2);
        Array *b = make_f64_1d(d2, 2);
        Array *r = kron(a, b);
        ASSERT_NOTNULL(r, "kron 1D non-null");
        ASSERT_NDIM(r, 1, "kron 1D ndim=1");
        ASSERT_SIZE(r, 4, "kron 1D size=4");
        ASSERT_EQ_ARR((double*)r->data, expected, 4, TOL_F64, "kron 1D values");
        free_a(r);
        free_a(a); free_a(b);
    }

    /* kron INT32 1D (promoted to FLOAT64) */
    {
        int d1[] = {1, 2};
        int d2[] = {3, 4};
        double expected[] = {3, 4, 6, 8};
        Array *a = make_i32_1d(d1, 2);
        Array *b = make_i32_1d(d2, 2);
        Array *r = kron(a, b);
        ASSERT_NOTNULL(r, "kron INT32 1D non-null");
        ASSERT_DTYPE(r, FLOAT64, "kron INT32 1D dtype=FLOAT64");
        ASSERT_EQ_ARR((double*)r->data, expected, 4, TOL_F64, "kron INT32 1D values");
        free_a(r);
        free_a(a); free_a(b);
    }

    /* kron [1] and [2,3,4] -> [2,3,4] */
    {
        double d1[] = {1};
        double d2[] = {2, 3, 4};
        double expected[] = {2, 3, 4};
        Array *a = make_f64_1d(d1, 1);
        Array *b = make_f64_1d(d2, 3);
        Array *r = kron(a, b);
        ASSERT_NOTNULL(r, "kron scalar x 1D non-null");
        ASSERT_SIZE(r, 3, "kron scalar x 1D size=3");
        ASSERT_EQ_ARR((double*)r->data, expected, 3, TOL_F64, "kron scalar x 1D values");
        free_a(r);
        free_a(a); free_a(b);
    }

    /* ── kron 2D x 2D ────────────────────────────────────────────── */
    TEST_SECTION("kron 2D");

    /* kron [[1,0],[0,1]] and [[2,3],[4,5]]
       -> [[2,3,0,0],[4,5,0,0],[0,0,2,3],[0,0,4,5]] */
    {
        double d1[] = {1, 0, 0, 1}; /* identity 2x2 */
        double d2[] = {2, 3, 4, 5};
        double expected[] = {
            2, 3, 0, 0,
            4, 5, 0, 0,
            0, 0, 2, 3,
            0, 0, 4, 5
        };
        Array *a = make_f64_2d(d1, 2, 2);
        Array *b = make_f64_2d(d2, 2, 2);
        Array *r = kron(a, b);
        ASSERT_NOTNULL(r, "kron 2D identity non-null");
        ASSERT_NDIM(r, 2, "kron 2D ndim=2");
        ASSERT_EQ_INT(r->shape[0], 4, "kron 2D shape[0]=4");
        ASSERT_EQ_INT(r->shape[1], 4, "kron 2D shape[1]=4");
        ASSERT_EQ_ARR((double*)r->data, expected, 16, TOL_F64, "kron 2D identity values");
        free_a(r);
        free_a(a); free_a(b);
    }

    /* kron INT32 2D (promoted to FLOAT64) */
    {
        int d1[] = {1, 2};
        int d2[] = {10, 20, 30, 40};
        double expected[] = {
            10, 20, 20, 40,
            30, 40, 60, 80
        };
        Array *a = make_i32_2d(d1, 1, 2);
        Array *b = make_i32_2d(d2, 2, 2);
        Array *r = kron(a, b);
        ASSERT_NOTNULL(r, "kron INT32 2D non-null");
        ASSERT_DTYPE(r, FLOAT64, "kron INT32 2D dtype=FLOAT64");
        ASSERT_NDIM(r, 2, "kron INT32 2D ndim=2");
        ASSERT_EQ_INT(r->shape[0], 2, "kron INT32 2D shape[0]=2");
        ASSERT_EQ_INT(r->shape[1], 4, "kron INT32 2D shape[1]=4");
        ASSERT_EQ_ARR((double*)r->data, expected, 8, TOL_F64, "kron INT32 2D values");
        free_a(r);
        free_a(a); free_a(b);
    }

    /* ── kron 1D x 2D ────────────────────────────────────────────── */
    TEST_SECTION("kron mixed");

    /* kron [1,2] and [[3,4],[5,6]]
       -> [1*[[3,4],[5,6]] | 2*[[3,4],[5,6]]]
       -> [[3,4,6,8],[5,6,10,12]] */
    {
        double d1[] = {1, 2};
        double d2[] = {3, 4, 5, 6};
        double expected[] = {
            3, 4, 6, 8,
            5, 6, 10, 12
        };
        Array *a = make_f64_1d(d1, 2);
        Array *b = make_f64_2d(d2, 2, 2);
        Array *r = kron(a, b);
        ASSERT_NOTNULL(r, "kron 1Dx2D non-null");
        ASSERT_NDIM(r, 2, "kron 1Dx2D ndim=2");
        ASSERT_EQ_INT(r->shape[0], 2, "kron 1Dx2D shape[0]=2");
        ASSERT_EQ_INT(r->shape[1], 4, "kron 1Dx2D shape[1]=4");
        ASSERT_EQ_ARR((double*)r->data, expected, 8, TOL_F64, "kron 1Dx2D values");
        free_a(r);
        free_a(a); free_a(b);
    }

    /* ── kron edge cases ─────────────────────────────────────────── */
    TEST_SECTION("kron edge");

    /* kron with different sizes */
    {
        double d1[] = {1, 2, 3};
        double d2[] = {10, 20};
        double expected[] = {10, 20, 20, 40, 30, 60};
        Array *a = make_f64_1d(d1, 3);
        Array *b = make_f64_1d(d2, 2);
        Array *r = kron(a, b);
        ASSERT_NOTNULL(r, "kron 3x2 non-null");
        ASSERT_SIZE(r, 6, "kron 3x2 size=6");
        ASSERT_EQ_ARR((double*)r->data, expected, 6, TOL_F64, "kron 3x2 values");
        free_a(r);
        free_a(a); free_a(b);
    }

    /* kron with zeros */
    {
        double d1[] = {0, 1, 0};
        double d2[] = {5, 5};
        double expected[] = {0, 0, 5, 5, 0, 0};
        Array *a = make_f64_1d(d1, 3);
        Array *b = make_f64_1d(d2, 2);
        Array *r = kron(a, b);
        ASSERT_NOTNULL(r, "kron with zero non-null");
        ASSERT_SIZE(r, 6, "kron with zero size=6");
        ASSERT_EQ_ARR((double*)r->data, expected, 6, TOL_F64, "kron with zero values");
        free_a(r);
        free_a(a); free_a(b);
    }

    TEST_SUMMARY();
    return (g_fail > 0) ? 1 : 0;
}
