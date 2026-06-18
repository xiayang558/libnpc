/**
 * test_logic.c — Tests for logical functions:
 *   logical_and, logical_or, logical_xor, logical_not, all, any
 */

#include "test_utils.h"

int main(void) {
    TEST_MAIN("Logic");

    /* ── logical_and ──────────────────────────────────────────────── */
    TEST_SECTION("logical_and");

    {
        /* logical_and: [1,0,1] & [1,1,0] -> [1,0,0] */
        double d1[] = {1, 0, 1};
        double d2[] = {1, 1, 0};
        Array *a1 = make_f64_1d(d1, 3);
        Array *a2 = make_f64_1d(d2, 3);
        Array *r = logical_and(a1, a2);
        ASSERT_NOTNULL(r, "logical_and not null");
        ASSERT_DTYPE(r, BOOL, "logical_and output dtype BOOL");
        ASSERT_SIZE(r, 3, "logical_and size=3");
        uint8_t expected[] = {1, 0, 0};
        ASSERT_EQ_IARR((uint8_t *)r->data, expected, 3, "logical_and [1,0,1]&[1,1,0]");
        free_a(a1); free_a(a2); free_a(r);
    }
    {
        /* logical_and: all true */
        double d1[] = {1, 1};
        double d2[] = {1, 1};
        Array *a1 = make_f64_1d(d1, 2);
        Array *a2 = make_f64_1d(d2, 2);
        Array *r = logical_and(a1, a2);
        ASSERT_NOTNULL(r, "logical_and all true not null");
        uint8_t expected[] = {1, 1};
        ASSERT_EQ_IARR((uint8_t *)r->data, expected, 2, "logical_and [1,1]&[1,1] = [1,1]");
        free_a(a1); free_a(a2); free_a(r);
    }
    {
        /* logical_and: all false with zeros */
        double d1[] = {0, 0, 0};
        double d2[] = {5, -3, 2};
        Array *a1 = make_f64_1d(d1, 3);
        Array *a2 = make_f64_1d(d2, 3);
        Array *r = logical_and(a1, a2);
        ASSERT_NOTNULL(r, "logical_and all false not null");
        uint8_t expected[] = {0, 0, 0};
        ASSERT_EQ_IARR((uint8_t *)r->data, expected, 3, "logical_and [0,0,0]&[5,-3,2] = [0,0,0]");
        free_a(a1); free_a(a2); free_a(r);
    }
    {
        /* logical_and: INT32 truthy-check */
        int d1[] = {3, 0, -7};
        int d2[] = {0, 5, 8};
        Array *a1 = make_i32_1d(d1, 3);
        Array *a2 = make_i32_1d(d2, 3);
        Array *r = logical_and(a1, a2);
        ASSERT_NOTNULL(r, "logical_and INT32 not null");
        uint8_t expected[] = {0, 0, 1};  /* (3&&0)=0, (0&&5)=0, (-7&&8)=1 */
        ASSERT_EQ_IARR((uint8_t *)r->data, expected, 3, "logical_and INT32 [3,0,-7]&[0,5,8]");
        free_a(a1); free_a(a2); free_a(r);
    }

    /* ── logical_or ───────────────────────────────────────────────── */
    TEST_SECTION("logical_or");

    {
        /* logical_or: [1,0,1] | [0,0,0] -> [1,0,1] */
        double d1[] = {1, 0, 1};
        double d2[] = {0, 0, 0};
        Array *a1 = make_f64_1d(d1, 3);
        Array *a2 = make_f64_1d(d2, 3);
        Array *r = logical_or(a1, a2);
        ASSERT_NOTNULL(r, "logical_or not null");
        ASSERT_DTYPE(r, BOOL, "logical_or output dtype BOOL");
        uint8_t expected[] = {1, 0, 1};
        ASSERT_EQ_IARR((uint8_t *)r->data, expected, 3, "logical_or [1,0,1]|[0,0,0]");
        free_a(a1); free_a(a2); free_a(r);
    }
    {
        /* logical_or: both false */
        double d1[] = {0, 0};
        double d2[] = {0, 0};
        Array *a1 = make_f64_1d(d1, 2);
        Array *a2 = make_f64_1d(d2, 2);
        Array *r = logical_or(a1, a2);
        ASSERT_NOTNULL(r, "logical_or both false not null");
        uint8_t expected[] = {0, 0};
        ASSERT_EQ_IARR((uint8_t *)r->data, expected, 2, "logical_or [0,0]|[0,0] = [0,0]");
        free_a(a1); free_a(a2); free_a(r);
    }
    {
        /* logical_or: INT32 */
        int d1[] = {0, 0, 2};
        int d2[] = {0, 3, 0};
        Array *a1 = make_i32_1d(d1, 3);
        Array *a2 = make_i32_1d(d2, 3);
        Array *r = logical_or(a1, a2);
        ASSERT_NOTNULL(r, "logical_or INT32 not null");
        uint8_t expected[] = {0, 1, 1};
        ASSERT_EQ_IARR((uint8_t *)r->data, expected, 3, "logical_or INT32 [0,0,2]|[0,3,0]");
        free_a(a1); free_a(a2); free_a(r);
    }

    /* ── logical_xor ──────────────────────────────────────────────── */
    TEST_SECTION("logical_xor");

    {
        /* logical_xor: [1,0,1] xor [1,1,0] -> [0,1,1] */
        double d1[] = {1, 0, 1};
        double d2[] = {1, 1, 0};
        Array *a1 = make_f64_1d(d1, 3);
        Array *a2 = make_f64_1d(d2, 3);
        Array *r = logical_xor(a1, a2);
        ASSERT_NOTNULL(r, "logical_xor not null");
        ASSERT_DTYPE(r, BOOL, "logical_xor output dtype BOOL");
        uint8_t expected[] = {0, 1, 1};
        ASSERT_EQ_IARR((uint8_t *)r->data, expected, 3, "logical_xor [1,0,1]^[1,1,0]");
        free_a(a1); free_a(a2); free_a(r);
    }
    {
        /* logical_xor: same values -> all false */
        double d1[] = {1, 0, 1};
        double d2[] = {1, 0, 1};
        Array *a1 = make_f64_1d(d1, 3);
        Array *a2 = make_f64_1d(d2, 3);
        Array *r = logical_xor(a1, a2);
        ASSERT_NOTNULL(r, "logical_xor same not null");
        uint8_t expected[] = {0, 0, 0};
        ASSERT_EQ_IARR((uint8_t *)r->data, expected, 3, "logical_xor same = [0,0,0]");
        free_a(a1); free_a(a2); free_a(r);
    }

    /* ── logical_not ──────────────────────────────────────────────── */
    TEST_SECTION("logical_not");

    {
        /* logical_not: ![1,0,1] -> [0,1,0] */
        double d[] = {1, 0, 1};
        Array *a = make_f64_1d(d, 3);
        Array *r = logical_not(a);
        ASSERT_NOTNULL(r, "logical_not not null");
        ASSERT_DTYPE(r, BOOL, "logical_not output dtype BOOL");
        ASSERT_SIZE(r, 3, "logical_not size=3");
        uint8_t expected[] = {0, 1, 0};
        ASSERT_EQ_IARR((uint8_t *)r->data, expected, 3, "logical_not [1,0,1]");
        free_a(a); free_a(r);
    }
    {
        /* logical_not: all zeros -> all ones */
        double d[] = {0, 0, 0, 0};
        Array *a = make_f64_1d(d, 4);
        Array *r = logical_not(a);
        ASSERT_NOTNULL(r, "logical_not zeros not null");
        uint8_t expected[] = {1, 1, 1, 1};
        ASSERT_EQ_IARR((uint8_t *)r->data, expected, 4, "logical_not [0,0,0,0] = [1,1,1,1]");
        free_a(a); free_a(r);
    }
    {
        /* logical_not: INT32 */
        int d[] = {0, 5, -1};
        Array *a = make_i32_1d(d, 3);
        Array *r = logical_not(a);
        ASSERT_NOTNULL(r, "logical_not INT32 not null");
        uint8_t expected[] = {1, 0, 0};
        ASSERT_EQ_IARR((uint8_t *)r->data, expected, 3, "logical_not INT32 [0,5,-1] = [1,0,0]");
        free_a(a); free_a(r);
    }

    /* ── all ──────────────────────────────────────────────────────── */
    TEST_SECTION("all");

    {
        /* all: [1,1,1] axis=-1 keepdims=1 -> 1 (TRUE) */
        double d[] = {1, 1, 1};
        Array *a = make_f64_1d(d, 3);
        Array *r = all(a, -1, 1);
        ASSERT_NOTNULL(r, "all true not null");
        ASSERT_DTYPE(r, BOOL, "all true dtype BOOL");
        uint8_t *rd = (uint8_t *)r->data;
        ASSERT_EQ_INT(rd[0], 1, "all [1,1,1] = true");
        free_a(a); free_a(r);
    }
    {
        /* all: [1,0,1] -> 0 */
        double d[] = {1, 0, 1};
        Array *a = make_f64_1d(d, 3);
        Array *r = all(a, -1, 1);
        ASSERT_NOTNULL(r, "all with false not null");
        ASSERT_DTYPE(r, BOOL, "all with false dtype BOOL");
        uint8_t *rd = (uint8_t *)r->data;
        ASSERT_EQ_INT(rd[0], 0, "all [1,0,1] = false");
        free_a(a); free_a(r);
    }
    {
        /* all: 2D axis=0 */
        double d[] = {1, 0, 1, 1, 1, 1};
        Array *a = make_f64_2d(d, 2, 3);
        Array *r = all(a, 0, 0);
        ASSERT_NOTNULL(r, "all 2D axis=0 not null");
        ASSERT_DTYPE(r, BOOL, "all 2D axis=0 dtype BOOL");
        /* col0: [1,1]=1, col1: [0,1]=0, col2: [1,1]=1 */
        uint8_t expected[] = {1, 0, 1};
        ASSERT_EQ_IARR((uint8_t *)r->data, expected, 3, "all 2D axis=0 = [1,0,1]");
        free_a(a); free_a(r);
    }
    {
        /* all: INT32 */
        int d[] = {5, 0, 3};
        Array *a = make_i32_1d(d, 3);
        Array *r = all(a, -1, 1);
        ASSERT_NOTNULL(r, "all INT32 not null");
        ASSERT_DTYPE(r, BOOL, "all INT32 dtype BOOL");
        uint8_t *rd = (uint8_t *)r->data;
        ASSERT_EQ_INT(rd[0], 0, "all INT32 [5,0,3] = false");
        free_a(a); free_a(r);
    }

    /* ── any ──────────────────────────────────────────────────────── */
    TEST_SECTION("any");

    {
        /* any: [0,0,1] -> 1 */
        double d[] = {0, 0, 1};
        Array *a = make_f64_1d(d, 3);
        Array *r = any(a, -1, 1);
        ASSERT_NOTNULL(r, "any true not null");
        ASSERT_DTYPE(r, BOOL, "any dtype BOOL");
        uint8_t *rd = (uint8_t *)r->data;
        ASSERT_EQ_INT(rd[0], 1, "any [0,0,1] = true");
        free_a(a); free_a(r);
    }
    {
        /* any: [0,0,0] -> 0 */
        double d[] = {0, 0, 0};
        Array *a = make_f64_1d(d, 3);
        Array *r = any(a, -1, 1);
        ASSERT_NOTNULL(r, "any all false not null");
        uint8_t *rd = (uint8_t *)r->data;
        ASSERT_EQ_INT(rd[0], 0, "any [0,0,0] = false");
        free_a(a); free_a(r);
    }
    {
        /* any: 2D axis=0 */
        double d[] = {0, 0, 0, 0, 1, 0};
        Array *a = make_f64_2d(d, 2, 3);
        Array *r = any(a, 0, 0);
        ASSERT_NOTNULL(r, "any 2D axis=0 not null");
        ASSERT_DTYPE(r, BOOL, "any 2D axis=0 dtype BOOL");
        /* col0: [0,0]=0, col1: [0,1]=1, col2: [0,0]=0 */
        uint8_t expected[] = {0, 1, 0};
        ASSERT_EQ_IARR((uint8_t *)r->data, expected, 3, "any 2D axis=0 = [0,1,0]");
        free_a(a); free_a(r);
    }
    {
        /* any: 2D axis=1 */
        double d[] = {0, 0, 1, 0, 1, 0};
        Array *a = make_f64_2d(d, 2, 3);
        Array *r = any(a, 1, 0);
        ASSERT_NOTNULL(r, "any 2D axis=1 not null");
        ASSERT_DTYPE(r, BOOL, "any 2D axis=1 dtype BOOL");
        /* row0: [0,0,1]=1, row1: [0,1,0]=1 */
        uint8_t expected[] = {1, 1};
        ASSERT_EQ_IARR((uint8_t *)r->data, expected, 2, "any 2D axis=1 = [1,1]");
        free_a(a); free_a(r);
    }

    TEST_SUMMARY();
    return (g_fail > 0) ? 1 : 0;
}
