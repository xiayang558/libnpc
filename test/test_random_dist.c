/**
 * test_random_dist.c — Tests for continuous distribution functions.
 *
 * Covers: random_gamma, random_chisquare, random_beta, random_standard_t,
 * random_f, random_laplace, random_logistic, random_lognormal,
 * random_pareto, random_power, random_rayleigh, random_weibull,
 * random_standard_cauchy, random_gumbel, random_triangular,
 * random_vonmises, random_wald
 */

#include "test_utils.h"
#include "random.h"
#include <math.h>

/* ─── Utility: empirical mean ───────────────────────────────────────── */
static double empirical_mean(const double *data, int n) {
    double s = 0;
    for (int i = 0; i < n; i++) s += data[i];
    return s / n;
}

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

int main(void) {
    TEST_MAIN("Random Distributions");

    /* ─── random_gamma ──────────────────────────────────────────────── */
    TEST_SECTION("random_gamma");

    {
        random_seed(42);
        Array *a = random_gamma(2.0, 1.0, (int[]){500}, 1);
        ASSERT_NOTNULL(a, "gamma(shape=2,scale=1,[500]) → not NULL");
        ASSERT_SIZE(a, 500, "gamma shape check");

        double *d = (double*)a->data;
        int all_pos = 1;
        for (int i = 0; i < 500; i++)
            if (d[i] <= 0.0) { all_pos = 0; break; }
        ASSERT_TRUE(all_pos, "gamma all > 0");

        double m = empirical_mean(d, 500);
        ASSERT_TRUE(fabs(m - 2.0) < 0.3, "gamma mean ~ shape*scale = 2");
        free_a(a);
    }

    /* ─── random_chisquare ───────────────────────────────────────────── */
    TEST_SECTION("random_chisquare");

    {
        random_seed(42);
        Array *a = random_chisquare(4.0, (int[]){500}, 1);
        ASSERT_NOTNULL(a, "chisquare(df=4,[500]) → not NULL");

        double *d = (double*)a->data;
        int all_nonneg = 1;
        for (int i = 0; i < 500; i++)
            if (d[i] < 0.0) { all_nonneg = 0; break; }
        ASSERT_TRUE(all_nonneg, "chisquare all >= 0");

        double m = empirical_mean(d, 500);
        ASSERT_TRUE(fabs(m - 4.0) < 0.5, "chisquare(df=4) mean ~ 4");
        free_a(a);
    }

    /* ─── random_beta ────────────────────────────────────────────────── */
    TEST_SECTION("random_beta");

    {
        random_seed(42);
        Array *a = random_beta(2.0, 2.0, (int[]){500}, 1);
        ASSERT_NOTNULL(a, "beta(a=2,b=2,[500]) → not NULL");

        double *d = (double*)a->data;
        int in_range = 1;
        for (int i = 0; i < 500; i++)
            if (d[i] < 0.0 || d[i] > 1.0) { in_range = 0; break; }
        ASSERT_TRUE(in_range, "beta all in [0,1]");

        double m = empirical_mean(d, 500);
        ASSERT_TRUE(fabs(m - 0.5) < 0.1,
                    "beta(a=2,b=2) mean ~ a/(a+b) = 0.5");
        free_a(a);
    }

    /* ─── random_standard_t ──────────────────────────────────────────── */
    TEST_SECTION("random_standard_t");

    {
        random_seed(42);
        Array *a = random_standard_t(10.0, (int[]){500}, 1);
        ASSERT_NOTNULL(a, "standard_t(df=10,[500]) → not NULL");

        double *d = (double*)a->data;
        double m = empirical_mean(d, 500);
        /* symmetric about 0 */
        ASSERT_TRUE(fabs(m) < 0.2, "standard_t(df=10) mean ~ 0");
        free_a(a);
    }

    /* ─── random_f ───────────────────────────────────────────────────── */
    TEST_SECTION("random_f");

    {
        random_seed(42);
        Array *a = random_f(5.0, 5.0, (int[]){300}, 1);
        ASSERT_NOTNULL(a, "f(dfn=5,dfd=5,[300]) → not NULL");

        double *d = (double*)a->data;
        int all_pos = 1;
        for (int i = 0; i < 300; i++)
            if (d[i] <= 0.0) { all_pos = 0; break; }
        ASSERT_TRUE(all_pos, "f all > 0");
        free_a(a);
    }

    /* ─── random_laplace ─────────────────────────────────────────────── */
    TEST_SECTION("random_laplace");

    {
        random_seed(42);
        Array *a = random_laplace(0.0, 1.0, (int[]){500}, 1);
        ASSERT_NOTNULL(a, "laplace(loc=0,scale=1,[500]) → not NULL");

        double *d = (double*)a->data;
        double m = empirical_mean(d, 500);
        ASSERT_TRUE(fabs(m) < 0.2, "laplace(0,1) mean ~ 0");
        free_a(a);
    }

    /* ─── random_logistic ────────────────────────────────────────────── */
    TEST_SECTION("random_logistic");

    {
        random_seed(42);
        Array *a = random_logistic(0.0, 1.0, (int[]){500}, 1);
        ASSERT_NOTNULL(a, "logistic(loc=0,scale=1,[500]) → not NULL");

        double *d = (double*)a->data;
        double m = empirical_mean(d, 500);
        ASSERT_TRUE(fabs(m) < 0.2, "logistic(0,1) mean ~ 0");
        free_a(a);
    }

    /* ─── random_lognormal ───────────────────────────────────────────── */
    TEST_SECTION("random_lognormal");

    {
        random_seed(42);
        Array *a = random_lognormal(0.0, 1.0, (int[]){300}, 1);
        ASSERT_NOTNULL(a, "lognormal(mean=0,sigma=1,[300]) → not NULL");

        double *d = (double*)a->data;
        int all_pos = 1;
        for (int i = 0; i < 300; i++)
            if (d[i] <= 0.0) { all_pos = 0; break; }
        ASSERT_TRUE(all_pos, "lognormal all > 0");
        free_a(a);
    }

    /* ─── random_pareto ──────────────────────────────────────────────── */
    TEST_SECTION("random_pareto");

    {
        random_seed(42);
        Array *a = random_pareto(3.0, (int[]){300}, 1);
        ASSERT_NOTNULL(a, "pareto(a=3,[300]) → not NULL");

        double *d = (double*)a->data;
        int all_ge_one = 1;
        for (int i = 0; i < 300; i++)
            if (d[i] < 1.0) { all_ge_one = 0; break; }
        ASSERT_TRUE(all_ge_one, "pareto(a=3) all >= 1");
        free_a(a);
    }

    /* ─── random_power ───────────────────────────────────────────────── */
    TEST_SECTION("random_power");

    {
        random_seed(42);
        Array *a = random_power(1.0, (int[]){300}, 1);
        ASSERT_NOTNULL(a, "power(a=1,[300]) → not NULL");

        double *d = (double*)a->data;
        int in_range = 1;
        for (int i = 0; i < 300; i++)
            if (d[i] < 0.0 || d[i] > 1.0) { in_range = 0; break; }
        ASSERT_TRUE(in_range, "power(a=1) all in [0,1]");
        free_a(a);
    }

    /* ─── random_rayleigh ────────────────────────────────────────────── */
    TEST_SECTION("random_rayleigh");

    {
        random_seed(42);
        Array *a = random_rayleigh(1.0, (int[]){300}, 1);
        ASSERT_NOTNULL(a, "rayleigh(scale=1,[300]) → not NULL");

        double *d = (double*)a->data;
        int all_nonneg = 1;
        for (int i = 0; i < 300; i++)
            if (d[i] < 0.0) { all_nonneg = 0; break; }
        ASSERT_TRUE(all_nonneg, "rayleigh(1) all >= 0");
        free_a(a);
    }

    /* ─── random_weibull ─────────────────────────────────────────────── */
    TEST_SECTION("random_weibull");

    {
        random_seed(42);
        Array *a = random_weibull(1.0, (int[]){300}, 1);
        ASSERT_NOTNULL(a, "weibull(a=1,[300]) → not NULL");

        double *d = (double*)a->data;
        int all_nonneg = 1;
        for (int i = 0; i < 300; i++)
            if (d[i] < 0.0) { all_nonneg = 0; break; }
        ASSERT_TRUE(all_nonneg, "weibull(a=1) all >= 0");
        free_a(a);
    }

    /* ─── random_standard_cauchy ─────────────────────────────────────── */
    TEST_SECTION("random_standard_cauchy");

    {
        random_seed(42);
        Array *a = random_standard_cauchy((int[]){300}, 1);
        ASSERT_NOTNULL(a, "standard_cauchy([300]) → not NULL");

        /* Just verify no NaN/inf issues */
        double *d = (double*)a->data;
        int all_valid = 1;
        for (int i = 0; i < 300; i++)
            if (isnan(d[i]) || isinf(d[i])) { all_valid = 0; break; }
        ASSERT_TRUE(all_valid, "standard_cauchy no NaN/inf");
        free_a(a);
    }

    /* ─── random_gumbel ──────────────────────────────────────────────── */
    TEST_SECTION("random_gumbel");

    {
        random_seed(42);
        Array *a = random_gumbel(0.0, 1.0, (int[]){300}, 1);
        ASSERT_NOTNULL(a, "gumbel(loc=0,scale=1,[300]) → not NULL");

        double *d = (double*)a->data;
        int all_valid = 1;
        for (int i = 0; i < 300; i++)
            if (isnan(d[i]) || isinf(d[i])) { all_valid = 0; break; }
        ASSERT_TRUE(all_valid, "gumbel no NaN/inf (all values valid)");
        free_a(a);
    }

    /* ─── random_triangular ──────────────────────────────────────────── */
    TEST_SECTION("random_triangular");

    {
        random_seed(42);
        Array *a = random_triangular(0.0, 0.5, 1.0, (int[]){300}, 1);
        ASSERT_NOTNULL(a, "triangular(0,0.5,1,[300]) → not NULL");

        double *d = (double*)a->data;
        int in_range = 1;
        for (int i = 0; i < 300; i++)
            if (d[i] < 0.0 || d[i] > 1.0) { in_range = 0; break; }
        ASSERT_TRUE(in_range, "triangular(0,0.5,1) all in [0,1]");
        free_a(a);
    }

    /* ─── random_vonmises ────────────────────────────────────────────── */
    TEST_SECTION("random_vonmises");

    {
        random_seed(42);
        Array *a = random_vonmises(0.0, 1.0, (int[]){300}, 1);
        ASSERT_NOTNULL(a, "vonmises(mu=0,kappa=1,[300]) → not NULL");

        double *d = (double*)a->data;
        int in_range = 1;
        for (int i = 0; i < 300; i++)
            if (d[i] < -M_PI || d[i] > M_PI) { in_range = 0; break; }
        ASSERT_TRUE(in_range, "vonmises all in [-π, π]");
        free_a(a);
    }

    /* ─── random_wald ────────────────────────────────────────────────── */
    TEST_SECTION("random_wald");

    {
        random_seed(42);
        Array *a = random_wald(1.0, 1.0, (int[]){300}, 1);
        ASSERT_NOTNULL(a, "wald(mean=1,scale=1,[300]) → not NULL");

        double *d = (double*)a->data;
        int all_pos = 1;
        for (int i = 0; i < 300; i++)
            if (d[i] <= 0.0) { all_pos = 0; break; }
        ASSERT_TRUE(all_pos, "wald all > 0");
        free_a(a);
    }

    /* ─── NULL shape / ndim=0 → scalar ───────────────────────────────── */
    TEST_SECTION("scalar (NULL shape / ndim=0)");

    {
        random_seed(42);
        Array *a = random_gamma(2.0, 1.0, NULL, 0);
        ASSERT_NOTNULL(a, "gamma(NULL,0) → scalar not NULL");
        ASSERT_SIZE(a, 1, "gamma scalar size=1");
        ASSERT_NDIM(a, 1, "gamma scalar ndim=1");
        free_a(a);
    }

    {
        random_seed(42);
        Array *a = random_beta(2.0, 2.0, NULL, 0);
        ASSERT_NOTNULL(a, "beta(NULL,0) → scalar not NULL");
        ASSERT_SIZE(a, 1, "beta scalar size=1");
        free_a(a);
    }

    /* ─── Different array shapes ─────────────────────────────────────── */
    TEST_SECTION("different shapes");

    {
        random_seed(42);
        Array *a = random_normal(0.0, 1.0, (int[]){3}, 1);
        ASSERT_NOTNULL(a, "normal [3] → not NULL");
        ASSERT_SIZE(a, 3, "normal [3] size=3");
        ASSERT_NDIM(a, 1, "normal [3] ndim=1");
        free_a(a);
    }

    {
        random_seed(42);
        Array *a = random_normal(0.0, 1.0, (int[]){2, 3}, 2);
        ASSERT_NOTNULL(a, "normal [2,3] → not NULL");
        ASSERT_SIZE(a, 6, "normal [2,3] size=6");
        ASSERT_NDIM(a, 2, "normal [2,3] ndim=2");
        free_a(a);
    }

    TEST_SUMMARY();
    return (g_fail > 0) ? 1 : 0;
}
