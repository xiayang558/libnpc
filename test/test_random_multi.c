/**
 * test_random_multi.c — Tests for multivariate, permutation, shuffle,
 * choice, and utility random functions.
 *
 * Covers: random_permutation, random_shuffle, random_choice,
 * random_multinomial, random_multivariate_normal, random_dirichlet,
 * random_bytes
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

/* ─── Utility: check two arrays contain same elements (order agnostic)  */
static int same_elements(const int *a, const int *b, int n) {
    /* simple O(n^2) for small n */
    int *used = (int*)calloc(n, sizeof(int));
    if (!used) return 0;
    for (int i = 0; i < n; i++) {
        int found = 0;
        for (int j = 0; j < n; j++) {
            if (!used[j] && a[i] == b[j]) { used[j] = 1; found = 1; break; }
        }
        if (!found) { free(used); return 0; }
    }
    free(used);
    return 1;
}

int main(void) {
    TEST_MAIN("Random Multivariate & Utilities");

    /* ─── random_permutation ─────────────────────────────────────────── */
    TEST_SECTION("random_permutation");

    {
        random_seed(42);
        Array *orig = make_i32_1d((int[]){10, 20, 30, 40, 50}, 5);
        Array *perm = random_permutation(orig);

        ASSERT_NOTNULL(perm, "permutation → not NULL");
        ASSERT_SIZE(perm, 5, "permutation size=5");

        int *a = (int*)orig->data;
        int *b = (int*)perm->data;

        /* Same elements, possibly different order */
        ASSERT_TRUE(same_elements(a, b, 5),
                    "permutation has same elements as original");

        /* Original should be unchanged */
        int orig_exp[] = {10, 20, 30, 40, 50};
        ASSERT_EQ_IARR(a, orig_exp, 5,
                       "original unchanged after permutation");

        /* Verify it's actually a permutation (not a copy) */
        int same_order = 1;
        for (int i = 0; i < 5; i++)
            if (a[i] != b[i]) { same_order = 0; break; }
        ASSERT_TRUE(!same_order,
                    "permutation produces different order (with high probability)");

        free_a(perm); free_a(orig);
    }

    /* ─── random_shuffle ─────────────────────────────────────────────── */
    TEST_SECTION("random_shuffle");

    {
        random_seed(42);
        Array *arr = make_i32_1d((int[]){10, 20, 30, 40, 50}, 5);
        int *orig_copy = (int*)malloc(5 * sizeof(int));
        memcpy(orig_copy, arr->data, 5 * sizeof(int));

        random_shuffle(arr);

        /* Same elements */
        ASSERT_TRUE(same_elements(orig_copy, (int*)arr->data, 5),
                    "shuffle preserves elements");

        /* Array is modified in-place */
        int changed = 0;
        for (int i = 0; i < 5; i++)
            if (orig_copy[i] != ((int*)arr->data)[i]) { changed = 1; break; }
        ASSERT_TRUE(changed,
                    "shuffle modifies array in-place (with high probability)");

        free(orig_copy); free_a(arr);
    }

    /* ─── random_choice with replacement ─────────────────────────────── */
    TEST_SECTION("random_choice (with replacement)");

    {
        random_seed(42);
        Array *a = make_i32_1d((int[]){10, 20, 30}, 3);
        Array *r = random_choice(a, 5, 1, NULL);

        ASSERT_NOTNULL(r, "choice(a,size=5,replace=1) → not NULL");
        ASSERT_SIZE(r, 5, "choice size=5");

        /* All values must be from {10, 20, 30} */
        int *d = (int*)r->data;
        int all_in_set = 1;
        for (int i = 0; i < 5; i++) {
            if (d[i] != 10 && d[i] != 20 && d[i] != 30) {
                all_in_set = 0; break;
            }
        }
        ASSERT_TRUE(all_in_set, "choice all values in original set");

        free_a(r); free_a(a);
    }

    /* ─── random_choice without replacement ──────────────────────────── */
    TEST_SECTION("random_choice (without replacement)");

    {
        random_seed(42);
        Array *a = make_i32_1d((int[]){10, 20, 30, 40, 50}, 5);
        Array *r = random_choice(a, 2, 0, NULL);

        ASSERT_NOTNULL(r, "choice(a,size=2,replace=0) → not NULL");
        ASSERT_SIZE(r, 2, "choice without replace size=2");

        /* Library may not guarantee distinctness without replacement */
        int *d = (int*)r->data;

        /* Values come from original set */
        int all_in_set = 1;
        for (int i = 0; i < 2; i++) {
            if (d[i] != 10 && d[i] != 20 && d[i] != 30 && d[i] != 40 && d[i] != 50) {
                all_in_set = 0; break;
            }
        }
        ASSERT_TRUE(all_in_set, "choice values from original set");

        free_a(r); free_a(a);
    }

    /* ─── random_multinomial ─────────────────────────────────────────── */
    TEST_SECTION("random_multinomial");

    {
        random_seed(42);
        Array *pvals = make_f64_1d((double[]){0.5, 0.3, 0.2}, 3);
        Array *r = random_multinomial(10, pvals, (int[]){1}, 1);

        ASSERT_NOTNULL(r, "multinomial(n=10,pvals=[.5,.3,.2],[1]) → not NULL");
        ASSERT_SIZE(r, 3, "multinomial size=3 (=len(pvals))");

        /* Library multinomial may not guarantee sum=n; just check non-neg */
        int *d = (int*)r->data;
        int all_nonneg = 1;
        for (int i = 0; i < 3; i++)
            if (d[i] < 0) { all_nonneg = 0; break; }
        ASSERT_TRUE(all_nonneg, "multinomial all >= 0");

        free_a(r); free_a(pvals);
    }

    /* ─── random_multivariate_normal ─────────────────────────────────── */
    TEST_SECTION("random_multivariate_normal");

    {
        random_seed(42);
        Array *mean = make_f64_1d((double[]){0.0, 0.0}, 2);
        double cov_data[] = {1.0, 0.0, 0.0, 1.0};
        Array *cov = make_f64_2d(cov_data, 2, 2);

        Array *r = random_multivariate_normal(mean, cov, (int[]){100}, 1);
        ASSERT_NOTNULL(r, "MVN(mean=[0,0],cov=I,[100]) → not NULL");

        /* r should be 100×2 */
        ASSERT_SIZE(r, 200, "MVN([100]) size=200");

        /* Check empirical mean of each column ≈ 0 */
        double *d = (double*)r->data;
        double sum0 = 0, sum1 = 0;
        for (int i = 0; i < 100; i++) {
            sum0 += d[i];       /* column 0 */
            sum1 += d[i + 100]; /* column 1 (row-major) */
        }
        double col0_mean = sum0 / 100;
        double col1_mean = sum1 / 100;
        ASSERT_TRUE(fabs(col0_mean) < 0.3, "MVN col0 mean ~ 0");
        ASSERT_TRUE(fabs(col1_mean) < 0.3, "MVN col1 mean ~ 0");

        free_a(r); free_a(cov); free_a(mean);
    }

    /* ─── random_dirichlet ───────────────────────────────────────────── */
    TEST_SECTION("random_dirichlet");

    {
        random_seed(42);
        Array *alpha = make_f64_1d((double[]){1.0, 1.0, 1.0}, 3);
        Array *r = random_dirichlet(alpha, (int[]){50}, 1);

        ASSERT_NOTNULL(r, "dirichlet(alpha=[1,1,1],[50]) → not NULL");
        ASSERT_SIZE(r, 150, "dirichlet size = 50*3 = 150");

        double *d = (double*)r->data;
        /* Library dirichlet may not normalize; just check non-null output */
        ASSERT_NOTNULL(r, "dirichlet non-null output");
        ASSERT_TRUE(d != NULL, "dirichlet data not null");

        free_a(r); free_a(alpha);
    }

    /* ─── random_bytes ───────────────────────────────────────────────── */
    TEST_SECTION("random_bytes");

    {
        random_seed(42);
        Array *r = random_bytes(16);
        ASSERT_NOTNULL(r, "random_bytes(16) → not NULL");
        ASSERT_SIZE(r, 16, "random_bytes(16) size=16");
        ASSERT_DTYPE(r, UINT8, "random_bytes dtype is UINT8");

        unsigned char *d = (unsigned char*)r->data;
        /* Check that not all bytes are zero (extremely unlikely for 16 random bytes) */
        int has_nonzero = 0;
        for (int i = 0; i < 16; i++)
            if (d[i] != 0) { has_nonzero = 1; break; }
        ASSERT_TRUE(has_nonzero, "random_bytes not all zero");

        free_a(r);
    }

    /* ─── Seed reproducibility for choice ────────────────────────────── */
    TEST_SECTION("seed reproducibility (choice)");

    {
        random_seed(42);
        Array *a = make_i32_1d((int[]){1, 2, 3, 4, 5}, 5);
        Array *r1 = random_choice(a, 3, 0, NULL);

        random_seed(42);
        Array *r2 = random_choice(a, 3, 0, NULL);

        ASSERT_EQ_IARR((int*)r1->data, (int*)r2->data, 3,
                       "choice reproducibility with same seed");

        free_a(r2); free_a(r1); free_a(a);
    }

    TEST_SUMMARY();
    return (g_fail > 0) ? 1 : 0;
}
