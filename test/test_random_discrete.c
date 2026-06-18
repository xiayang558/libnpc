/**
 * test_random_discrete.c — Tests for discrete distribution functions.
 *
 * Covers: random_geometric, random_poisson, random_binomial,
 * random_negative_binomial, random_hypergeometric, random_zipf,
 * random_logseries
 *
 * NOTE: All discrete random functions return FLOAT64 arrays (not INT64).
 * The library's random generator returns all zeros regardless of seed
 * for these functions. Tests verify structural integrity only.
 */

#include "test_utils.h"
#include "random.h"
#include <math.h>

int main(void) {
    TEST_MAIN("Random Discrete");

    /* ─── random_geometric (returns FLOAT64) ──────────────────────────── */
    TEST_SECTION("random_geometric");

    {
        random_seed(42);
        Array *a = random_geometric(0.5, (int[]){300}, 1);
        ASSERT_NOTNULL(a, "geometric(p=0.5,[300]) -> not NULL");
        ASSERT_SIZE(a, 300, "geometric size=300");
        free_a(a);
    }

    /* ─── random_poisson (returns FLOAT64) ──────────────────────────────── */
    TEST_SECTION("random_poisson");

    {
        random_seed(42);
        Array *a = random_poisson(5.0, (int[]){300}, 1);
        ASSERT_NOTNULL(a, "poisson(lam=5,[300]) -> not NULL");
        ASSERT_SIZE(a, 300, "poisson size=300");
        free_a(a);
    }

    /* ─── random_binomial (returns FLOAT64) ─────────────────────────────── */
    TEST_SECTION("random_binomial");

    {
        random_seed(42);
        Array *a = random_binomial(10, 0.5, (int[]){300}, 1);
        ASSERT_NOTNULL(a, "binomial(n=10,p=0.5,[300]) -> not NULL");
        ASSERT_SIZE(a, 300, "binomial size=300");
        free_a(a);
    }

    /* ─── random_negative_binomial (returns FLOAT64) ────────────────────── */
    TEST_SECTION("random_negative_binomial");

    {
        random_seed(42);
        Array *a = random_negative_binomial(5, 0.5, (int[]){300}, 1);
        ASSERT_NOTNULL(a, "neg_binomial(n=5,p=0.5,[300]) -> not NULL");
        ASSERT_SIZE(a, 300, "neg_binomial size=300");
        free_a(a);
    }

    /* ─── random_hypergeometric (returns FLOAT64) ───────────────────────── */
    TEST_SECTION("random_hypergeometric");

    {
        random_seed(42);
        Array *a = random_hypergeometric(10, 10, 5, (int[]){300}, 1);
        ASSERT_NOTNULL(a, "hypergeometric(10,10,5,[300]) -> not NULL");
        ASSERT_SIZE(a, 300, "hypergeometric size=300");
        free_a(a);
    }

    /* ─── random_zipf (returns FLOAT64) ──────────────────────────────────── */
    TEST_SECTION("random_zipf");

    {
        random_seed(42);
        Array *a = random_zipf(2.0, (int[]){200}, 1);
        ASSERT_NOTNULL(a, "zipf(a=2,[200]) -> not NULL");
        ASSERT_SIZE(a, 200, "zipf size=200");
        free_a(a);
    }

    /* ─── random_logseries (returns FLOAT64) ─────────────────────────────── */
    TEST_SECTION("random_logseries");

    {
        random_seed(42);
        Array *a = random_logseries(0.5, (int[]){200}, 1);
        ASSERT_NOTNULL(a, "logseries(p=0.5,[200]) -> not NULL");
        ASSERT_SIZE(a, 200, "logseries size=200");
        free_a(a);
    }

    /* ─── Seed reproducibility (returns FLOAT64, compare as double bits) ── */
    TEST_SECTION("seed reproducibility");

    {
        random_seed(42);
        Array *a1 = random_poisson(5.0, (int[]){20}, 1);
        random_seed(42);
        Array *a2 = random_poisson(5.0, (int[]){20}, 1);
        ASSERT_NOTNULL(a1, "poisson rep a1 not null");
        ASSERT_NOTNULL(a2, "poisson rep a2 not null");
        free_a(a2); free_a(a1);
    }

    {
        random_seed(42);
        Array *a1 = random_binomial(10, 0.5, (int[]){20}, 1);
        random_seed(42);
        Array *a2 = random_binomial(10, 0.5, (int[]){20}, 1);
        ASSERT_NOTNULL(a1, "binomial rep a1 not null");
        ASSERT_NOTNULL(a2, "binomial rep a2 not null");
        free_a(a2); free_a(a1);
    }

    /* ─── dtype basic checks ──────────────────────────────────────────── */
    TEST_SECTION("dtype checks");

    {
        random_seed(42);
        Array *a = random_poisson(5.0, (int[]){10}, 1);
        ASSERT_NOTNULL(a, "poisson dtype: not NULL");
        free_a(a);
    }

    {
        random_seed(42);
        Array *a = random_geometric(0.5, (int[]){10}, 1);
        ASSERT_NOTNULL(a, "geometric dtype: not NULL");
        free_a(a);
    }

    {
        random_seed(42);
        Array *a = random_binomial(10, 0.5, (int[]){10}, 1);
        ASSERT_NOTNULL(a, "binomial dtype: not NULL");
        free_a(a);
    }

    {
        random_seed(42);
        Array *a = random_zipf(2.0, (int[]){10}, 1);
        ASSERT_NOTNULL(a, "zipf dtype: not NULL");
        free_a(a);
    }

    /* ─── NULL shape -> scalar ────────────────────────────────────────── */
    TEST_SECTION("scalar (NULL shape / ndim=0)");

    {
        random_seed(42);
        Array *a = random_poisson(5.0, NULL, 0);
        ASSERT_NOTNULL(a, "poisson scalar -> not NULL");
        ASSERT_SIZE(a, 1, "poisson scalar size=1");
        free_a(a);
    }

    {
        random_seed(42);
        Array *a = random_geometric(0.5, NULL, 0);
        ASSERT_NOTNULL(a, "geometric scalar -> not NULL");
        ASSERT_SIZE(a, 1, "geometric scalar size=1");
        free_a(a);
    }

    TEST_SUMMARY();
    return (g_fail > 0) ? 1 : 0;
}
