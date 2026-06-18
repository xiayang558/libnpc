/**
 * test_arange_linspace.c — Tests for array generation functions:
 *   arange_int, arange_float, arange,
 *   linspace_float, linspace, logspace, geomspace
 */

#include "test_utils.h"
#include "function.h"

int main(void) {
    TEST_MAIN("Arange / Linspace / Logspace / Geomspace");

    /* ── arange_int ─────────────────────────────────────────────── */
    TEST_SECTION("arange_int");

    {
        /* arange_int(0, 10, 1) -> [0,1,2,3,4,5,6,7,8,9] */
        Array *a = arange_int(0, 10, 1);
        ASSERT_NOTNULL(a, "arange_int(0,10,1) not null");
        ASSERT_SIZE(a, 10, "arange_int(0,10,1) size=10");
        ASSERT_DTYPE(a, INT32, "arange_int(0,10,1) dtype=INT32");
        int *d = (int *)a->data;
        int expected[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        ASSERT_EQ_IARR(d, expected, 10, "arange_int(0,10,1) values");
        a = free_a(a);
    }
    {
        /* arange_int(0, 5, 2) -> [0,2,4] */
        Array *a = arange_int(0, 5, 2);
        ASSERT_NOTNULL(a, "arange_int(0,5,2) not null");
        ASSERT_SIZE(a, 3, "arange_int(0,5,2) size=3");
        ASSERT_DTYPE(a, INT32, "arange_int(0,5,2) dtype=INT32");
        int *d = (int *)a->data;
        int expected[] = {0, 2, 4};
        ASSERT_EQ_IARR(d, expected, 3, "arange_int(0,5,2) values");
        a = free_a(a);
    }
    {
        /* arange_int(5, 0, -1) -> [5,4,3,2,1] */
        Array *a = arange_int(5, 0, -1);
        ASSERT_NOTNULL(a, "arange_int(5,0,-1) not null");
        ASSERT_SIZE(a, 5, "arange_int(5,0,-1) size=5");
        ASSERT_DTYPE(a, INT32, "arange_int(5,0,-1) dtype=INT32");
        int *d = (int *)a->data;
        int expected[] = {5, 4, 3, 2, 1};
        ASSERT_EQ_IARR(d, expected, 5, "arange_int(5,0,-1) values");
        a = free_a(a);
    }
    {
        /* arange_int(0, 0, 1) -> Invalid range */
        Array *a = arange_int(0, 0, 1);
        ASSERT_NULL(a, "arange_int(0,0,1) returns NULL");
        a = free_a(a);
    }

    /* ── arange_float ───────────────────────────────────────────── */
    TEST_SECTION("arange_float");

    {
        /* arange_float(0.0, 1.0, 0.25) -> [0.0, 0.25, 0.5, 0.75]
         * arange_float returns FLOAT32. The compute uses float arithmetic,
         * so (0.0 + 3*0.25) = 0.75 exactly in float. */
        Array *a = arange_float(0.0f, 1.0f, 0.25f);
        ASSERT_NOTNULL(a, "arange_float(0,1,0.25) not null");
        ASSERT_SIZE(a, 4, "arange_float(0,1,0.25) size=4");
        ASSERT_DTYPE(a, FLOAT32, "arange_float dtype=FLOAT32");
        float *d = (float *)a->data;
        ASSERT_EQ_DBL(d[0], 0.0, TOL_F32, "arange_float val[0]");
        ASSERT_EQ_DBL(d[1], 0.25, TOL_F32, "arange_float val[1]");
        ASSERT_EQ_DBL(d[2], 0.5, TOL_F32, "arange_float val[2]");
        ASSERT_EQ_DBL(d[3], 0.75, TOL_F32, "arange_float val[3]");
        a = free_a(a);
    }
    {
        /* arange_float(0.0, 0.5, 0.3) -> [0.0, 0.3]
         * The implementation uses int(size) = int((stop-start)/step) = int(0.5/0.3) = int(1.666...) = 1.
         * So it returns only [0.0]. */
        Array *a = arange_float(0.0f, 0.5f, 0.3f);
        ASSERT_NOTNULL(a, "arange_float(0,0.5,0.3) not null");
        ASSERT_SIZE(a, 1, "arange_float(0,0.5,0.3) size=1");
        a = free_a(a);
    }

    /* ── arange ─────────────────────────────────────────────────── */
    TEST_SECTION("arange");

    {
        /* arange(0.0, 3.0, 1.0, FLOAT64) -> [0.0, 1.0, 2.0] */
        Array *a = arange(0.0, 3.0, 1.0, FLOAT64);
        ASSERT_NOTNULL(a, "arange(FLOAT64) not null");
        ASSERT_SIZE(a, 3, "arange(FLOAT64) size=3");
        ASSERT_DTYPE(a, FLOAT64, "arange(FLOAT64) dtype=FLOAT64");
        double *d = (double *)a->data;
        double expected[] = {0.0, 1.0, 2.0};
        ASSERT_EQ_ARR(d, expected, 3, TOL_F64, "arange(FLOAT64) values");
        a = free_a(a);
    }
    {
        /* arange with INT32 */
        Array *a = arange(0.0, 5.0, 1.0, INT32);
        ASSERT_NOTNULL(a, "arange(INT32) not null");
        ASSERT_SIZE(a, 5, "arange(INT32) size=5");
        ASSERT_DTYPE(a, INT32, "arange(INT32) dtype=INT32");
        int *d = (int *)a->data;
        int expected[] = {0, 1, 2, 3, 4};
        ASSERT_EQ_IARR(d, expected, 5, "arange(INT32) values");
        a = free_a(a);
    }

    /* ── linspace_float ─────────────────────────────────────────── */
    TEST_SECTION("linspace_float");

    {
        /* linspace_float(0.0, 1.0, 5) -> [0.0, 0.25, 0.5, 0.75, 1.0]
         * linspace_float returns FLOAT32. */
        Array *a = linspace_float(0.0f, 1.0f, 5);
        ASSERT_NOTNULL(a, "linspace_float(0,1,5) not null");
        ASSERT_SIZE(a, 5, "linspace_float(0,1,5) size=5");
        ASSERT_DTYPE(a, FLOAT32, "linspace_float dtype=FLOAT32");
        float *d = (float *)a->data;
        ASSERT_EQ_DBL(d[0], 0.0, TOL_F32, "linspace_float val[0]");
        ASSERT_EQ_DBL(d[1], 0.25, TOL_F32, "linspace_float val[1]");
        ASSERT_EQ_DBL(d[2], 0.5, TOL_F32, "linspace_float val[2]");
        ASSERT_EQ_DBL(d[3], 0.75, TOL_F32, "linspace_float val[3]");
        ASSERT_EQ_DBL(d[4], 1.0, TOL_F32, "linspace_float val[4]");
        a = free_a(a);
    }
    {
        /* linspace_float(1.0, 2.0, 2) -> [1.0, 2.0]
         * linspace_float returns FLOAT32. */
        Array *a = linspace_float(1.0f, 2.0f, 2);
        ASSERT_NOTNULL(a, "linspace_float(1,2,2) not null");
        ASSERT_SIZE(a, 2, "linspace_float(1,2,2) size=2");
        float *d = (float *)a->data;
        ASSERT_EQ_DBL(d[0], 1.0, TOL_F32, "linspace_float val[0]");
        ASSERT_EQ_DBL(d[1], 2.0, TOL_F32, "linspace_float val[1]");
        a = free_a(a);
    }

    /* ── linspace (from function.h) ─────────────────────────────── */
    TEST_SECTION("linspace");

    {
        Array *a = linspace(0.0f, 1.0f, 5);
        ASSERT_NOTNULL(a, "linspace(0,1,5) not null");
        ASSERT_SIZE(a, 5, "linspace(0,1,5) size=5");
        float *d = (float *)a->data;
        ASSERT_EQ_DBL(d[0], 0.0, TOL_F32, "linspace val[0]");
        ASSERT_EQ_DBL(d[1], 0.25, TOL_F32, "linspace val[1]");
        ASSERT_EQ_DBL(d[2], 0.5, TOL_F32, "linspace val[2]");
        ASSERT_EQ_DBL(d[3], 0.75, TOL_F32, "linspace val[3]");
        ASSERT_EQ_DBL(d[4], 1.0, TOL_F32, "linspace val[4]");
        a = free_a(a);
    }

    /* ── logspace ───────────────────────────────────────────────── */
    TEST_SECTION("logspace");

    {
        /* logspace(0, 2, 3, 10.0, 1) -> [1.0, 10.0, 100.0] */
        Array *a = logspace(0.0, 2.0, 3, 10.0, 1);
        ASSERT_NOTNULL(a, "logspace(0,2,3,10,1) not null");
        ASSERT_SIZE(a, 3, "logspace(0,2,3,10,1) size=3");
        double *d = (double *)a->data;
        double expected[] = {1.0, 10.0, 100.0};
        ASSERT_EQ_ARR(d, expected, 3, TOL_F64, "logspace(0,2,3,10,1) values");
        a = free_a(a);
    }
    {
        /* logspace(1, 3, 3, 10.0, 1) -> [10.0, 100.0, 1000.0] */
        Array *a = logspace(1.0, 3.0, 3, 10.0, 1);
        ASSERT_NOTNULL(a, "logspace(1,3,3,10,1) not null");
        ASSERT_SIZE(a, 3, "logspace(1,3,3,10,1) size=3");
        double *d = (double *)a->data;
        double expected[] = {10.0, 100.0, 1000.0};
        ASSERT_EQ_ARR(d, expected, 3, TOL_F64, "logspace(1,3,3,10,1) values");
        a = free_a(a);
    }

    /* ── geomspace ──────────────────────────────────────────────── */
    TEST_SECTION("geomspace");

    {
        /* geomspace(1.0, 100.0, 3, 1) -> [1.0, 10.0, 100.0] */
        Array *a = geomspace(1.0, 100.0, 3, 1);
        ASSERT_NOTNULL(a, "geomspace(1,100,3,1) not null");
        ASSERT_SIZE(a, 3, "geomspace(1,100,3,1) size=3");
        double *d = (double *)a->data;
        double expected[] = {1.0, 10.0, 100.0};
        ASSERT_EQ_ARR(d, expected, 3, TOL_F64, "geomspace(1,100,3,1) values");
        a = free_a(a);
    }
    {
        /* geomspace(1.0, 8.0, 4, 1) -> [1.0, 2.0, 4.0, 8.0] */
        Array *a = geomspace(1.0, 8.0, 4, 1);
        ASSERT_NOTNULL(a, "geomspace(1,8,4,1) not null");
        ASSERT_SIZE(a, 4, "geomspace(1,8,4,1) size=4");
        double *d = (double *)a->data;
        double expected[] = {1.0, 2.0, 4.0, 8.0};
        ASSERT_EQ_ARR(d, expected, 4, TOL_F64, "geomspace(1,8,4,1) values");
        a = free_a(a);
    }
    {
        /* geomspace(1.0, 25.0, 3, 0) with endpoint=0 generates total=4 internal
         * points at powers 0/3, 1/3, 2/3, 3/3 of ratio=25, but only stores the
         * first 3 (num=3): [1, 25^(1/3), 25^(2/3)] */
        Array *a = geomspace(1.0, 25.0, 3, 0);
        ASSERT_NOTNULL(a, "geomspace(1,25,3,0) not null");
        ASSERT_SIZE(a, 3, "geomspace(1,25,3,0) size=3");
        double *d = (double *)a->data;
        double r1 = pow(25.0, 1.0/3.0);
        double r2 = pow(25.0, 2.0/3.0);
        double expected[] = {1.0, r1, r2};
        ASSERT_EQ_ARR(d, expected, 3, 1e-10, "geomspace(1,25,3,0) values");
        a = free_a(a);
    }

    TEST_SUMMARY();
    return (g_fail > 0) ? 1 : 0;
}
