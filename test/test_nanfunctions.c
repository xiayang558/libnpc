/**
 * test_nanfunctions.c — Tests for NaN-aware functions:
 *   nanmax, nanmin, nanmean, nanmedian, nancumsum, nancumprod,
 *   nanargmax, nanargmin, nan_to_num, isnan_array, isinf_array, isfinite_array
 */

#include "test_utils.h"

int main(void) {
    TEST_MAIN("NaN Functions");

    double NAN_VAL = NAN;
    double INF_VAL = INFINITY;

    /* ── nanmax ───────────────────────────────────────────────────── */
    TEST_SECTION("nanmax");

    {
        /* nanmax: [1, NAN, 3, NAN, 2] axis=-1 -> 3 */
        double data[] = {1, NAN, 3, NAN, 2};
        Array *a = make_f64_1d(data, 5);
        Array *r = nanmax(a, -1);
        ASSERT_NOTNULL(r, "nanmax 1D not null");
        double *rd = (double *)r->data;
        ASSERT_EQ_DBL(rd[0], 3.0, TOL_F64, "nanmax [1,NAN,3,NAN,2] = 3");
        free_a(a); free_a(r);
    }
    {
        /* nanmax: all-NaN [NAN, NAN] -> NaN */
        double data[] = {NAN, NAN};
        Array *a = make_f64_1d(data, 2);
        Array *r = nanmax(a, -1);
        ASSERT_NOTNULL(r, "nanmax all NaN not null");
        double *rd = (double *)r->data;
        ASSERT_TRUE(isnan(rd[0]), "nanmax all-NaN result is NaN");
        free_a(a); free_a(r);
    }
    {
        /* nanmax: no NaN */
        double data[] = {5, 1, 9, 3};
        Array *a = make_f64_1d(data, 4);
        Array *r = nanmax(a, -1);
        ASSERT_NOTNULL(r, "nanmax no NaN not null");
        double *rd = (double *)r->data;
        ASSERT_EQ_DBL(rd[0], 9.0, TOL_F64, "nanmax [5,1,9,3] = 9");
        free_a(a); free_a(r);
    }

    /* ── nanmin ───────────────────────────────────────────────────── */
    TEST_SECTION("nanmin");

    {
        /* nanmin: [1, NAN, 3, NAN, 2] -> 1 */
        double data[] = {1, NAN, 3, NAN, 2};
        Array *a = make_f64_1d(data, 5);
        Array *r = nanmin(a, -1);
        ASSERT_NOTNULL(r, "nanmin 1D not null");
        double *rd = (double *)r->data;
        ASSERT_EQ_DBL(rd[0], 1.0, TOL_F64, "nanmin [1,NAN,3,NAN,2] = 1");
        free_a(a); free_a(r);
    }
    {
        /* nanmin: all-NaN -> NaN */
        double data[] = {NAN, NAN};
        Array *a = make_f64_1d(data, 2);
        Array *r = nanmin(a, -1);
        ASSERT_NOTNULL(r, "nanmin all NaN not null");
        double *rd = (double *)r->data;
        ASSERT_TRUE(isnan(rd[0]), "nanmin all-NaN result is NaN");
        free_a(a); free_a(r);
    }
    {
        /* nanmin: mixed with negatives */
        double data[] = {-3, NAN, 7, NAN, -1};
        Array *a = make_f64_1d(data, 5);
        Array *r = nanmin(a, -1);
        ASSERT_NOTNULL(r, "nanmin negatives not null");
        double *rd = (double *)r->data;
        ASSERT_EQ_DBL(rd[0], -3.0, TOL_F64, "nanmin [-3,NAN,7,NAN,-1] = -3");
        free_a(a); free_a(r);
    }

    /* ── nanmean ──────────────────────────────────────────────────── */
    TEST_SECTION("nanmean");

    {
        /* nanmean: [1, NAN, 3] -> 2.0 */
        double data[] = {1, NAN, 3};
        Array *a = make_f64_1d(data, 3);
        Array *r = nanmean(a, -1);
        ASSERT_NOTNULL(r, "nanmean 1D not null");
        double *rd = (double *)r->data;
        ASSERT_EQ_DBL(rd[0], 2.0, TOL_F64, "nanmean [1,NAN,3] = 2.0");
        free_a(a); free_a(r);
    }
    {
        /* nanmean: no NaN */
        double data[] = {2, 4, 6};
        Array *a = make_f64_1d(data, 3);
        Array *r = nanmean(a, -1);
        ASSERT_NOTNULL(r, "nanmean no NaN not null");
        double *rd = (double *)r->data;
        ASSERT_EQ_DBL(rd[0], 4.0, TOL_F64, "nanmean [2,4,6] = 4.0");
        free_a(a); free_a(r);
    }
    {
        /* nanmean: all NaN -> NaN */
        double data[] = {NAN, NAN};
        Array *a = make_f64_1d(data, 2);
        Array *r = nanmean(a, -1);
        ASSERT_NOTNULL(r, "nanmean all NaN not null");
        double *rd = (double *)r->data;
        ASSERT_TRUE(isnan(rd[0]), "nanmean all-NaN result is NaN");
        free_a(a); free_a(r);
    }

    /* ── nanmedian ────────────────────────────────────────────────── */
    TEST_SECTION("nanmedian");

    {
        /* nanmedian: [1, NAN, 3, NAN, 5] -> 3.0 */
        double data[] = {1, NAN, 3, NAN, 5};
        Array *a = make_f64_1d(data, 5);
        Array *r = nanmedian(a, -1);
        ASSERT_NOTNULL(r, "nanmedian 1D not null");
        double *rd = (double *)r->data;
        ASSERT_EQ_DBL(rd[0], 3.0, TOL_F64, "nanmedian [1,NAN,3,NAN,5] = 3.0");
        free_a(a); free_a(r);
    }
    {
        /* nanmedian: even count after NaN removal [1, NAN, 2, 3] -> (1+2+3)/3? no:
         * valid values: [1,2,3] -> median=2 */
        double data[] = {1, NAN, 2, 3};
        Array *a = make_f64_1d(data, 4);
        Array *r = nanmedian(a, -1);
        ASSERT_NOTNULL(r, "nanmedian even valid not null");
        double *rd = (double *)r->data;
        ASSERT_EQ_DBL(rd[0], 2.0, TOL_F64, "nanmedian [1,NAN,2,3] = 2.0");
        free_a(a); free_a(r);
    }

    /* ── nancumsum ────────────────────────────────────────────────── */
    TEST_SECTION("nancumsum");

    {
        /* nancumsum: [1, NAN, 2, NAN, 3] -> [1, 1, 3, 3, 6] */
        double data[] = {1, NAN, 2, NAN, 3};
        double expected[] = {1, 1, 3, 3, 6};
        Array *a = make_f64_1d(data, 5);
        Array *r = nancumsum(a, -1);
        ASSERT_NOTNULL(r, "nancumsum 1D not null");
        ASSERT_SIZE(r, 5, "nancumsum 1D size=5");
        ASSERT_EQ_ARR((double *)r->data, expected, 5, TOL_F64, "nancumsum [1,NAN,2,NAN,3]");
        free_a(a); free_a(r);
    }
    {
        /* nancumsum: leading NaN [NAN, NAN, 1, 2] -> [0, 0, 1, 3] */
        double data[] = {NAN, NAN, 1, 2};
        Array *a = make_f64_1d(data, 4);
        Array *r = nancumsum(a, -1);
        ASSERT_NOTNULL(r, "nancumsum leading NaN not null");
        double expected[] = {0, 0, 1, 3};
        ASSERT_EQ_ARR((double *)r->data, expected, 4, TOL_F64, "nancumsum leading NaN = [0,0,1,3]");
        free_a(a); free_a(r);
    }

    /* ── nancumprod ───────────────────────────────────────────────── */
    TEST_SECTION("nancumprod");

    {
        /* nancumprod: [2, NAN, 3] -> [2, 2, 6] */
        double data[] = {2, NAN, 3};
        double expected[] = {2, 2, 6};
        Array *a = make_f64_1d(data, 3);
        Array *r = nancumprod(a, -1);
        ASSERT_NOTNULL(r, "nancumprod 1D not null");
        ASSERT_SIZE(r, 3, "nancumprod 1D size=3");
        ASSERT_EQ_ARR((double *)r->data, expected, 3, TOL_F64, "nancumprod [2,NAN,3] = [2,2,6]");
        free_a(a); free_a(r);
    }
    {
        /* nancumprod: leading NaN [NAN, 3, 4] -> [1, 3, 12] */
        double data[] = {NAN, 3, 4};
        Array *a = make_f64_1d(data, 3);
        Array *r = nancumprod(a, -1);
        ASSERT_NOTNULL(r, "nancumprod leading NaN not null");
        double expected[] = {1, 3, 12};
        ASSERT_EQ_ARR((double *)r->data, expected, 3, TOL_F64, "nancumprod leading NaN = [1,3,12]");
        free_a(a); free_a(r);
    }

    /* ── nanargmax ────────────────────────────────────────────────── */
    TEST_SECTION("nanargmax");

    {
        /* nanargmax: [1, NAN, 5, NAN, 2] -> 2 (index of 5) */
        double data[] = {1, NAN, 5, NAN, 2};
        Array *a = make_f64_1d(data, 5);
        Array *r = nanargmax(a, -1);
        ASSERT_NOTNULL(r, "nanargmax 1D not null");
        int64_t *rd = (int64_t *)r->data;
        ASSERT_EQ_I64(rd[0], 2, "nanargmax [1,NAN,5,NAN,2] = 2");
        free_a(a); free_a(r);
    }
    {
        /* nanargmax: no NaN */
        double data[] = {3, 7, 2};
        Array *a = make_f64_1d(data, 3);
        Array *r = nanargmax(a, -1);
        ASSERT_NOTNULL(r, "nanargmax no NaN not null");
        int64_t *rd = (int64_t *)r->data;
        ASSERT_EQ_I64(rd[0], 1, "nanargmax [3,7,2] = 1");
        free_a(a); free_a(r);
    }

    /* ── nanargmin ────────────────────────────────────────────────── */
    TEST_SECTION("nanargmin");

    {
        /* nanargmin: [5, NAN, 1, NAN, 3] -> 2 (index of 1) */
        double data[] = {5, NAN, 1, NAN, 3};
        Array *a = make_f64_1d(data, 5);
        Array *r = nanargmin(a, -1);
        ASSERT_NOTNULL(r, "nanargmin 1D not null");
        int64_t *rd = (int64_t *)r->data;
        ASSERT_EQ_I64(rd[0], 2, "nanargmin [5,NAN,1,NAN,3] = 2");
        free_a(a); free_a(r);
    }
    {
        /* nanargmin: no NaN */
        double data[] = {9, 1, 7};
        Array *a = make_f64_1d(data, 3);
        Array *r = nanargmin(a, -1);
        ASSERT_NOTNULL(r, "nanargmin no NaN not null");
        int64_t *rd = (int64_t *)r->data;
        ASSERT_EQ_I64(rd[0], 1, "nanargmin [9,1,7] = 1");
        free_a(a); free_a(r);
    }

    /* ── nan_to_num ───────────────────────────────────────────────── */
    TEST_SECTION("nan_to_num");

    {
        /* nan_to_num: [1, NAN, INF, -INF, 2]
         * NaN->0, INF->large_num, -INF->-large_num */
        double data[] = {1, NAN, INF_VAL, -INF_VAL, 2};
        Array *a = make_f64_1d(data, 5);
        Array *r = nan_to_num(a);
        ASSERT_NOTNULL(r, "nan_to_num not null");
        ASSERT_SIZE(r, 5, "nan_to_num size=5");
        double *rd = (double *)r->data;
        ASSERT_EQ_DBL(rd[0], 1.0, TOL_F64, "nan_to_num idx 0 = 1");
        ASSERT_EQ_DBL(rd[1], 0.0, TOL_F64, "nan_to_num idx 1 NaN -> 0");
        ASSERT_TRUE(isfinite(rd[2]) && rd[2] > 1e300, "nan_to_num idx 2 INF -> large");
        ASSERT_TRUE(isfinite(rd[3]) && rd[3] < -1e300, "nan_to_num idx 3 -INF -> -large");
        ASSERT_EQ_DBL(rd[4], 2.0, TOL_F64, "nan_to_num idx 4 = 2");
        free_a(a); free_a(r);
    }

    /* ── isnan_array ──────────────────────────────────────────────── */
    TEST_SECTION("isnan_array");

    {
        /* isnan_array: [1, NAN, 2] -> [0, 1, 0] (BOOL) */
        double data[] = {1, NAN, 2};
        Array *a = make_f64_1d(data, 3);
        Array *r = isnan_array(a);
        ASSERT_NOTNULL(r, "isnan_array not null");
        ASSERT_DTYPE(r, BOOL, "isnan_array output dtype BOOL");
        ASSERT_SIZE(r, 3, "isnan_array size=3");
        uint8_t expected[] = {0, 1, 0};
        ASSERT_EQ_IARR((uint8_t *)r->data, expected, 3, "isnan_array [1,NAN,2] = [0,1,0]");
        free_a(a); free_a(r);
    }
    {
        /* isnan_array: no NaN */
        double data[] = {3, 5, 7};
        Array *a = make_f64_1d(data, 3);
        Array *r = isnan_array(a);
        ASSERT_NOTNULL(r, "isnan_array no NaN not null");
        uint8_t expected[] = {0, 0, 0};
        ASSERT_EQ_IARR((uint8_t *)r->data, expected, 3, "isnan_array [3,5,7] = [0,0,0]");
        free_a(a); free_a(r);
    }

    /* ── isinf_array ──────────────────────────────────────────────── */
    TEST_SECTION("isinf_array");

    {
        /* isinf_array: [1, INF, 2] -> [0, 1, 0] */
        double data[] = {1, INF_VAL, 2};
        Array *a = make_f64_1d(data, 3);
        Array *r = isinf_array(a);
        ASSERT_NOTNULL(r, "isinf_array not null");
        ASSERT_DTYPE(r, BOOL, "isinf_array output dtype BOOL");
        uint8_t expected[] = {0, 1, 0};
        ASSERT_EQ_IARR((uint8_t *)r->data, expected, 3, "isinf_array [1,INF,2] = [0,1,0]");
        free_a(a); free_a(r);
    }
    {
        /* isinf_array: negative infinity */
        double data[] = {0, -INF_VAL, 5};
        Array *a = make_f64_1d(data, 3);
        Array *r = isinf_array(a);
        ASSERT_NOTNULL(r, "isinf_array neg inf not null");
        uint8_t expected[] = {0, 1, 0};
        ASSERT_EQ_IARR((uint8_t *)r->data, expected, 3, "isinf_array [0,-INF,5] = [0,1,0]");
        free_a(a); free_a(r);
    }

    /* ── isfinite_array ───────────────────────────────────────────── */
    TEST_SECTION("isfinite_array");

    {
        /* isfinite_array: [1, INF, NAN, 2] -> [1, 0, 0, 1] */
        double data[] = {1, INF_VAL, NAN, 2};
        Array *a = make_f64_1d(data, 4);
        Array *r = isfinite_array(a);
        ASSERT_NOTNULL(r, "isfinite_array not null");
        ASSERT_DTYPE(r, BOOL, "isfinite_array output dtype BOOL");
        uint8_t expected[] = {1, 0, 0, 1};
        ASSERT_EQ_IARR((uint8_t *)r->data, expected, 4, "isfinite_array [1,INF,NAN,2] = [1,0,0,1]");
        free_a(a); free_a(r);
    }
    {
        /* isfinite_array: all finite */
        double data[] = {0, -1, 2.5, 100};
        Array *a = make_f64_1d(data, 4);
        Array *r = isfinite_array(a);
        ASSERT_NOTNULL(r, "isfinite_array all finite not null");
        uint8_t expected[] = {1, 1, 1, 1};
        ASSERT_EQ_IARR((uint8_t *)r->data, expected, 4, "isfinite_array all finite = [1,1,1,1]");
        free_a(a); free_a(r);
    }

    TEST_SUMMARY();
    return (g_fail > 0) ? 1 : 0;
}
