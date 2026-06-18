/**
 * test_astype_copy.c — Tests for type conversion and copy functions:
 *   astype, copy_array, copy, conj_transpose
 */

#include "test_utils.h"

int main(void) {
    TEST_MAIN("Astype / Copy / ConjTranspose");

    /* ── astype: FLOAT64 -> INT32 ───────────────────────────────── */
    TEST_SECTION("astype: FLOAT64 -> INT32");

    {
        double data[] = {1.7, 2.3, 3.9, -4.2};
        Array *orig = create_array((int[]){4}, 1, FLOAT64);
        memcpy(orig->data, data, 4 * sizeof(double));

        Array *a = astype(orig, INT32);
        ASSERT_NOTNULL(a, "astype FLOAT64->INT32 not null");
        ASSERT_DTYPE(a, INT32, "astype FLOAT64->INT32 dtype=INT32");
        ASSERT_SIZE(a, 4, "astype FLOAT64->INT32 size=4");

        int *d = (int *)a->data;
        ASSERT_EQ_INT(d[0], 1, "astype FLOAT64->INT32 [0]=1");
        ASSERT_EQ_INT(d[1], 2, "astype FLOAT64->INT32 [1]=2");
        ASSERT_EQ_INT(d[2], 3, "astype FLOAT64->INT32 [2]=3");
        ASSERT_EQ_INT(d[3], -4, "astype FLOAT64->INT32 [3]=-4");

        orig = free_a(orig);
        a = free_a(a);
    }

    /* ── astype: INT32 -> FLOAT64 ───────────────────────────────── */
    TEST_SECTION("astype: INT32 -> FLOAT64");

    {
        int data[] = {1, 2, 3, -5};
        Array *orig = create_array((int[]){4}, 1, INT32);
        memcpy(orig->data, data, 4 * sizeof(int));

        Array *a = astype(orig, FLOAT64);
        ASSERT_NOTNULL(a, "astype INT32->FLOAT64 not null");
        ASSERT_DTYPE(a, FLOAT64, "astype INT32->FLOAT64 dtype=FLOAT64");
        ASSERT_SIZE(a, 4, "astype INT32->FLOAT64 size=4");

        double *d = (double *)a->data;
        double expected[] = {1.0, 2.0, 3.0, -5.0};
        ASSERT_EQ_ARR(d, expected, 4, TOL_F64, "astype INT32->FLOAT64 values");

        orig = free_a(orig);
        a = free_a(a);
    }

    /* ── astype: FLOAT64 -> COMPLEX128 ──────────────────────────── */
    TEST_SECTION("astype: FLOAT64 -> COMPLEX128");

    {
        double data[] = {1.0, 2.0, 3.0};
        Array *orig = create_array((int[]){3}, 1, FLOAT64);
        memcpy(orig->data, data, 3 * sizeof(double));

        Array *a = astype(orig, COMPLEX128);
        ASSERT_NOTNULL(a, "astype FLOAT64->COMPLEX128 not null");
        ASSERT_DTYPE(a, COMPLEX128, "astype FLOAT64->COMPLEX128 dtype=COMPLEX128");
        ASSERT_SIZE(a, 3, "astype FLOAT64->COMPLEX128 size=3");

        complex double *d = (complex double *)a->data;
        complex double expected[] = {1.0+0.0*I, 2.0+0.0*I, 3.0+0.0*I};
        ASSERT_EQ_CPX(d, expected, 3, TOL_F64, "astype FLOAT64->COMPLEX128 values");

        orig = free_a(orig);
        a = free_a(a);
    }

    /* ── copy_array ─────────────────────────────────────────────── */
    TEST_SECTION("copy_array");

    {
        double data[] = {10.0, 20.0, 30.0};
        Array *orig = create_array((int[]){3}, 1, FLOAT64);
        memcpy(orig->data, data, 3 * sizeof(double));

        Array *cp = copy_array(orig);
        ASSERT_NOTNULL(cp, "copy_array not null");
        ASSERT_DTYPE(cp, FLOAT64, "copy_array dtype=FLOAT64");
        ASSERT_SIZE(cp, 3, "copy_array size=3");

        /* verify same data */
        double *d_cp = (double *)cp->data;
        double expected[] = {10.0, 20.0, 30.0};
        ASSERT_EQ_ARR(d_cp, expected, 3, TOL_F64, "copy_array same data");

        /* verify deep copy (different data pointer) */
        ASSERT_TRUE(cp->data != orig->data, "copy_array deep copy (diff ptr)");

        /* modify original, verify copy is unchanged */
        ((double *)orig->data)[0] = 99.0;
        ASSERT_EQ_DBL(((double *)cp->data)[0], 10.0, TOL_F64,
                      "copy_array unchanged after orig modified");

        orig = free_a(orig);
        cp = free_a(cp);
    }

    /* ── copy (alias) ───────────────────────────────────────────── */
    TEST_SECTION("copy");

    {
        int data[] = {5, 10, 15};
        Array *orig = create_array((int[]){3}, 1, INT32);
        memcpy(orig->data, data, 3 * sizeof(int));

        Array *cp = copy(orig);
        ASSERT_NOTNULL(cp, "copy not null");
        ASSERT_DTYPE(cp, INT32, "copy dtype=INT32");
        ASSERT_SIZE(cp, 3, "copy size=3");

        int *d = (int *)cp->data;
        int expected[] = {5, 10, 15};
        ASSERT_EQ_IARR(d, expected, 3, "copy same data");

        ASSERT_TRUE(cp->data != orig->data, "copy deep copy (diff ptr)");

        orig = free_a(orig);
        cp = free_a(cp);
    }

    /* ── conj_transpose: real matrix ────────────────────────────── */
    TEST_SECTION("conj_transpose: real");

    {
        /* conj_transpose only works on complex arrays; real input returns NULL */
        double data[] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
        Array *orig = create_array((int[]){2, 3}, 2, FLOAT64);
        memcpy(orig->data, data, 6 * sizeof(double));

        Array *a = conj_transpose(orig);
        ASSERT_NULL(a, "conj_transpose real returns NULL (not complex)");

        orig = free_a(orig);
    }

    /* ── conj_transpose: complex matrix ─────────────────────────── */
    TEST_SECTION("conj_transpose: complex");

    {
        /* 2x3 complex: [[1+i, 2+0i, 3+0i],[4+0i, 5+0i, 4-i]]
         * transpose+conj -> 3x2: [[1-i, 4+0i],[2+0i, 5+0i],[3+0i, 4+i]] */
        complex double data[] = {
            1.0+1.0*I,  2.0+0.0*I,  3.0+0.0*I,
            4.0+0.0*I,  5.0+0.0*I,  4.0-1.0*I
        };
        Array *orig = create_array((int[]){2, 3}, 2, COMPLEX128);
        memcpy(orig->data, data, 6 * sizeof(complex double));

        Array *a = conj_transpose(orig);
        ASSERT_NOTNULL(a, "conj_transpose complex not null");
        ASSERT_DTYPE(a, COMPLEX128, "conj_transpose complex dtype");
        ASSERT_NDIM(a, 2, "conj_transpose complex ndim=2");
        ASSERT_SIZE(a, 6, "conj_transpose complex size=6");
        ASSERT_EQ_INT(a->shape[0], 3, "conj_transpose complex shape[0]=3");
        ASSERT_EQ_INT(a->shape[1], 2, "conj_transpose complex shape[1]=2");

        complex double *d = (complex double *)a->data;
        complex double expected[] = {
            1.0-1.0*I,  4.0+0.0*I,   /* row 0: (1+i)->(1-i), (4+0i)->(4+0i) */
            2.0+0.0*I,  5.0+0.0*I,   /* row 1 */
            3.0+0.0*I,  4.0+1.0*I    /* row 2: (4-i)->(4+i) */
        };
        ASSERT_EQ_CPX(d, expected, 6, TOL_F64, "conj_transpose complex values");

        orig = free_a(orig);
        a = free_a(a);
    }

    /* ── conj_transpose: 1D array ───────────────────────────────── */
    TEST_SECTION("conj_transpose: 1D");

    {
        /* conj_transpose requires complex input; 1D real returns NULL */
        double data[] = {1.0, 2.0, 3.0};
        Array *orig = create_array((int[]){3}, 1, FLOAT64);
        memcpy(orig->data, data, 3 * sizeof(double));

        Array *a = conj_transpose(orig);
        ASSERT_NULL(a, "conj_transpose 1D returns NULL (not complex)");

        orig = free_a(orig);
    }

    TEST_SUMMARY();
    return (g_fail > 0) ? 1 : 0;
}
