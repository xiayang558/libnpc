/**
 * test_corr_cov.c — Tests for correlation/covariance/convolution:
 *   corrcoef, cov, correlate, convolve
 */

#include "test_utils.h"

int main(void) {
    TEST_MAIN("Correlation & Covariance");

    /* ── cov ──────────────────────────────────────────────────────── */
    TEST_SECTION("cov");

    {
        /* cov: 2D data [[1,2,3],[4,5,6]] rowvar=1 (rows=variables)
         * Var1 = [1,2,3], Var2 = [4,5,6]
         * Both have same variance: mean1=2, var1=((1-2)^2+(2-2)^2+(3-2)^2)/2 = 1
         * mean2=5, var2=((4-5)^2+(5-5)^2+(6-5)^2)/2 = 1
         * covariance = ((1-2)*(4-5)+(2-2)*(5-5)+(3-2)*(6-5))/2 = (1+0+1)/2 = 1
         * Result: [[1, 1], [1, 1]] */
        double data[] = {1, 2, 3, 4, 5, 6};
        Array *a = make_f64_2d(data, 2, 3);
        Array *r = cov(a, 1, 1, FLOAT64);
        ASSERT_NOTNULL(r, "cov rowvar=1 ddof=1 not null");
        ASSERT_NDIM(r, 2, "cov result ndim=2");
        ASSERT_EQ_INT(r->shape[0], 2, "cov shape[0]=2");
        ASSERT_EQ_INT(r->shape[1], 2, "cov shape[1]=2");
        ASSERT_DTYPE(r, FLOAT64, "cov dtype=FLOAT64");
        double *rd = (double *)r->data;
        ASSERT_EQ_DBL(rd[0], 1.0, TOL_F64, "cov[0][0] = 1.0");
        ASSERT_EQ_DBL(rd[1], 1.0, TOL_F64, "cov[0][1] = 1.0");
        ASSERT_EQ_DBL(rd[2], 1.0, TOL_F64, "cov[1][0] = 1.0");
        ASSERT_EQ_DBL(rd[3], 1.0, TOL_F64, "cov[1][1] = 1.0");
        free_a(a); free_a(r);
    }
    {
        /* cov: with ddof=0 */
        double data[] = {1, 2, 3, 4, 5, 6};
        Array *a = make_f64_2d(data, 2, 3);
        Array *r = cov(a, 1, 0, FLOAT64);
        ASSERT_NOTNULL(r, "cov ddof=0 not null");
        double *rd = (double *)r->data;
        /* var=(2/3) each, cov=(2/3) */
        ASSERT_EQ_DBL(rd[0], 2.0/3.0, TOL_F64, "cov ddof=0 var1 = 2/3");
        ASSERT_EQ_DBL(rd[3], 2.0/3.0, TOL_F64, "cov ddof=0 var2 = 2/3");
        free_a(a); free_a(r);
    }

    /* ── corrcoef ─────────────────────────────────────────────────── */
    TEST_SECTION("corrcoef");

    {
        /* corrcoef: [[1,2,3],[4,5,6]] rowvar=1
         * Both rows are perfectly correlated (y = x + 3)
         * Result: [[1, 1], [1, 1]] */
        double data[] = {1, 2, 3, 4, 5, 6};
        Array *a = make_f64_2d(data, 2, 3);
        Array *r = corrcoef(a, 1);
        ASSERT_NOTNULL(r, "corrcoef rowvar=1 not null");
        ASSERT_NDIM(r, 2, "corrcoef ndim=2");
        ASSERT_EQ_INT(r->shape[0], 2, "corrcoef shape[0]=2");
        ASSERT_EQ_INT(r->shape[1], 2, "corrcoef shape[1]=2");
        double *rd = (double *)r->data;
        /* diagonal should be 1.0 */
        ASSERT_EQ_DBL(rd[0], 1.0, TOL_F64, "corrcoef diag[0] = 1.0");
        ASSERT_EQ_DBL(rd[3], 1.0, TOL_F64, "corrcoef diag[1] = 1.0");
        /* off-diagonal for perfect correlation */
        ASSERT_EQ_DBL(rd[1], 1.0, TOL_F64, "corrcoef [0][1] = 1.0 (perfect corr)");
        ASSERT_EQ_DBL(rd[2], 1.0, TOL_F64, "corrcoef [1][0] = 1.0 (perfect corr)");
        free_a(a); free_a(r);
    }
    {
        /* corrcoef: anti-correlated [[1,2,3],[3,2,1]] rowvar=1
         * y = -x + 4, perfectly negative correlation */
        double data[] = {1, 2, 3, 3, 2, 1};
        Array *a = make_f64_2d(data, 2, 3);
        Array *r = corrcoef(a, 1);
        ASSERT_NOTNULL(r, "corrcoef anti-corr not null");
        double *rd = (double *)r->data;
        ASSERT_EQ_DBL(rd[0], 1.0, TOL_F64, "corrcoef anti-corr diag[0] = 1");
        ASSERT_EQ_DBL(rd[3], 1.0, TOL_F64, "corrcoef anti-corr diag[1] = 1");
        ASSERT_EQ_DBL(rd[1], -1.0, TOL_F64, "corrcoef anti-corr off-diag = -1");
        free_a(a); free_a(r);
    }

    /* ── correlate ────────────────────────────────────────────────── */
    TEST_SECTION("correlate");

    {
        /* correlate: [1,2,3] and [0,1,0.5] mode="same"
         * Output shape should match first input */
        double adata[] = {1, 2, 3};
        double vdata[] = {0, 1, 0.5};
        Array *a = make_f64_1d(adata, 3);
        Array *v = make_f64_1d(vdata, 3);
        Array *r = correlate(a, v, "same");
        ASSERT_NOTNULL(r, "correlate mode=same not null");
        ASSERT_NDIM(r, 1, "correlate ndim=1");
        ASSERT_SIZE(r, 3, "correlate size matches input");
        /* correlation (not convolution): cross-correlation
         * for mode="same", output is centered
         * correlate(a,v) = sum_k a[k+n] * v[k]
         * k=0: a0*v0 + a1*v1 + a2*v2 = 1*0 + 2*1 + 3*0.5 = 0+2+1.5 = 3.5
         * k=-1: a0*v1 + a1*v2 = 1*1 + 2*0.5 = 1+1 = 2
         * k=1: a1*v0 + a2*v1 = 2*0 + 3*1 = 3
         * Result: [2, 3.5, 3] */
        double *rd = (double *)r->data;
        double expected[] = {2.0, 3.5, 3.0};
        ASSERT_EQ_ARR(rd, expected, 3, TOL_F64, "correlate mode=same result");
        free_a(a); free_a(v); free_a(r);
    }

    /* ── convolve ─────────────────────────────────────────────────── */
    TEST_SECTION("convolve");

    {
        /* convolve: [1,2,3] * [0,1,0.5] = [0, 1, 2.5, 4, 1.5] (mode="full")
         * convolution: y[n] = sum_k a[k] * v[n-k]
         * n=0: a0*v0 = 1*0 = 0
         * n=1: a0*v1 + a1*v0 = 1*1 + 2*0 = 1
         * n=2: a0*v2 + a1*v1 + a2*v0 = 1*0.5 + 2*1 + 3*0 = 2.5
         * n=3: a1*v2 + a2*v1 = 2*0.5 + 3*1 = 4
         * n=4: a2*v2 = 3*0.5 = 1.5 */
        double adata[] = {1, 2, 3};
        double vdata[] = {0, 1, 0.5};
        Array *a = make_f64_1d(adata, 3);
        Array *v = make_f64_1d(vdata, 3);
        Array *r = convolve(a, v);
        ASSERT_NOTNULL(r, "convolve not null");
        ASSERT_NDIM(r, 1, "convolve ndim=1");
        ASSERT_SIZE(r, 5, "convolve size=5 (full)");
        double expected[] = {0, 1, 2.5, 4, 1.5};
        ASSERT_EQ_ARR((double *)r->data, expected, 5, TOL_F64, "convolve [1,2,3]*[0,1,0.5]");
        free_a(a); free_a(v); free_a(r);
    }
    {
        /* convolve: [1,1,1] * [1,1,1] = [1, 2, 3, 2, 1] */
        double adata[] = {1, 1, 1};
        double vdata[] = {1, 1, 1};
        Array *a = make_f64_1d(adata, 3);
        Array *v = make_f64_1d(vdata, 3);
        Array *r = convolve(a, v);
        ASSERT_NOTNULL(r, "convolve [1,1,1]*[1,1,1] not null");
        ASSERT_SIZE(r, 5, "convolve [1,1,1]*[1,1,1] size=5");
        double expected[] = {1, 2, 3, 2, 1};
        ASSERT_EQ_ARR((double *)r->data, expected, 5, TOL_F64, "convolve [1,1,1]*[1,1,1] = [1,2,3,2,1]");
        free_a(a); free_a(v); free_a(r);
    }
    {
        /* convolve: different lengths: [2,3] * [1,4] -> [2, 11, 12] */
        double adata[] = {2, 3};
        double vdata[] = {1, 4};
        Array *a = make_f64_1d(adata, 2);
        Array *v = make_f64_1d(vdata, 2);
        Array *r = convolve(a, v);
        ASSERT_NOTNULL(r, "convolve different lengths not null");
        ASSERT_SIZE(r, 3, "convolve [2,3]*[1,4] size=3");
        /* n=0: 2*1=2, n=1: 2*4+3*1=11, n=2: 3*4=12 */
        double expected[] = {2, 11, 12};
        ASSERT_EQ_ARR((double *)r->data, expected, 3, TOL_F64, "convolve [2,3]*[1,4] = [2,11,12]");
        free_a(a); free_a(v); free_a(r);
    }

    TEST_SUMMARY();
    return (g_fail > 0) ? 1 : 0;
}
