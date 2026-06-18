/**
 * test_array_info.c — Tests for array info / utility functions:
 *   array_info, asize, dtype_size, dtype_name, np_typename,
 *   compare_shapes, get_include, getbufsize, setbufsize
 */

#include "test_utils.h"

int main(void) {
    TEST_MAIN("Array Info / Utilities");

    /* ── asize ──────────────────────────────────────────────────── */
    TEST_SECTION("asize");

    {
        int shape[] = {5};
        Array *a = zeros(shape, 1, FLOAT64);
        ASSERT_NOTNULL(a, "asize create array");

        /* axis=0 on 1D returns 5 */
        int sz = asize(a, 0);
        ASSERT_EQ_INT(sz, 5, "asize axis=0 -> 5");

        /* axis=-1 on 1D returns 5 */
        sz = asize(a, -1);
        ASSERT_EQ_INT(sz, 5, "asize axis=-1 -> 5");

        a = free_a(a);
    }

    {
        /* 2D array: shape {3, 4} */
        int shape[] = {3, 4};
        Array *a = zeros(shape, 2, FLOAT64);
        ASSERT_NOTNULL(a, "asize 2D create array");

        ASSERT_EQ_INT(asize(a, 0), 3, "asize 2D axis=0 -> 3");
        ASSERT_EQ_INT(asize(a, 1), 4, "asize 2D axis=1 -> 4");
        /* asize with negative axis returns total size (like numpy.size with no axis) */
        ASSERT_EQ_INT(asize(a, -1), 12, "asize 2D axis=-1 -> size=12");

        a = free_a(a);
    }

    /* ── dtype_size ─────────────────────────────────────────────── */
    TEST_SECTION("dtype_size");

    ASSERT_EQ_INT((int)dtype_size(INT8),    1, "dtype_size INT8=1");
    ASSERT_EQ_INT((int)dtype_size(INT32),   4, "dtype_size INT32=4");
    ASSERT_EQ_INT((int)dtype_size(INT64),   8, "dtype_size INT64=8");
    ASSERT_EQ_INT((int)dtype_size(FLOAT32), 4, "dtype_size FLOAT32=4");
    ASSERT_EQ_INT((int)dtype_size(FLOAT64), 8, "dtype_size FLOAT64=8");
    ASSERT_EQ_INT((int)dtype_size(COMPLEX64),  8, "dtype_size COMPLEX64=8");
    ASSERT_EQ_INT((int)dtype_size(COMPLEX128), 16, "dtype_size COMPLEX128=16");
    ASSERT_EQ_INT((int)dtype_size(BOOL),    1, "dtype_size BOOL=1");

    /* ── dtype_name ─────────────────────────────────────────────── */
    TEST_SECTION("dtype_name");

    {
        const char *name = dtype_name(INT32);
        ASSERT_NOTNULL(name, "dtype_name INT32 not null");
        ASSERT_TRUE(name[0] != '\0', "dtype_name INT32 non-empty");
    }
    {
        const char *name = dtype_name(FLOAT64);
        ASSERT_NOTNULL(name, "dtype_name FLOAT64 not null");
        ASSERT_TRUE(name[0] != '\0', "dtype_name FLOAT64 non-empty");
    }
    {
        const char *name = dtype_name(COMPLEX128);
        ASSERT_NOTNULL(name, "dtype_name COMPLEX128 not null");
        ASSERT_TRUE(name[0] != '\0', "dtype_name COMPLEX128 non-empty");
    }
    {
        const char *name = dtype_name(BOOL);
        ASSERT_NOTNULL(name, "dtype_name BOOL not null");
    }

    /* ── np_typename ────────────────────────────────────────────── */
    TEST_SECTION("np_typename");

    {
        const char *name = np_typename("i");
        ASSERT_NOTNULL(name, "np_typename('i') not null");
        /* typically maps to INT32 */
    }
    {
        const char *name = np_typename("f8");
        ASSERT_NOTNULL(name, "np_typename('f8') not null");
    }
    {
        const char *name = np_typename("c16");
        ASSERT_NOTNULL(name, "np_typename('c16') not null");
    }

    /* ── compare_shapes ─────────────────────────────────────────── */
    TEST_SECTION("compare_shapes");

    {
        Array *a = zeros((int[]){3, 4}, 2, FLOAT64);
        Array *b = zeros((int[]){3, 4}, 2, INT32);
        ASSERT_TRUE(compare_shapes(a, b), "compare_shapes same shape -> true");

        b = free_a(b);
        Array *c = zeros((int[]){4, 3}, 2, FLOAT64);
        ASSERT_TRUE(!compare_shapes(a, c), "compare_shapes diff order -> false");

        a = free_a(a);
        c = free_a(c);
    }
    {
        /* different ndim */
        Array *a = zeros((int[]){5}, 1, FLOAT64);
        Array *b = zeros((int[]){1, 5}, 2, FLOAT64);
        ASSERT_TRUE(!compare_shapes(a, b), "compare_shapes diff ndim -> false");

        a = free_a(a);
        b = free_a(b);
    }
    /* compare_shapes with 0D not applicable (create_array rejects ndim=0) */

    /* ── get_include ────────────────────────────────────────────── */
    TEST_SECTION("get_include");

    {
        const char *inc = get_include();
        ASSERT_NOTNULL(inc, "get_include not null");
        ASSERT_TRUE(inc[0] != '\0', "get_include non-empty");
    }

    /* ── getbufsize / setbufsize ────────────────────────────────── */
    TEST_SECTION("getbufsize / setbufsize");

    {
        int old = getbufsize();
        ASSERT_TRUE(old > 0, "getbufsize > 0");

        setbufsize(4096);
        ASSERT_EQ_INT(getbufsize(), 4096, "setbufsize(4096) then get = 4096");

        /* restore */
        setbufsize(old);
    }
    {
        /* setbufsize to another value */
        int old = getbufsize();
        setbufsize(8192);
        ASSERT_EQ_INT(getbufsize(), 8192, "setbufsize(8192) then get = 8192");
        setbufsize(old);
    }

    /* ── array_info ─────────────────────────────────────────────── */
    TEST_SECTION("array_info");

    {
        int shape[] = {2, 3};
        Array *a = ones(shape, 2, FLOAT64);
        ASSERT_NOTNULL(a, "array_info create array");
        /* just call it, verify no crash */
        array_info(a);
        ASSERT_TRUE(1, "array_info no crash");
        a = free_a(a);
    }
    {
        /* empty array (ndim=0 not supported, expect NULL) */
        Array *a = create_array(NULL, 0, INT32);
        ASSERT_NULL(a, "array_info 0D returns NULL");
        a = free_a(a);
    }

    TEST_SUMMARY();
    return (g_fail > 0) ? 1 : 0;
}
