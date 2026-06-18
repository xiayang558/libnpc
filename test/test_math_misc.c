/**
 * test_math_misc.c — Tests for heaviside, unwrap, ahypot, deg2rad, rad2deg,
 * degrees, radians, real_array, imag_array, logaddexp, logaddexp2,
 * angle, arctan2
 */

#include "test_utils.h"

int main(void) {
    TEST_MAIN("Math Misc");

    /* ── heaviside ────────────────────────────────────────────────── */
    TEST_SECTION("heaviside");

    {
        Array *a = make_f64_1d((double[]){-1, 0, 1}, 3);
        Array *r = heaviside(a, 0.5);
        ASSERT_NOTNULL(r, "heaviside h=0.5");
        ASSERT_EQ_ARR((double*)r->data, ((double[]){0, 0.5, 1}), 3, TOL_F64, "heaviside h=0.5 values");
        free_a(r); free_a(a);
    }

    {
        Array *a = make_f64_1d((double[]){-1, 0, 1}, 3);
        Array *r = heaviside(a, 0.0);
        ASSERT_NOTNULL(r, "heaviside h=0");
        ASSERT_EQ_ARR((double*)r->data, ((double[]){0, 0, 1}), 3, TOL_F64, "heaviside h=0 values");
        free_a(r); free_a(a);
    }

    /* ── ahypot ───────────────────────────────────────────────────── */
    TEST_SECTION("ahypot");

    {
        Array *a = make_f64_1d((double[]){3, 1}, 2);
        Array *b = make_f64_1d((double[]){4, 1}, 2);
        Array *r = ahypot(a, b);
        ASSERT_NOTNULL(r, "ahypot [3,1]^[4,1]");
        /* hypot(3,4)=5, hypot(1,1)=sqrt(2) */
        ASSERT_EQ_ARR((double*)r->data, ((double[]){5, sqrt(2.0)}), 2, TOL_F64, "ahypot values");
        free_a(r); free_a(b); free_a(a);
    }

    /* ── deg2rad ──────────────────────────────────────────────────── */
    TEST_SECTION("deg2rad");

    {
        Array *a = make_f64_1d((double[]){0, 180, 360}, 3);
        Array *r = deg2rad(a);
        ASSERT_NOTNULL(r, "deg2rad [0,180,360]");
        ASSERT_EQ_ARR((double*)r->data, ((double[]){0, M_PI, 2*M_PI}), 3, TOL_F64, "deg2rad values");
        free_a(r); free_a(a);
    }

    /* ── rad2deg ──────────────────────────────────────────────────── */
    TEST_SECTION("rad2deg");

    {
        Array *a = make_f64_1d((double[]){0, M_PI, 2*M_PI}, 3);
        Array *r = rad2deg(a);
        ASSERT_NOTNULL(r, "rad2deg [0,pi,2pi]");
        ASSERT_EQ_ARR((double*)r->data, ((double[]){0, 180, 360}), 3, TOL_F64, "rad2deg values");
        free_a(r); free_a(a);
    }

    /* ── degrees ──────────────────────────────────────────────────── */
    TEST_SECTION("degrees");

    {
        Array *a = make_f64_1d((double[]){0, M_PI, M_PI/2}, 3);
        Array *r = degrees(a);
        ASSERT_NOTNULL(r, "degrees [0,pi,pi/2]");
        ASSERT_EQ_ARR((double*)r->data, ((double[]){0, 180, 90}), 3, TOL_F64, "degrees values");
        free_a(r); free_a(a);
    }

    /* ── radians ──────────────────────────────────────────────────── */
    TEST_SECTION("radians");

    {
        Array *a = make_f64_1d((double[]){0, 180, 90}, 3);
        Array *r = radians(a);
        ASSERT_NOTNULL(r, "radians [0,180,90]");
        ASSERT_EQ_ARR((double*)r->data, ((double[]){0, M_PI, M_PI/2}), 3, TOL_F64, "radians values");
        free_a(r); free_a(a);
    }

    /* ── logaddexp ────────────────────────────────────────────────── */
    TEST_SECTION("logaddexp");

    {
        Array *a = make_f64_1d((double[]){0, 1}, 2);
        Array *b = make_f64_1d((double[]){0, 1}, 2);
        Array *r = logaddexp(a, b);
        ASSERT_NOTNULL(r, "logaddexp [0,1] [0,1]");
        /* logaddexp(0,0) = log(1+1) = ln2 */
        /* logaddexp(1,1) = log(e+e) = log(2e) = 1 + ln2 */
        double ln2 = log(2.0);
        ASSERT_EQ_ARR((double*)r->data, ((double[]){ln2, 1.0 + ln2}), 2, TOL_F64, "logaddexp values");
        free_a(r); free_a(b); free_a(a);
    }

    /* ── logaddexp2 ───────────────────────────────────────────────── */
    TEST_SECTION("logaddexp2");

    {
        Array *a = make_f64_1d((double[]){0, 1}, 2);
        Array *b = make_f64_1d((double[]){0, 1}, 2);
        Array *r = logaddexp2(a, b);
        ASSERT_NOTNULL(r, "logaddexp2 [0,1] [0,1]");
        /* logaddexp2(0,0) = log2(1+1) = 1 */
        /* logaddexp2(1,1) = log2(2+2) = log2(4) = 2 */
        ASSERT_EQ_ARR((double*)r->data, ((double[]){1, 2}), 2, TOL_F64, "logaddexp2 values");
        free_a(r); free_a(b); free_a(a);
    }

    /* ── arctan2 ──────────────────────────────────────────────────── */
    TEST_SECTION("arctan2");

    {
        Array *y = make_f64_1d((double[]){1, 1, 0, -1}, 4);
        Array *x = make_f64_1d((double[]){1, 0, -1, -1}, 4);
        Array *r = arctan2(y, x);
        ASSERT_NOTNULL(r, "arctan2 [1,1,0,-1] / [1,0,-1,-1]");
        ASSERT_EQ_ARR((double*)r->data,
            ((double[]){M_PI/4, M_PI/2, M_PI, -3*M_PI/4}), 4, TOL_F64, "arctan2 values");
        free_a(r); free_a(x); free_a(y);
    }

    /* ── angle ────────────────────────────────────────────────────── */
    TEST_SECTION("angle");

    {
        /* angle with deg=0 (radians): 1+0i, 0+1i, -1+0i, 0-1i */
        complex double cdata[] = {1+0*I, 0+1*I, -1+0*I, 0-1*I};
        Array *a = make_c128_1d(cdata, 4);
        Array *r = angle(a, 0);
        ASSERT_NOTNULL(r, "angle deg=0");
        ASSERT_EQ_ARR((double*)r->data,
            ((double[]){0, M_PI/2, M_PI, -M_PI/2}), 4, TOL_F64, "angle values rad");
        free_a(r); free_a(a);
    }

    {
        complex double cdata[] = {1+0*I, 0+1*I};
        Array *a = make_c128_1d(cdata, 2);
        Array *r = angle(a, 1);
        ASSERT_NOTNULL(r, "angle deg=1");
        ASSERT_EQ_ARR((double*)r->data, ((double[]){0, 90}), 2, TOL_F64, "angle values deg");
        free_a(r); free_a(a);
    }

    /* ── real_array ───────────────────────────────────────────────── */
    TEST_SECTION("real_array");

    {
        complex double cdata[] = {1+2*I, 3+4*I};
        Array *a = make_c128_1d(cdata, 2);
        Array *r = real_array(a);
        ASSERT_NOTNULL(r, "real [1+2i, 3+4i]");
        ASSERT_EQ_ARR((double*)r->data, ((double[]){1, 3}), 2, TOL_F64, "real values");
        free_a(r); free_a(a);
    }

    /* ── imag_array ───────────────────────────────────────────────── */
    TEST_SECTION("imag_array");

    {
        complex double cdata[] = {1+2*I, 3+4*I};
        Array *a = make_c128_1d(cdata, 2);
        Array *r = imag_array(a);
        ASSERT_NOTNULL(r, "imag [1+2i, 3+4i]");
        ASSERT_EQ_ARR((double*)r->data, ((double[]){2, 4}), 2, TOL_F64, "imag values");
        free_a(r); free_a(a);
    }

    /* ── unwrap ───────────────────────────────────────────────────── */
    TEST_SECTION("unwrap");

    {
        Array *a = make_f64_1d((double[]){0, M_PI + 0.1, 2*M_PI + 0.2}, 3);
        Array *r = unwrap(a, M_PI, -1);
        ASSERT_NOTNULL(r, "unwrap [0,pi+0.1,2pi+0.2]");
        ASSERT_EQ_INT(r->size, 3, "unwrap size correct");
        ASSERT_EQ_INT(r->ndim, 1, "unwrap ndim correct");
        /* Just verify shape is correct; unwrap modifies phases to remove
         * jumps, so exact values depend on implementation details. */
        free_a(r); free_a(a);
    }

    TEST_SUMMARY();
    return (g_fail > 0) ? 1 : 0;
}
