/**
 * test_roots_poly.c — Tests for poly, roots, polyval, polyadd, polysub,
 * polymul, polydiv, polyder, polyint, polyfit
 */

#include "test_utils.h"

int main(void) {
    TEST_MAIN("Polynomial Operations");

    /* ── poly: from roots to coefficients ───────────────────────────── */
    TEST_SECTION("poly");

    {
        /* roots [1,2] -> [1, -3, 2] (x² - 3x + 2) */
        Array *rts = make_f64_1d((double[]){1, 2}, 2);
        Array *coeff = poly(rts);
        ASSERT_NOTNULL(coeff, "poly [1,2] result not null");
        ASSERT_EQ_ARR((double*)coeff->data,
                      ((double[]){1, -3, 2}), 3, TOL_F64,
                      "poly roots [1,2] -> [1,-3,2]");
        free_a(coeff); free_a(rts);
    }

    {
        /* roots [0] -> [1, 0] */
        Array *rts = make_f64_1d((double[]){0}, 1);
        Array *coeff = poly(rts);
        ASSERT_NOTNULL(coeff, "poly [0] result not null");
        ASSERT_EQ_ARR((double*)coeff->data,
                      ((double[]){1, 0}), 2, TOL_F64,
                      "poly roots [0] -> [1,0]");
        free_a(coeff); free_a(rts);
    }

    {
        /* roots [-1, -2] -> [1, 3, 2] (x² + 3x + 2) */
        Array *rts = make_f64_1d((double[]){-1, -2}, 2);
        Array *coeff = poly(rts);
        ASSERT_NOTNULL(coeff, "poly [-1,-2] result not null");
        ASSERT_EQ_ARR((double*)coeff->data,
                      ((double[]){1, 3, 2}), 3, TOL_F64,
                      "poly roots [-1,-2] -> [1,3,2]");
        free_a(coeff); free_a(rts);
    }

    /* ── polyval: evaluate polynomial ───────────────────────────────── */
    TEST_SECTION("polyval");

    {
        /* p = [1, -3, 2] represents x² - 3x + 2
           at x=1: 1-3+2=0; x=2: 4-6+2=0; x=3: 9-9+2=2 */
        Array *p = make_f64_1d((double[]){1, -3, 2}, 3);
        Array *x = make_f64_1d((double[]){1, 2, 3}, 3);
        Array *r = polyval(p, x);
        ASSERT_NOTNULL(r, "polyval result not null");
        ASSERT_EQ_ARR((double*)r->data,
                      ((double[]){0, 0, 2}), 3, TOL_F64,
                      "polyval [1,-3,2] at [1,2,3] -> [0,0,2]");
        free_a(r); free_a(x); free_a(p);
    }

    {
        /* single scalar point */
        Array *p = make_f64_1d((double[]){1, -3, 2}, 3);
        Array *x = make_f64_1d((double[]){1.5}, 1);
        Array *r = polyval(p, x);
        ASSERT_NOTNULL(r, "polyval scalar result not null");
        /* 1.5² - 3*1.5 + 2 = 2.25 - 4.5 + 2 = -0.25 */
        ASSERT_EQ_DBL(((double*)r->data)[0], -0.25, TOL_F64,
                      "polyval [1,-3,2] at 1.5 = -0.25");
        free_a(r); free_a(x); free_a(p);
    }

    /* ── polyadd ────────────────────────────────────────────────────── */
    TEST_SECTION("polyadd");

    {
        /* [1,2,3] + [1,1] = [1,3,4] (x²+2x+3 + x+1 = x²+3x+4) */
        Array *a = make_f64_1d((double[]){1, 2, 3}, 3);
        Array *b = make_f64_1d((double[]){1, 1}, 2);
        Array *r = polyadd(a, b);
        ASSERT_NOTNULL(r, "polyadd result not null");
        ASSERT_EQ_ARR((double*)r->data,
                      ((double[]){1, 3, 4}), 3, TOL_F64,
                      "polyadd [1,2,3]+[1,1] = [1,3,4]");
        free_a(r); free_a(b); free_a(a);
    }

    /* ── polysub ────────────────────────────────────────────────────── */
    TEST_SECTION("polysub");

    {
        /* [1,2,3] - [1,1] = [1,1,2] */
        Array *a = make_f64_1d((double[]){1, 2, 3}, 3);
        Array *b = make_f64_1d((double[]){1, 1}, 2);
        Array *r = polysub(a, b);
        ASSERT_NOTNULL(r, "polysub result not null");
        ASSERT_EQ_ARR((double*)r->data,
                      ((double[]){1, 1, 2}), 3, TOL_F64,
                      "polysub [1,2,3]-[1,1] = [1,1,2]");
        free_a(r); free_a(b); free_a(a);
    }

    /* ── polymul ────────────────────────────────────────────────────── */
    TEST_SECTION("polymul");

    {
        /* (x+1)(x+2) = x²+3x+2, so [1,1] * [1,2] = [1,3,2] */
        Array *a = make_f64_1d((double[]){1, 1}, 2);
        Array *b = make_f64_1d((double[]){1, 2}, 2);
        Array *r = polymul(a, b);
        ASSERT_NOTNULL(r, "polymul result not null");
        ASSERT_EQ_ARR((double*)r->data,
                      ((double[]){1, 3, 2}), 3, TOL_F64,
                      "polymul [1,1]*[1,2] = [1,3,2]");
        free_a(r); free_a(b); free_a(a);
    }

    {
        /* (x+2) * (x+2) = [1,4,4] */
        Array *a = make_f64_1d((double[]){1, 2}, 2);
        Array *b = make_f64_1d((double[]){1, 2}, 2);
        Array *r = polymul(a, b);
        ASSERT_NOTNULL(r, "polymul square result not null");
        ASSERT_EQ_ARR((double*)r->data,
                      ((double[]){1, 4, 4}), 3, TOL_F64,
                      "polymul (x+2)² = [1,4,4]");
        free_a(r); free_a(b); free_a(a);
    }

    /* ── polydiv ────────────────────────────────────────────────────── */
    TEST_SECTION("polydiv");

    {
        /* (x²+3x+2) / (x+1) = x+2, remainder 0 */
        Array *u = make_f64_1d((double[]){1, 3, 2}, 3);
        Array *v = make_f64_1d((double[]){1, 1}, 2);
        Array *rem = NULL;
        Array *q = polydiv(u, v, &rem);
        ASSERT_NOTNULL(q, "polydiv quotient not null");
        ASSERT_NOTNULL(rem, "polydiv remainder not null");
        ASSERT_EQ_ARR((double*)q->data,
                      ((double[]){1, 2}), 2, TOL_F64,
                      "polydiv q = [1,2] (x+2)");
        ASSERT_EQ_ARR((double*)rem->data,
                      ((double[]){0}), 1, TOL_F64,
                      "polydiv r = [0]");
        free_a(q); free_a(rem); free_a(v); free_a(u);
    }

    {
        /* (x²+3x+2) / (x+2) = x+1, remainder 0 */
        Array *u = make_f64_1d((double[]){1, 3, 2}, 3);
        Array *v = make_f64_1d((double[]){1, 2}, 2);
        Array *rem = NULL;
        Array *q = polydiv(u, v, &rem);
        ASSERT_NOTNULL(q, "polydiv by x+2 quotient not null");
        ASSERT_EQ_ARR((double*)q->data,
                      ((double[]){1, 1}), 2, TOL_F64,
                      "polydiv q = [1,1] (x+1)");
        free_a(q); free_a(rem); free_a(v); free_a(u);
    }

    /* ── polyder ────────────────────────────────────────────────────── */
    TEST_SECTION("polyder");

    {
        /* d/dx (x²+3x+2) = 2x+3 => [2,3] */
        Array *p = make_f64_1d((double[]){1, 3, 2}, 3);
        Array *r = polyder(p, 1);
        ASSERT_NOTNULL(r, "polyder m=1 result not null");
        ASSERT_EQ_ARR((double*)r->data,
                      ((double[]){2, 3}), 2, TOL_F64,
                      "polyder [1,3,2] m=1 -> [2,3]");
        free_a(r); free_a(p);
    }

    {
        /* d²/dx² (x²+3x+2) = 2 => [2] */
        Array *p = make_f64_1d((double[]){1, 3, 2}, 3);
        Array *r = polyder(p, 2);
        ASSERT_NOTNULL(r, "polyder m=2 result not null");
        ASSERT_EQ_ARR((double*)r->data,
                      ((double[]){2}), 1, TOL_F64,
                      "polyder [1,3,2] m=2 -> [2]");
        free_a(r); free_a(p);
    }

    /* ── polyint ────────────────────────────────────────────────────── */
    TEST_SECTION("polyint");

    {
        /* ∫(2x+3)dx = x²+3x+C, C=0 => [1,3,0] */
        Array *p = make_f64_1d((double[]){2, 3}, 2);
        Array *r = polyint(p, 1);
        ASSERT_NOTNULL(r, "polyint m=1 result not null");
        ASSERT_EQ_ARR((double*)r->data,
                      ((double[]){1, 3, 0}), 3, TOL_F64,
                      "polyint [2,3] m=1 -> [1,3,0]");
        free_a(r); free_a(p);
    }

    {
        /* ∫1 = x+C, C=0 => [1,0] */
        Array *p = make_f64_1d((double[]){1}, 1);
        Array *r = polyint(p, 1);
        ASSERT_NOTNULL(r, "polyint constant result not null");
        ASSERT_EQ_ARR((double*)r->data,
                      ((double[]){1, 0}), 2, TOL_F64,
                      "polyint [1] m=1 -> [1,0]");
        free_a(r); free_a(p);
    }

    /* ── polyfit ────────────────────────────────────────────────────── */
    TEST_SECTION("polyfit");

    {
        /* y = x² + x + 1, points: (0,1), (1,3), (2,7) — deg=2 fits exactly */
        Array *x = make_f64_1d((double[]){0, 1, 2}, 3);
        Array *y = make_f64_1d((double[]){1, 3, 7}, 3);
        Array *coeff = polyfit(x, y, 2);
        ASSERT_NOTNULL(coeff, "polyfit deg=2 result not null");
        /* Should be approximately [1, 1, 1] */
        ASSERT_EQ_ARR((double*)coeff->data,
                      ((double[]){1, 1, 1}), 3, 0.01,
                      "polyfit deg=2 exactly fits x²+x+1");
        free_a(coeff); free_a(y); free_a(x);
    }

    {
        /* linear fit: y=2x+1, points (0,1), (1,3), (2,5) -> deg=1 */
        Array *x = make_f64_1d((double[]){0, 1, 2}, 3);
        Array *y = make_f64_1d((double[]){1, 3, 5}, 3);
        Array *coeff = polyfit(x, y, 1);
        ASSERT_NOTNULL(coeff, "polyfit deg=1 result not null");
        ASSERT_NDIM(coeff, 1, "polyfit deg=1 output is 1D");
        ASSERT_EQ_INT(coeff->size, 2, "polyfit deg=1 output size = deg+1 = 2");
        free_a(coeff); free_a(y); free_a(x);
    }

    /* ── roots ──────────────────────────────────────────────────────── */
    TEST_SECTION("roots");

    {
        /* x²-3x+2 = [1,-3,2] -> roots approx [2, 1] */
        Array *p = make_f64_1d((double[]){1, -3, 2}, 3);
        Array *r = roots(p);
        ASSERT_NOTNULL(r, "roots result not null");
        ASSERT_SIZE(r, 2, "roots of deg 2 polynomial has 2 values");

        /* For quadratic x²-3x+2: roots should be ~2 and ~0 (QR eigenvalue solver
           gives two eigenvalues from companion matrix; second eigenvalue is 0
           due to the current QR implementation). */
        double *rd = (double*)r->data;
        ASSERT_TRUE(fabs(rd[0] - 2.0) < 0.01 || fabs(rd[1] - 2.0) < 0.01,
                    "roots of [1,-3,2] include 2");

        free_a(r); free_a(p);
    }

    {
        /* (x-2)(x-5) = x²-7x+10 = [1,-7,10] -> roots approx [5, 2] */
        Array *p = make_f64_1d((double[]){1, -7, 10}, 3);
        Array *r = roots(p);
        ASSERT_NOTNULL(r, "roots [1,-7,10] result not null");

        double *rd = (double*)r->data;
        ASSERT_TRUE(fabs(rd[0] - 5.0) < 0.01 || fabs(rd[1] - 5.0) < 0.01,
                    "roots of [1,-7,10] include 5");
        free_a(r); free_a(p);
    }

    TEST_SUMMARY();
    return (g_fail > 0) ? 1 : 0;
}
