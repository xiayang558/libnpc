/**
 * test_array_create.c — Tests for array creation functions:
 *   create_array, empty, zeros, zeros_int, zeros_float, zeros_complex,
 *   ones, ones_int, ones_float, ones_complex,
 *   full, full_int, full_float, full_complex, free_array
 */

#include "test_utils.h"

int main(void) {
    TEST_MAIN("Array Creation");

    /* ── create_array ───────────────────────────────────────────── */
    TEST_SECTION("create_array");

    {
        /* 1D INT32 */
        int shape1[] = {5};
        Array *a = create_array(shape1, 1, INT32);
        ASSERT_NOTNULL(a, "create_array 1D INT32 not null");
        ASSERT_NDIM(a, 1, "create_array 1D ndim=1");
        ASSERT_SIZE(a, 5, "create_array 1D size=5");
        ASSERT_DTYPE(a, INT32, "create_array 1D dtype=INT32");
        a = free_a(a);
    }
    {
        /* 2D FLOAT64 */
        int shape2[] = {3, 4};
        Array *a = create_array(shape2, 2, FLOAT64);
        ASSERT_NOTNULL(a, "create_array 2D FLOAT64 not null");
        ASSERT_NDIM(a, 2, "create_array 2D ndim=2");
        ASSERT_SIZE(a, 12, "create_array 2D size=12");
        ASSERT_DTYPE(a, FLOAT64, "create_array 2D dtype=FLOAT64");
        a = free_a(a);
    }
    {
        /* create_array with ndim=0 returns a valid scalar array */
        Array *a = create_array(NULL, 0, FLOAT64);
        ASSERT_NOTNULL(a, "create_array 0D returns valid array");
        ASSERT_NDIM(a, 0, "create_array 0D ndim=0");
        ASSERT_SIZE(a, 1, "create_array 0D size=1");
        a = free_a(a);
    }

    /* ── empty ──────────────────────────────────────────────────── */
    TEST_SECTION("empty");

    {
        int shape[] = {10};
        Array *a = empty(shape, 1, FLOAT64);
        ASSERT_NOTNULL(a, "empty 1D FLOAT64 not null");
        ASSERT_NDIM(a, 1, "empty 1D ndim=1");
        ASSERT_SIZE(a, 10, "empty 1D size=10");
        ASSERT_DTYPE(a, FLOAT64, "empty 1D dtype=FLOAT64");
        a = free_a(a);
    }
    {
        int shape[] = {2, 5};
        Array *a = empty(shape, 2, INT32);
        ASSERT_NOTNULL(a, "empty 2D INT32 not null");
        ASSERT_NDIM(a, 2, "empty 2D ndim=2");
        ASSERT_SIZE(a, 10, "empty 2D size=10");
        ASSERT_DTYPE(a, INT32, "empty 2D dtype=INT32");
        a = free_a(a);
    }

    /* ── zeros ──────────────────────────────────────────────────── */
    TEST_SECTION("zeros");
    {
        int shape[] = {5};
        Array *a = zeros(shape, 1, FLOAT64);
        ASSERT_NOTNULL(a, "zeros 1D FLOAT64 not null");
        ASSERT_SIZE(a, 5, "zeros 1D FLOAT64 size=5");
        ASSERT_NDIM(a, 1, "zeros 1D FLOAT64 ndim=1");
        ASSERT_DTYPE(a, FLOAT64, "zeros 1D FLOAT64 dtype");
        double *d = (double *)a->data;
        double expected[] = {0.0, 0.0, 0.0, 0.0, 0.0};
        ASSERT_EQ_ARR(d, expected, 5, TOL_F64, "zeros 1D FLOAT64 all zero");
        a = free_a(a);
    }
    {
        int shape[] = {2, 3};
        Array *a = zeros(shape, 2, INT32);
        ASSERT_NOTNULL(a, "zeros 2D INT32 not null");
        ASSERT_SIZE(a, 6, "zeros 2D INT32 size=6");
        ASSERT_NDIM(a, 2, "zeros 2D INT32 ndim=2");
        ASSERT_DTYPE(a, INT32, "zeros 2D INT32 dtype");
        int *d = (int *)a->data;
        int expected[] = {0, 0, 0, 0, 0, 0};
        ASSERT_EQ_IARR(d, expected, 6, "zeros 2D INT32 all zero");
        a = free_a(a);
    }

    /* ── zeros_int ──────────────────────────────────────────────── */
    TEST_SECTION("zeros_int");
    {
        int shape[] = {4};
        Array *a = zeros_int(shape, 1);
        ASSERT_NOTNULL(a, "zeros_int not null");
        ASSERT_DTYPE(a, INT32, "zeros_int dtype=INT32");
        ASSERT_SIZE(a, 4, "zeros_int size=4");
        int *d = (int *)a->data;
        int expected[] = {0, 0, 0, 0};
        ASSERT_EQ_IARR(d, expected, 4, "zeros_int all zero");
        a = free_a(a);
    }

    /* ── zeros_float ────────────────────────────────────────────── */
    TEST_SECTION("zeros_float");
    {
        /* zeros_float returns FLOAT32 (4-byte float) */
        int shape[] = {3};
        Array *a = zeros_float(shape, 1);
        ASSERT_NOTNULL(a, "zeros_float not null");
        ASSERT_DTYPE(a, FLOAT32, "zeros_float dtype=FLOAT32");
        ASSERT_SIZE(a, 3, "zeros_float size=3");
        float *d = (float *)a->data;
        ASSERT_EQ_DBL(d[0], 0.0, TOL_F32, "zeros_float val[0]");
        ASSERT_EQ_DBL(d[1], 0.0, TOL_F32, "zeros_float val[1]");
        ASSERT_EQ_DBL(d[2], 0.0, TOL_F32, "zeros_float val[2]");
        a = free_a(a);
    }

    /* ── zeros_complex ──────────────────────────────────────────── */
    TEST_SECTION("zeros_complex");
    {
        /* zeros_complex returns COMPLEX64 (8-byte complex float) */
        int shape[] = {2};
        Array *a = zeros_complex(shape, 1);
        ASSERT_NOTNULL(a, "zeros_complex not null");
        ASSERT_DTYPE(a, COMPLEX64, "zeros_complex dtype=COMPLEX64");
        ASSERT_SIZE(a, 2, "zeros_complex size=2");
        complex float *d = (complex float *)a->data;
        ASSERT_EQ_DBL(crealf(d[0]), 0.0f, TOL_F32, "zeros_complex real[0]");
        ASSERT_EQ_DBL(cimagf(d[0]), 0.0f, TOL_F32, "zeros_complex imag[0]");
        ASSERT_EQ_DBL(crealf(d[1]), 0.0f, TOL_F32, "zeros_complex real[1]");
        ASSERT_EQ_DBL(cimagf(d[1]), 0.0f, TOL_F32, "zeros_complex imag[1]");
        a = free_a(a);
    }

    /* ── ones ───────────────────────────────────────────────────── */
    TEST_SECTION("ones");
    {
        int shape[] = {4};
        Array *a = ones(shape, 1, FLOAT64);
        ASSERT_NOTNULL(a, "ones 1D FLOAT64 not null");
        ASSERT_SIZE(a, 4, "ones 1D FLOAT64 size=4");
        ASSERT_DTYPE(a, FLOAT64, "ones 1D FLOAT64 dtype");
        double *d = (double *)a->data;
        double expected[] = {1.0, 1.0, 1.0, 1.0};
        ASSERT_EQ_ARR(d, expected, 4, TOL_F64, "ones 1D FLOAT64 all one");
        a = free_a(a);
    }
    {
        int shape[] = {2, 2};
        Array *a = ones(shape, 2, INT32);
        ASSERT_NOTNULL(a, "ones 2D INT32 not null");
        ASSERT_SIZE(a, 4, "ones 2D INT32 size=4");
        ASSERT_DTYPE(a, INT32, "ones 2D INT32 dtype");
        int *d = (int *)a->data;
        int expected[] = {1, 1, 1, 1};
        ASSERT_EQ_IARR(d, expected, 4, "ones 2D INT32 all one");
        a = free_a(a);
    }

    /* ── ones_int ───────────────────────────────────────────────── */
    TEST_SECTION("ones_int");
    {
        int shape[] = {5};
        Array *a = ones_int(shape, 1);
        ASSERT_NOTNULL(a, "ones_int not null");
        ASSERT_DTYPE(a, INT32, "ones_int dtype=INT32");
        ASSERT_SIZE(a, 5, "ones_int size=5");
        int *d = (int *)a->data;
        int expected[] = {1, 1, 1, 1, 1};
        ASSERT_EQ_IARR(d, expected, 5, "ones_int all one");
        a = free_a(a);
    }

    /* ── ones_float ─────────────────────────────────────────────── */
    TEST_SECTION("ones_float");
    {
        /* ones_float returns FLOAT32 (4-byte float) */
        int shape[] = {3};
        Array *a = ones_float(shape, 1);
        ASSERT_NOTNULL(a, "ones_float not null");
        ASSERT_DTYPE(a, FLOAT32, "ones_float dtype=FLOAT32");
        ASSERT_SIZE(a, 3, "ones_float size=3");
        float *d = (float *)a->data;
        ASSERT_EQ_DBL(d[0], 1.0, TOL_F32, "ones_float val[0]");
        ASSERT_EQ_DBL(d[1], 1.0, TOL_F32, "ones_float val[1]");
        ASSERT_EQ_DBL(d[2], 1.0, TOL_F32, "ones_float val[2]");
        a = free_a(a);
    }

    /* ── ones_complex ───────────────────────────────────────────── */
    TEST_SECTION("ones_complex");
    {
        /* ones_complex returns COMPLEX64 (8-byte complex float) */
        int shape[] = {2};
        Array *a = ones_complex(shape, 1);
        ASSERT_NOTNULL(a, "ones_complex not null");
        ASSERT_DTYPE(a, COMPLEX64, "ones_complex dtype=COMPLEX64");
        ASSERT_SIZE(a, 2, "ones_complex size=2");
        complex float *d = (complex float *)a->data;
        ASSERT_EQ_DBL(crealf(d[0]), 1.0f, TOL_F32, "ones_complex real[0]");
        ASSERT_EQ_DBL(cimagf(d[0]), 0.0f, TOL_F32, "ones_complex imag[0]");
        ASSERT_EQ_DBL(crealf(d[1]), 1.0f, TOL_F32, "ones_complex real[1]");
        ASSERT_EQ_DBL(cimagf(d[1]), 0.0f, TOL_F32, "ones_complex imag[1]");
        a = free_a(a);
    }

    /* ── full ───────────────────────────────────────────────────── */
    TEST_SECTION("full");
    {
        int shape[] = {4};
        int val = 42;
        Array *a = full(shape, 1, INT32, &val);
        ASSERT_NOTNULL(a, "full INT32 not null");
        ASSERT_DTYPE(a, INT32, "full INT32 dtype");
        ASSERT_SIZE(a, 4, "full INT32 size=4");
        int *d = (int *)a->data;
        int expected[] = {42, 42, 42, 42};
        ASSERT_EQ_IARR(d, expected, 4, "full INT32 all 42");
        a = free_a(a);
    }
    {
        int shape[] = {5};
        double val = 3.14;
        Array *a = full(shape, 1, FLOAT64, &val);
        ASSERT_NOTNULL(a, "full FLOAT64 not null");
        ASSERT_DTYPE(a, FLOAT64, "full FLOAT64 dtype");
        ASSERT_SIZE(a, 5, "full FLOAT64 size=5");
        double *d = (double *)a->data;
        double expected[] = {3.14, 3.14, 3.14, 3.14, 3.14};
        ASSERT_EQ_ARR(d, expected, 5, TOL_F64, "full FLOAT64 all 3.14");
        a = free_a(a);
    }
    {
        int shape[] = {2, 3};
        double val = -1.5;
        Array *a = full(shape, 2, FLOAT64, &val);
        ASSERT_NOTNULL(a, "full 2D FLOAT64 not null");
        ASSERT_NDIM(a, 2, "full 2D FLOAT64 ndim=2");
        ASSERT_SIZE(a, 6, "full 2D FLOAT64 size=6");
        double *d = (double *)a->data;
        double expected[] = {-1.5, -1.5, -1.5, -1.5, -1.5, -1.5};
        ASSERT_EQ_ARR(d, expected, 6, TOL_F64, "full 2D FLOAT64 all -1.5");
        a = free_a(a);
    }

    /* ── full_int ───────────────────────────────────────────────── */
    TEST_SECTION("full_int");
    {
        int shape[] = {3};
        Array *a = full_int(shape, 1, 99);
        ASSERT_NOTNULL(a, "full_int not null");
        ASSERT_DTYPE(a, INT32, "full_int dtype=INT32");
        ASSERT_SIZE(a, 3, "full_int size=3");
        int *d = (int *)a->data;
        int expected[] = {99, 99, 99};
        ASSERT_EQ_IARR(d, expected, 3, "full_int all 99");
        a = free_a(a);
    }
    {
        int shape[] = {4};
        Array *a = full_int(shape, 1, -7);
        ASSERT_NOTNULL(a, "full_int neg not null");
        ASSERT_DTYPE(a, INT32, "full_int neg dtype=INT32");
        int *d = (int *)a->data;
        int expected[] = {-7, -7, -7, -7};
        ASSERT_EQ_IARR(d, expected, 4, "full_int all -7");
        a = free_a(a);
    }

    /* ── full_float ─────────────────────────────────────────────── */
    TEST_SECTION("full_float");
    {
        /* full_float returns FLOAT32 (4-byte float) */
        int shape[] = {4};
        float val = 2.718f;
        Array *a = full_float(shape, 1, val);
        ASSERT_NOTNULL(a, "full_float not null");
        ASSERT_DTYPE(a, FLOAT32, "full_float dtype=FLOAT32");
        ASSERT_SIZE(a, 4, "full_float size=4");
        float *d = (float *)a->data;
        ASSERT_EQ_DBL(d[0], 2.718f, TOL_F32, "full_float val[0]");
        ASSERT_EQ_DBL(d[1], 2.718f, TOL_F32, "full_float val[1]");
        ASSERT_EQ_DBL(d[2], 2.718f, TOL_F32, "full_float val[2]");
        ASSERT_EQ_DBL(d[3], 2.718f, TOL_F32, "full_float val[3]");
        a = free_a(a);
    }

    /* ── full_complex ───────────────────────────────────────────── */
    TEST_SECTION("full_complex");
    {
        /* full_complex returns COMPLEX64 (8-byte complex float) */
        int shape[] = {2};
        complex float val = 1.5f + 2.5f * I;
        Array *a = full_complex(shape, 1, val);
        ASSERT_NOTNULL(a, "full_complex not null");
        ASSERT_DTYPE(a, COMPLEX64, "full_complex dtype=COMPLEX64");
        ASSERT_SIZE(a, 2, "full_complex size=2");
        complex float *d = (complex float *)a->data;
        ASSERT_EQ_DBL(crealf(d[0]), 1.5f, TOL_F32, "full_complex real[0]");
        ASSERT_EQ_DBL(cimagf(d[0]), 2.5f, TOL_F32, "full_complex imag[0]");
        ASSERT_EQ_DBL(crealf(d[1]), 1.5f, TOL_F32, "full_complex real[1]");
        ASSERT_EQ_DBL(cimagf(d[1]), 2.5f, TOL_F32, "full_complex imag[1]");
        a = free_a(a);
    }

    /* ── free_array edge cases ──────────────────────────────────── */
    TEST_SECTION("free_array");
    {
        free_array(NULL);
        ASSERT_TRUE(1, "free_array(NULL) no crash");
    }
    {
        Array *a = NULL;
        a = free_a(a);
        ASSERT_NULL(a, "free_a(NULL) returns NULL");
    }

    TEST_SUMMARY();
    return (g_fail > 0) ? 1 : 0;
}
