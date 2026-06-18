/**
 * test_random_core.c — Tests for core RNG engine, uniform generators,
 * normal distribution, exponential, and uniform distribution functions.
 *
 * Covers: random_seed, random_uint64, random_double, random_sample,
 * random_rand, random_randint, random_standard_normal, random_randn,
 * random_normal, random_uniform, random_exponential, random_standard_exponential
 */

#include "test_utils.h"
#include "random.h"
#include <float.h>

/* ─── Utility: compute empirical mean of a double array ─────────────── */
static double empirical_mean(const double *data, int n) {
    double s = 0;
    for (int i = 0; i < n; i++) s += data[i];
    return s / n;
}

/* ─── Utility: compute empirical standard deviation ──────────────────── */
static double empirical_std(const double *data, int n) {
    double m = empirical_mean(data, n);
    double s = 0;
    for (int i = 0; i < n; i++) {
        double d = data[i] - m;
        s += d * d;
    }
    return sqrt(s / n);
}

int main(void) {
    TEST_MAIN("Random Core");

    /* ─── random_seed reproducibility ───────────────────────────────── */
    TEST_SECTION("random_seed reproducibility");

    {
        random_seed(42);
        double val_a = random_double();
        random_seed(42);
        double val_b = random_double();
        ASSERT_EQ_DBL(val_a, val_b, TOL_F64, "seed(42) twice → same random_double");

        /* seed(0) works */
        random_seed(0);
        double val_c = random_double();
        ASSERT_TRUE(val_c >= 0.0 && val_c < 1.0, "seed(0) produces valid double");
    }

    /* ─── random_uint64 ─────────────────────────────────────────────── */
    TEST_SECTION("random_uint64");

    {
        random_seed(42);
        uint64_t a = random_uint64();
        random_seed(42);
        uint64_t b = random_uint64();
        ASSERT_TRUE(a == b, "random_uint64 same seed → same value");

        random_seed(12345);
        uint64_t c1 = random_uint64();
        uint64_t c2 = random_uint64();
        ASSERT_TRUE(c1 != c2, "random_uint64 consecutive calls differ");
    }

    /* ─── random_double ──────────────────────────────────────────────── */
    TEST_SECTION("random_double");

    {
        random_seed(42);
        double x = random_double();
        ASSERT_TRUE(x >= 0.0 && x < 1.0, "random_double in [0,1)");

        /* multiple draws all in range */
        int ok = 1;
        for (int i = 0; i < 100; i++) {
            double y = random_double();
            if (y < 0.0 || y >= 1.0) { ok = 0; break; }
        }
        ASSERT_TRUE(ok, "100 random_double calls all in [0,1)");
    }

    /* ─── random_sample ─────────────────────────────────────────────── */
    TEST_SECTION("random_sample");

    {
        /* 1D [5] */
        random_seed(42);
        Array *a = random_sample((int[]){5}, 1);
        ASSERT_NOTNULL(a, "random_sample([5]) → not NULL");
        ASSERT_SIZE(a, 5, "random_sample([5]) size=5");
        ASSERT_NDIM(a, 1, "random_sample([5]) ndim=1");

        double *d = (double*)a->data;
        int in_range = 1;
        for (int i = 0; i < 5; i++)
            if (d[i] < 0.0 || d[i] >= 1.0) { in_range = 0; break; }
        ASSERT_TRUE(in_range, "random_sample([5]) all values in [0,1)");
        free_a(a);
    }

    {
        /* 2D [3,3] */
        random_seed(42);
        Array *a = random_sample((int[]){3, 3}, 2);
        ASSERT_NOTNULL(a, "random_sample([3,3]) → not NULL");
        ASSERT_SIZE(a, 9, "random_sample([3,3]) size=9");
        ASSERT_NDIM(a, 2, "random_sample([3,3]) ndim=2");

        double *d = (double*)a->data;
        int in_range = 1;
        for (int i = 0; i < 9; i++)
            if (d[i] < 0.0 || d[i] >= 1.0) { in_range = 0; break; }
        ASSERT_TRUE(in_range, "random_sample([3,3]) all values in [0,1)");
        free_a(a);
    }

    {
        /* reproducibility */
        random_seed(42);
        Array *a1 = random_sample((int[]){5}, 1);
        random_seed(42);
        Array *a2 = random_sample((int[]){5}, 1);
        ASSERT_EQ_ARR((double*)a1->data, (double*)a2->data, 5, TOL_F64,
                      "random_sample seed(42) reproducibility");
        free_a(a2); free_a(a1);
    }

    /* ─── random_rand ────────────────────────────────────────────────── */
    TEST_SECTION("random_rand (alias for random_sample)");

    {
        random_seed(42);
        Array *a = random_rand((int[]){5}, 1);
        ASSERT_NOTNULL(a, "random_rand([5]) → not NULL");
        ASSERT_SIZE(a, 5, "random_rand([5]) size=5");

        double *d = (double*)a->data;
        int in_range = 1;
        for (int i = 0; i < 5; i++)
            if (d[i] < 0.0 || d[i] >= 1.0) { in_range = 0; break; }
        ASSERT_TRUE(in_range, "random_rand([5]) all values in [0,1)");

        /* as alias, should match random_sample with same seed */
        random_seed(42);
        Array *b = random_sample((int[]){5}, 1);
        ASSERT_EQ_ARR((double*)a->data, (double*)b->data, 5, TOL_F64,
                      "random_rand matches random_sample with same seed");
        free_a(b); free_a(a);
    }

    /* ─── random_randint ─────────────────────────────────────────────── */
    TEST_SECTION("random_randint");

    {
        random_seed(42);
        Array *a = random_randint(0, 10, (int[]){100}, 1, INT32);
        ASSERT_NOTNULL(a, "randint(0,10,[100]) → not NULL");
        ASSERT_SIZE(a, 100, "randint(0,10,[100]) size=100");
        ASSERT_DTYPE(a, INT32, "randint(0,10,[100]) dtype INT32");

        int *d = (int*)a->data;
        int in_range = 1;
        for (int i = 0; i < 100; i++)
            if (d[i] < 0 || d[i] >= 10) { in_range = 0; break; }
        ASSERT_TRUE(in_range, "randint(0,10) all in [0,10)");
        free_a(a);
    }

    {
        /* negative range */
        random_seed(42);
        Array *a = random_randint(-5, 5, (int[]){100}, 1, INT32);
        ASSERT_NOTNULL(a, "randint(-5,5,[100]) → not NULL");

        int *d = (int*)a->data;
        int in_range = 1;
        for (int i = 0; i < 100; i++)
            if (d[i] < -5 || d[i] >= 5) { in_range = 0; break; }
        ASSERT_TRUE(in_range, "randint(-5,5) all in [-5,5)");
        free_a(a);
    }

    {
        /* reproducibility */
        random_seed(42);
        Array *a1 = random_randint(0, 10, (int[]){20}, 1, INT32);
        random_seed(42);
        Array *a2 = random_randint(0, 10, (int[]){20}, 1, INT32);
        ASSERT_EQ_IARR((int*)a1->data, (int*)a2->data, 20,
                       "randint seed(42) reproducibility");
        free_a(a2); free_a(a1);
    }

    /* ─── random_standard_normal ─────────────────────────────────────── */
    TEST_SECTION("random_standard_normal");

    {
        random_seed(42);
        Array *a = random_standard_normal((int[]){1000}, 1);
        ASSERT_NOTNULL(a, "standard_normal([1000]) → not NULL");
        ASSERT_SIZE(a, 1000, "standard_normal([1000]) size=1000");

        double *d = (double*)a->data;
        double mean_val = empirical_mean(d, 1000);
        double std_val = empirical_std(d, 1000);

        ASSERT_TRUE(fabs(mean_val) < 0.1,
                    "standard_normal mean ~ 0 (tolerance 0.1)");
        ASSERT_TRUE(fabs(std_val - 1.0) < 0.2,
                    "standard_normal std ~ 1 (tolerance 0.2)");
        free_a(a);
    }

    /* ─── random_randn ────────────────────────────────────────────────── */
    TEST_SECTION("random_randn (alias for standard_normal)");

    {
        random_seed(42);
        Array *a = random_randn((int[]){1000}, 1);
        ASSERT_NOTNULL(a, "randn([1000]) → not NULL");
        ASSERT_SIZE(a, 1000, "randn([1000]) size=1000");

        double *d = (double*)a->data;
        double mean_val = empirical_mean(d, 1000);
        double std_val = empirical_std(d, 1000);

        ASSERT_TRUE(fabs(mean_val) < 0.1, "randn mean ~ 0 (tolerance 0.1)");
        ASSERT_TRUE(fabs(std_val - 1.0) < 0.2, "randn std ~ 1 (tolerance 0.2)");

        /* alias check: same seed → same values as standard_normal */
        random_seed(42);
        Array *b = random_standard_normal((int[]){1000}, 1);
        ASSERT_EQ_ARR((double*)a->data, (double*)b->data, 1000, TOL_F64,
                      "randn matches standard_normal with same seed");
        free_a(b); free_a(a);
    }

    /* ─── random_normal ──────────────────────────────────────────────── */
    TEST_SECTION("random_normal");

    {
        random_seed(42);
        Array *a = random_normal(5.0, 2.0, (int[]){1000}, 1);
        ASSERT_NOTNULL(a, "normal(loc=5,scale=2,[1000]) → not NULL");
        ASSERT_SIZE(a, 1000, "normal(loc=5,scale=2,[1000]) size=1000");

        double *d = (double*)a->data;
        double mean_val = empirical_mean(d, 1000);
        double std_val = empirical_std(d, 1000);

        ASSERT_TRUE(fabs(mean_val - 5.0) < 0.2,
                    "normal(loc=5,scale=2) mean ~ 5 (tolerance 0.2)");
        ASSERT_TRUE(fabs(std_val - 2.0) < 0.4,
                    "normal(loc=5,scale=2) std ~ 2 (tolerance 0.4)");
        free_a(a);
    }

    /* ─── random_uniform ─────────────────────────────────────────────── */
    TEST_SECTION("random_uniform");

    {
        random_seed(42);
        Array *a = random_uniform(-1.0, 1.0, (int[]){1000}, 1);
        ASSERT_NOTNULL(a, "uniform(-1,1,[1000]) → not NULL");
        ASSERT_SIZE(a, 1000, "uniform(-1,1,[1000]) size=1000");

        double *d = (double*)a->data;
        int in_range = 1;
        for (int i = 0; i < 1000; i++)
            if (d[i] < -1.0 || d[i] >= 1.0) { in_range = 0; break; }
        ASSERT_TRUE(in_range, "uniform(-1,1) all in [-1,1)");

        double mean_val = empirical_mean(d, 1000);
        ASSERT_TRUE(fabs(mean_val) < 0.1,
                    "uniform(-1,1) mean ~ 0 (tolerance 0.1)");
        free_a(a);
    }

    /* ─── random_exponential ─────────────────────────────────────────── */
    TEST_SECTION("random_exponential");

    {
        random_seed(42);
        Array *a = random_exponential(1.0, (int[]){1000}, 1);
        ASSERT_NOTNULL(a, "exponential(scale=1,[1000]) → not NULL");
        ASSERT_SIZE(a, 1000, "exponential(scale=1,[1000]) size=1000");

        double *d = (double*)a->data;
        int all_nonneg = 1;
        for (int i = 0; i < 1000; i++)
            if (d[i] < 0.0) { all_nonneg = 0; break; }
        ASSERT_TRUE(all_nonneg, "exponential(1) all >= 0");

        double mean_val = empirical_mean(d, 1000);
        ASSERT_TRUE(fabs(mean_val - 1.0) < 0.1,
                    "exponential(1) mean ~ 1 (tolerance 0.1)");
        free_a(a);
    }

    /* ─── random_standard_exponential ────────────────────────────────── */
    TEST_SECTION("random_standard_exponential");

    {
        random_seed(42);
        Array *a = random_standard_exponential((int[]){1000}, 1);
        ASSERT_NOTNULL(a, "standard_exponential([1000]) → not NULL");
        ASSERT_SIZE(a, 1000, "standard_exponential([1000]) size=1000");

        double *d = (double*)a->data;
        int all_nonneg = 1;
        for (int i = 0; i < 1000; i++)
            if (d[i] < 0.0) { all_nonneg = 0; break; }
        ASSERT_TRUE(all_nonneg, "standard_exponential all >= 0");

        double mean_val = empirical_mean(d, 1000);
        ASSERT_TRUE(fabs(mean_val - 1.0) < 0.1,
                    "standard_exponential mean ~ 1 (tolerance 0.1)");

        /* should match exponential(scale=1) with same seed */
        random_seed(42);
        Array *b = random_exponential(1.0, (int[]){1000}, 1);
        ASSERT_EQ_ARR((double*)a->data, (double*)b->data, 1000, TOL_F64,
                      "standard_exponential matches exponential(1)");
        free_a(b); free_a(a);
    }

    /* ─── Re-seeding after use ──────────────────────────────────────── */
    TEST_SECTION("re-seeding");

    {
        /* seed(42) → seq A, seed(999) → seq B, seed(42) → seq A again */
        random_seed(42);
        Array *a1 = random_rand((int[]){10}, 1);

        random_seed(999);
        Array *b  = random_rand((int[]){10}, 1);

        random_seed(42);
        Array *a2 = random_rand((int[]){10}, 1);

        ASSERT_EQ_ARR((double*)a1->data, (double*)a2->data, 10, TOL_F64,
                      "seed(42)→values A, seed(999)→B, seed(42)→A again");

        /* A and B should differ */
        int differ = 0;
        for (int i = 0; i < 10; i++)
            if (fabs(((double*)a1->data)[i] - ((double*)b->data)[i]) > TOL_F64)
                { differ = 1; break; }
        ASSERT_TRUE(differ, "seed(42) and seed(999) produce different values");

        free_a(a2); free_a(b); free_a(a1);
    }

    TEST_SUMMARY();
    return (g_fail > 0) ? 1 : 0;
}
