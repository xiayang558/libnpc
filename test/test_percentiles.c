/**
 * test_percentiles.c — Tests for percentile/quantile functions:
 *   percentile, quantile, nanpercentile, nanquantile
 */

#include "test_utils.h"

int main(void) {
    TEST_MAIN("Percentiles & Quantiles");

    double NAN_VAL = NAN;

    /* ── percentile ───────────────────────────────────────────────── */
    TEST_SECTION("percentile");

    {
        /* percentile: [1,2,3,4,5] q=[50] axis=-1 -> 3 */
        double data[] = {1, 2, 3, 4, 5};
        double qdata[] = {50};
        Array *a = make_f64_1d(data, 5);
        Array *q = make_f64_1d(qdata, 1);
        Array *r = percentile(a, q, -1);
        ASSERT_NOTNULL(r, "percentile q=50 not null");
        ASSERT_SIZE(r, 1, "percentile q=50 size=1");
        double *rd = (double *)r->data;
        ASSERT_EQ_DBL(rd[0], 3.0, TOL_F64, "percentile [1..5] q=50 = 3");
        free_a(a); free_a(q); free_a(r);
    }
    {
        /* percentile: [1,2,3,4,5] q=[0,50,100] -> [1,3,5] */
        double data[] = {1, 2, 3, 4, 5};
        double qdata[] = {0, 50, 100};
        Array *a = make_f64_1d(data, 5);
        Array *q = make_f64_1d(qdata, 3);
        Array *r = percentile(a, q, -1);
        ASSERT_NOTNULL(r, "percentile q=[0,50,100] not null");
        ASSERT_SIZE(r, 3, "percentile q=[0,50,100] size=3");
        double expected[] = {1, 3, 5};
        ASSERT_EQ_ARR((double *)r->data, expected, 3, TOL_F64, "percentile [1..5] q=[0,50,100]");
        free_a(a); free_a(q); free_a(r);
    }
    {
        /* percentile: single element */
        double data[] = {42};
        double qdata[] = {50};
        Array *a = make_f64_1d(data, 1);
        Array *q = make_f64_1d(qdata, 1);
        Array *r = percentile(a, q, -1);
        ASSERT_NOTNULL(r, "percentile single not null");
        double *rd = (double *)r->data;
        ASSERT_EQ_DBL(rd[0], 42.0, TOL_F64, "percentile [42] q=50 = 42");
        free_a(a); free_a(q); free_a(r);
    }
    {
        /* percentile: 2D with axis */
        double data[] = {1, 2, 3, 4, 5, 6};
        double qdata[] = {50};
        Array *a = create_array((int[]){2, 3}, 2, FLOAT64);
        memcpy(a->data, data, 6 * sizeof(double));
        Array *q = make_f64_1d(qdata, 1);
        Array *r = percentile(a, q, 0);
        ASSERT_NOTNULL(r, "percentile 2D axis=0 not null");
        /* column percentiles: col0=[1,4] 50th≈2.5, col1=[2,5] 50th≈3.5, col2=[3,6] 50th≈4.5 */
        double expected[] = {2.5, 3.5, 4.5};
        ASSERT_EQ_ARR((double *)r->data, expected, 3, TOL_F64, "percentile 2D axis=0 q=50");
        free_a(a); free_a(q); free_a(r);
    }

    /* ── quantile ─────────────────────────────────────────────────── */
    TEST_SECTION("quantile");

    {
        /* quantile: [1,2,3,4] q=[0.25,0.5,0.75]
         * Approx: 0.25 -> 1.75, 0.5 -> 2.5, 0.75 -> 3.25 */
        double data[] = {1, 2, 3, 4};
        double qdata[] = {0.25, 0.5, 0.75};
        Array *a = make_f64_1d(data, 4);
        Array *q = make_f64_1d(qdata, 3);
        Array *r = quantile(a, q, -1);
        ASSERT_NOTNULL(r, "quantile q=[0.25,0.5,0.75] not null");
        ASSERT_SIZE(r, 3, "quantile size=3");
        double *rd = (double *)r->data;
        ASSERT_EQ_DBL(rd[0], 1.75, TOL_F64, "quantile [1..4] q=0.25 = 1.75");
        ASSERT_EQ_DBL(rd[1], 2.5, TOL_F64, "quantile [1..4] q=0.5 = 2.5");
        ASSERT_EQ_DBL(rd[2], 3.25, TOL_F64, "quantile [1..4] q=0.75 = 3.25");
        free_a(a); free_a(q); free_a(r);
    }
    {
        /* quantile: q=0 -> min, q=1 -> max */
        double data[] = {5, 8, 3, 1, 9};
        double qdata[] = {0, 1};
        Array *a = make_f64_1d(data, 5);
        Array *q = make_f64_1d(qdata, 2);
        Array *r = quantile(a, q, -1);
        ASSERT_NOTNULL(r, "quantile q=[0,1] not null");
        double *rd = (double *)r->data;
        ASSERT_EQ_DBL(rd[0], 1.0, TOL_F64, "quantile q=0 = 1 (min)");
        ASSERT_EQ_DBL(rd[1], 9.0, TOL_F64, "quantile q=1 = 9 (max)");
        free_a(a); free_a(q); free_a(r);
    }

    /* ── nanpercentile ────────────────────────────────────────────── */
    TEST_SECTION("nanpercentile");

    {
        /* nanpercentile: [1, NAN, 3, NAN, 5] q=50 -> 3 */
        double data[] = {1, NAN, 3, NAN, 5};
        double qdata[] = {50};
        Array *a = make_f64_1d(data, 5);
        Array *q = make_f64_1d(qdata, 1);
        Array *r = nanpercentile(a, q, -1);
        ASSERT_NOTNULL(r, "nanpercentile q=50 not null");
        double *rd = (double *)r->data;
        ASSERT_EQ_DBL(rd[0], 3.0, TOL_F64, "nanpercentile [1,NAN,3,NAN,5] q=50 = 3");
        free_a(a); free_a(q); free_a(r);
    }
    {
        /* nanpercentile: all NaN */
        double data[] = {NAN, NAN};
        double qdata[] = {50};
        Array *a = make_f64_1d(data, 2);
        Array *q = make_f64_1d(qdata, 1);
        Array *r = nanpercentile(a, q, -1);
        ASSERT_NOTNULL(r, "nanpercentile all NaN not null");
        double *rd = (double *)r->data;
        ASSERT_TRUE(isnan(rd[0]), "nanpercentile all-NaN result is NaN");
        free_a(a); free_a(q); free_a(r);
    }

    /* ── nanquantile ──────────────────────────────────────────────── */
    TEST_SECTION("nanquantile");

    {
        /* nanquantile: [1, NAN, 2, NAN, 3, 4] q=0.5 -> 2.5 */
        double data[] = {1, NAN, 2, NAN, 3, 4};
        double qdata[] = {0.5};
        Array *a = make_f64_1d(data, 6);
        Array *q = make_f64_1d(qdata, 1);
        Array *r = nanquantile(a, q, -1);
        ASSERT_NOTNULL(r, "nanquantile q=0.5 not null");
        double *rd = (double *)r->data;
        /* valid values: [1,2,3,4], median = 2.5 */
        ASSERT_EQ_DBL(rd[0], 2.5, TOL_F64, "nanquantile [1,NAN,2,NAN,3,4] q=0.5 = 2.5");
        free_a(a); free_a(q); free_a(r);
    }
    {
        /* nanquantile: q=0 and q=1 */
        double data[] = {NAN, 5, NAN, 1, 9, NAN};
        double qdata[] = {0, 1};
        Array *a = make_f64_1d(data, 6);
        Array *q = make_f64_1d(qdata, 2);
        Array *r = nanquantile(a, q, -1);
        ASSERT_NOTNULL(r, "nanquantile q=[0,1] not null");
        double *rd = (double *)r->data;
        ASSERT_EQ_DBL(rd[0], 1.0, TOL_F64, "nanquantile q=0 = 1 (min)");
        ASSERT_EQ_DBL(rd[1], 9.0, TOL_F64, "nanquantile q=1 = 9 (max)");
        free_a(a); free_a(q); free_a(r);
    }

    TEST_SUMMARY();
    return (g_fail > 0) ? 1 : 0;
}
