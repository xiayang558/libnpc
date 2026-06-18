/**
 * test_datetime.c — Tests for datetime_now, datetime_from_string,
 *                   datetime_as_string, datetime_add_days, datetime_sub_days,
 *                   datetime_eq, datetime_lt, datetime_gt, datetime_to_ns,
 *                   is_busday
 */
#include <string.h>
#include "test_utils.h"
#include "datetime64.h"

int main(void) {
    TEST_MAIN("datetime_now / datetime_from_string / datetime_as_string / "
              "datetime_add_days / datetime_sub_days / datetime_eq / "
              "datetime_lt / datetime_gt / datetime_to_ns / is_busday");

    /* ── datetime_now ───────────────────────────────────────────────── */
    {
        DateTime64 now = datetime_now();
        ASSERT_TRUE(now.value > 0, "datetime_now has positive value");
    }

    /* ── datetime_from_string ───────────────────────────────────────── */
    {
        DateTime64 dt = datetime_from_string("2025-01-15", DT_UNIT_D);
        ASSERT_TRUE(dt.value > 0, "datetime_from_string 2025-01-15");
    }

    /* ── datetime_as_string ─────────────────────────────────────────── */
    {
        DateTime64 dt = datetime_from_string("2025-01-15", DT_UNIT_D);
        char *s = datetime_as_string(&dt);
        ASSERT_TRUE(s != NULL, "datetime_as_string returns non-NULL");
        free(s);
    }

    /* ── datetime_add_days ──────────────────────────────────────────── */
    {
        DateTime64 dt = datetime_from_string("2025-01-15", DT_UNIT_D);
        DateTime64 dt2 = datetime_add_days(dt, 5);
        char *s = datetime_as_string(&dt2);
        ASSERT_TRUE(s != NULL, "datetime_add_days +5");
        if (s) {
            /* check that the result contains "2025-01-20" */
            ASSERT_TRUE(strstr(s, "2025-01-20") != NULL ||
                        strstr(s, "01-20") != NULL,
                        "datetime_add_days = 2025-01-20");
            free(s);
        }
    }

    /* ── datetime_sub_days ──────────────────────────────────────────── */
    {
        DateTime64 dt = datetime_from_string("2025-01-15", DT_UNIT_D);
        DateTime64 dt2 = datetime_sub_days(dt, 5);
        char *s = datetime_as_string(&dt2);
        ASSERT_TRUE(s != NULL, "datetime_sub_days -5");
        if (s) {
            ASSERT_TRUE(strstr(s, "2025-01-10") != NULL ||
                        strstr(s, "01-10") != NULL,
                        "datetime_sub_days = 2025-01-10");
            free(s);
        }
    }

    /* ── datetime_eq ────────────────────────────────────────────────── */
    {
        DateTime64 a = datetime_from_string("2025-01-15", DT_UNIT_D);
        DateTime64 b = datetime_from_string("2025-01-15", DT_UNIT_D);
        DateTime64 c = datetime_from_string("2025-01-16", DT_UNIT_D);
        ASSERT_TRUE(datetime_eq(a, b), "datetime_eq same dates true");
        ASSERT_TRUE(!datetime_eq(a, c), "datetime_eq different dates false");
    }

    /* ── datetime_lt ────────────────────────────────────────────────── */
    {
        DateTime64 a = datetime_from_string("2025-01-15", DT_UNIT_D);
        DateTime64 b = datetime_from_string("2025-01-20", DT_UNIT_D);
        ASSERT_TRUE(datetime_lt(a, b), "datetime_lt 15 < 20");
        ASSERT_TRUE(!datetime_lt(b, a), "datetime_lt 20 < 15 false");
    }

    /* ── datetime_gt ────────────────────────────────────────────────── */
    {
        DateTime64 a = datetime_from_string("2025-01-15", DT_UNIT_D);
        DateTime64 b = datetime_from_string("2025-01-20", DT_UNIT_D);
        ASSERT_TRUE(datetime_gt(b, a), "datetime_gt 20 > 15");
        ASSERT_TRUE(!datetime_gt(a, b), "datetime_gt 15 > 20 false");
    }

    /* ── datetime_to_ns ─────────────────────────────────────────────── */
    {
        DateTime64 dt = datetime_from_string("2025-01-15", DT_UNIT_D);
        int64_t ns = datetime_to_ns(dt);
        ASSERT_TRUE(ns > 0, "datetime_to_ns positive");
    }

    /* ── is_busday with default weekmask ────────────────────────────── */
    /* Jan 13, 2025 is a Monday */
    {
        /* Create a DATETIME64 array */
        int shape[] = {2};
        Array *dates = create_array(shape, 1, DATETIME64);
        ASSERT_NOTNULL(dates, "create DATETIME64 array");
        if (dates) {
            DateTime64 *dts = (DateTime64*)dates->data;
            dts[0] = datetime_from_string("2025-01-13", DT_UNIT_D); /* Monday */
            dts[1] = datetime_from_string("2025-01-18", DT_UNIT_D); /* Saturday */

            Array *r = is_busday(dates, NULL, NULL); /* default weekmask "1111100" */
            ASSERT_NOTNULL(r, "is_busday Mon+Sat default");
            if (r) {
                ASSERT_DTYPE(r, BOOL, "is_busday dtype BOOL");
                ASSERT_SIZE(r, 2, "is_busday size=2");
                int *bd = (int*)r->data;
                ASSERT_EQ_INT(bd[0], 1, "Monday is busday");
                ASSERT_EQ_INT(bd[1], 0, "Saturday is not busday");
            }
            free_a(dates); free_a(r);
        }
    }

    /* ── is_busday with explicit weekmask ────────────────────────────── */
    {
        int shape[] = {1};
        Array *dates = create_array(shape, 1, DATETIME64);
        ASSERT_NOTNULL(dates, "create DATETIME64 array for weekmask test");
        if (dates) {
            DateTime64 *dts = (DateTime64*)dates->data;
            dts[0] = datetime_from_string("2025-01-13", DT_UNIT_D); /* Monday */

            /* weekmask "1111111" - all days are business days */
            Array *r = is_busday(dates, "1111111", NULL);
            ASSERT_NOTNULL(r, "is_busday Monday weekmask=1111111");
            if (r) {
                ASSERT_EQ_INT(((int*)r->data)[0], 1, "Monday with 1111111");
            }
            free_a(r);

            /* weekmask "0000000" - no days are business days */
            r = is_busday(dates, "0000000", NULL);
            ASSERT_NOTNULL(r, "is_busday Monday weekmask=0000000");
            if (r) {
                ASSERT_EQ_INT(((int*)r->data)[0], 0, "Monday with 0000000");
            }
            free_a(r);
            free_a(dates);
        }
    }

    /* ── is_busday NULL weekmask uses default ───────────────────────── */
    {
        int shape[] = {1};
        Array *dates = create_array(shape, 1, DATETIME64);
        ASSERT_NOTNULL(dates, "create DATETIME64 array NULL weekmask");
        if (dates) {
            DateTime64 *dts = (DateTime64*)dates->data;
            dts[0] = datetime_from_string("2025-01-14", DT_UNIT_D); /* Tuesday */
            Array *r = is_busday(dates, NULL, NULL);
            ASSERT_NOTNULL(r, "is_busday NULL weekmask -> default");
            if (r) {
                ASSERT_EQ_INT(((int*)r->data)[0], 1, "Tuesday with default weekmask");
            }
            free_a(dates); free_a(r);
        }
    }

    /* ── datetime_from_string with time ─────────────────────────────── */
    {
        DateTime64 dt = datetime_from_string("2025-01-15T12:00:00", DT_UNIT_S);
        ASSERT_TRUE(dt.value > 0, "datetime_from_string with time");
        char *s = datetime_as_string(&dt);
        ASSERT_TRUE(s != NULL, "datetime_as_string with time");
        free(s);
    }

    TEST_SUMMARY();
    return (g_fail > 0) ? 1 : 0;
}
