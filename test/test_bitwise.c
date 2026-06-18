/**
 * test_bitwise.c — Tests for bitwise functions:
 *   bitwise_and, bitwise_or, bitwise_xor, bitwise_not, invert,
 *   left_shift, right_shift
 */

#include "test_utils.h"

int main(void) {
    TEST_MAIN("Bitwise");

    /* ── bitwise_and ──────────────────────────────────────────────── */
    TEST_SECTION("bitwise_and");

    {
        /* bitwise_and: [1,2,3] & [1,1,3] -> [1,0,3]
         * 1 & 1 = 1, 2 & 1 = 0, 3 & 3 = 3 */
        int d1[] = {1, 2, 3};
        int d2[] = {1, 1, 3};
        Array *a1 = make_i32_1d(d1, 3);
        Array *a2 = make_i32_1d(d2, 3);
        Array *r = bitwise_and(a1, a2);
        ASSERT_NOTNULL(r, "bitwise_and not null");
        ASSERT_DTYPE(r, INT32, "bitwise_and output dtype INT32");
        ASSERT_SIZE(r, 3, "bitwise_and size=3");
        int expected[] = {1, 0, 3};
        ASSERT_EQ_IARR((int *)r->data, expected, 3, "bitwise_and [1,2,3]&[1,1,3]");
        free_a(a1); free_a(a2); free_a(r);
    }
    {
        /* bitwise_and: all same */
        int d1[] = {7, 7, 7};
        int d2[] = {7, 7, 7};
        Array *a1 = make_i32_1d(d1, 3);
        Array *a2 = make_i32_1d(d2, 3);
        Array *r = bitwise_and(a1, a2);
        ASSERT_NOTNULL(r, "bitwise_and same not null");
        int expected[] = {7, 7, 7};
        ASSERT_EQ_IARR((int *)r->data, expected, 3, "bitwise_and [7,7,7]&[7,7,7]");
        free_a(a1); free_a(a2); free_a(r);
    }
    {
        /* bitwise_and: with zeros */
        int d1[] = {15, 8, 3};
        int d2[] = {0, 0, 0};
        Array *a1 = make_i32_1d(d1, 3);
        Array *a2 = make_i32_1d(d2, 3);
        Array *r = bitwise_and(a1, a2);
        ASSERT_NOTNULL(r, "bitwise_and with zeros not null");
        int expected[] = {0, 0, 0};
        ASSERT_EQ_IARR((int *)r->data, expected, 3, "bitwise_and [15,8,3]&[0,0,0]");
        free_a(a1); free_a(a2); free_a(r);
    }

    /* ── bitwise_or ───────────────────────────────────────────────── */
    TEST_SECTION("bitwise_or");

    {
        /* bitwise_or: [1,2,4] | [1,1,1] -> [1,3,5]
         * 1 | 1 = 1, 2 | 1 = 3, 4 | 1 = 5 */
        int d1[] = {1, 2, 4};
        int d2[] = {1, 1, 1};
        Array *a1 = make_i32_1d(d1, 3);
        Array *a2 = make_i32_1d(d2, 3);
        Array *r = bitwise_or(a1, a2);
        ASSERT_NOTNULL(r, "bitwise_or not null");
        ASSERT_DTYPE(r, INT32, "bitwise_or output dtype INT32");
        int expected[] = {1, 3, 5};
        ASSERT_EQ_IARR((int *)r->data, expected, 3, "bitwise_or [1,2,4]|[1,1,1]");
        free_a(a1); free_a(a2); free_a(r);
    }
    {
        /* bitwise_or: with zeros */
        int d1[] = {5, 0, 7};
        int d2[] = {0, 3, 0};
        Array *a1 = make_i32_1d(d1, 3);
        Array *a2 = make_i32_1d(d2, 3);
        Array *r = bitwise_or(a1, a2);
        ASSERT_NOTNULL(r, "bitwise_or zeros not null");
        int expected[] = {5, 3, 7};
        ASSERT_EQ_IARR((int *)r->data, expected, 3, "bitwise_or [5,0,7]|[0,3,0]");
        free_a(a1); free_a(a2); free_a(r);
    }

    /* ── bitwise_xor ──────────────────────────────────────────────── */
    TEST_SECTION("bitwise_xor");

    {
        /* bitwise_xor: [1,2,3] xor [1,1,3] -> [0,3,0]
         * 1^1=0, 2^1=3, 3^3=0 */
        int d1[] = {1, 2, 3};
        int d2[] = {1, 1, 3};
        Array *a1 = make_i32_1d(d1, 3);
        Array *a2 = make_i32_1d(d2, 3);
        Array *r = bitwise_xor(a1, a2);
        ASSERT_NOTNULL(r, "bitwise_xor not null");
        ASSERT_DTYPE(r, INT32, "bitwise_xor output dtype INT32");
        int expected[] = {0, 3, 0};
        ASSERT_EQ_IARR((int *)r->data, expected, 3, "bitwise_xor [1,2,3]^[1,1,3]");
        free_a(a1); free_a(a2); free_a(r);
    }
    {
        /* bitwise_xor: all same -> zeros */
        int d1[] = {7, 5, 3};
        int d2[] = {7, 5, 3};
        Array *a1 = make_i32_1d(d1, 3);
        Array *a2 = make_i32_1d(d2, 3);
        Array *r = bitwise_xor(a1, a2);
        ASSERT_NOTNULL(r, "bitwise_xor same not null");
        int expected[] = {0, 0, 0};
        ASSERT_EQ_IARR((int *)r->data, expected, 3, "bitwise_xor same = [0,0,0]");
        free_a(a1); free_a(a2); free_a(r);
    }

    /* ── bitwise_not ──────────────────────────────────────────────── */
    TEST_SECTION("bitwise_not");

    {
        /* bitwise_not: ~[0, -1] -> [-1, 0] (on INT32)
         * ~0 = -1, ~(-1) = 0 */
        int d[] = {0, -1};
        Array *a = make_i32_1d(d, 2);
        Array *r = bitwise_not(a);
        ASSERT_NOTNULL(r, "bitwise_not not null");
        ASSERT_DTYPE(r, INT32, "bitwise_not output dtype INT32");
        ASSERT_SIZE(r, 2, "bitwise_not size=2");
        int expected[] = {-1, 0};
        ASSERT_EQ_IARR((int *)r->data, expected, 2, "bitwise_not [0,-1] = [-1,0]");
        free_a(a); free_a(r);
    }
    {
        /* bitwise_not: ~5 = -6 */
        int d[] = {5};
        Array *a = make_i32_1d(d, 1);
        Array *r = bitwise_not(a);
        ASSERT_NOTNULL(r, "bitwise_not 5 not null");
        int *rd = (int *)r->data;
        ASSERT_EQ_INT(rd[0], -6, "bitwise_not 5 = -6");
        free_a(a); free_a(r);
    }
    {
        /* bitwise_not: ~3 = -4 */
        int d[] = {3};
        Array *a = make_i32_1d(d, 1);
        Array *r = bitwise_not(a);
        ASSERT_NOTNULL(r, "bitwise_not 3 not null");
        int *rd = (int *)r->data;
        ASSERT_EQ_INT(rd[0], -4, "bitwise_not 3 = -4");
        free_a(a); free_a(r);
    }

    /* ── invert (same as bitwise_not) ─────────────────────────────── */
    TEST_SECTION("invert");

    {
        /* invert: ~[0, -1] -> [-1, 0] */
        int d[] = {0, -1};
        Array *a = make_i32_1d(d, 2);
        Array *r = invert(a);
        ASSERT_NOTNULL(r, "invert not null");
        ASSERT_DTYPE(r, INT32, "invert output dtype INT32");
        int expected[] = {-1, 0};
        ASSERT_EQ_IARR((int *)r->data, expected, 2, "invert [0,-1] = [-1,0]");
        free_a(a); free_a(r);
    }

    /* ── left_shift ───────────────────────────────────────────────── */
    TEST_SECTION("left_shift");

    {
        /* left_shift: [1,2,3] << [1,1,2] -> [2,4,12]
         * 1<<1=2, 2<<1=4, 3<<2=12 */
        int d1[] = {1, 2, 3};
        int d2[] = {1, 1, 2};
        Array *a1 = make_i32_1d(d1, 3);
        Array *a2 = make_i32_1d(d2, 3);
        Array *r = left_shift(a1, a2);
        ASSERT_NOTNULL(r, "left_shift not null");
        ASSERT_DTYPE(r, INT32, "left_shift output dtype INT32");
        int expected[] = {2, 4, 12};
        ASSERT_EQ_IARR((int *)r->data, expected, 3, "left_shift [1,2,3]<<[1,1,2]");
        free_a(a1); free_a(a2); free_a(r);
    }
    {
        /* left_shift: shift by 0 */
        int d1[] = {7, 8};
        int d2[] = {0, 0};
        Array *a1 = make_i32_1d(d1, 2);
        Array *a2 = make_i32_1d(d2, 2);
        Array *r = left_shift(a1, a2);
        ASSERT_NOTNULL(r, "left_shift by 0 not null");
        int expected[] = {7, 8};
        ASSERT_EQ_IARR((int *)r->data, expected, 2, "left_shift by 0 = unchanged");
        free_a(a1); free_a(a2); free_a(r);
    }

    /* ── right_shift ──────────────────────────────────────────────── */
    TEST_SECTION("right_shift");

    {
        /* right_shift: [4,8,16] >> [1,2,1] -> [2,2,8]
         * 4>>1=2, 8>>2=2, 16>>1=8 */
        int d1[] = {4, 8, 16};
        int d2[] = {1, 2, 1};
        Array *a1 = make_i32_1d(d1, 3);
        Array *a2 = make_i32_1d(d2, 3);
        Array *r = right_shift(a1, a2);
        ASSERT_NOTNULL(r, "right_shift not null");
        ASSERT_DTYPE(r, INT32, "right_shift output dtype INT32");
        int expected[] = {2, 2, 8};
        ASSERT_EQ_IARR((int *)r->data, expected, 3, "right_shift [4,8,16]>>[1,2,1]");
        free_a(a1); free_a(a2); free_a(r);
    }
    {
        /* right_shift: shift by 0 */
        int d1[] = {31, 63};
        int d2[] = {0, 0};
        Array *a1 = make_i32_1d(d1, 2);
        Array *a2 = make_i32_1d(d2, 2);
        Array *r = right_shift(a1, a2);
        ASSERT_NOTNULL(r, "right_shift by 0 not null");
        int expected[] = {31, 63};
        ASSERT_EQ_IARR((int *)r->data, expected, 2, "right_shift by 0 = unchanged");
        free_a(a1); free_a(a2); free_a(r);
    }
    {
        /* right_shift: negative number (-8 >> 1 = -4) */
        int d1[] = {-8, -16};
        int d2[] = {1, 2};
        Array *a1 = make_i32_1d(d1, 2);
        Array *a2 = make_i32_1d(d2, 2);
        Array *r = right_shift(a1, a2);
        ASSERT_NOTNULL(r, "right_shift negatives not null");
        int expected[] = {-4, -4};
        ASSERT_EQ_IARR((int *)r->data, expected, 2, "right_shift [-8,-16]>>[1,2]");
        free_a(a1); free_a(a2); free_a(r);
    }

    TEST_SUMMARY();
    return (g_fail > 0) ? 1 : 0;
}
