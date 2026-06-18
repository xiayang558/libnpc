/**
 * test_math_float_ops.c — Tests for fabs_array, copysign_array,
 * nextafter_array, spacing_array, frexp_array, modf_array
 */

#include "test_utils.h"

int main(void) {
    TEST_MAIN("Math Float Ops");

    /* ── fabs_array ───────────────────────────────────────────────── */
    TEST_SECTION("fabs_array");

    {
        Array *a = make_f64_1d((double[]){-1.5, 0, 2.3, -4}, 4);
        Array *r = fabs_array(a);
        ASSERT_NOTNULL(r, "fabs [-1.5,0,2.3,-4]");
        ASSERT_EQ_ARR((double*)r->data, ((double[]){1.5, 0, 2.3, 4}), 4, TOL_F64, "fabs values");
        free_a(r); free_a(a);
    }

    /* ── copysign_array ───────────────────────────────────────────── */
    TEST_SECTION("copysign_array");

    {
        Array *a = make_f64_1d((double[]){1, 2, 3}, 3);
        Array *b = make_f64_1d((double[]){-1, 1, -1}, 3);
        Array *r = copysign_array(a, b);
        ASSERT_NOTNULL(r, "copysign [1,2,3] <- signs [-1,1,-1]");
        ASSERT_EQ_ARR((double*)r->data, ((double[]){-1, 2, -3}), 3, TOL_F64, "copysign values");
        free_a(r); free_a(b); free_a(a);
    }

    /* ── nextafter_array ──────────────────────────────────────────── */
    TEST_SECTION("nextafter_array");

    {
        Array *a = make_f64_1d((double[]){1.0}, 1);
        Array *b = make_f64_1d((double[]){2.0}, 1);
        Array *r = nextafter_array(a, b);
        ASSERT_NOTNULL(r, "nextafter(1.0, 2.0)");
        /* nextafter(1.0, 2.0) should be slightly larger than 1.0 */
        double got = ((double*)r->data)[0];
        ASSERT_TRUE(got > 1.0, "nextafter(1.0, 2.0) > 1.0");
        ASSERT_TRUE(got < 1.1, "nextafter(1.0, 2.0) not too large");
        free_a(r); free_a(b); free_a(a);
    }

    {
        Array *a = make_f64_1d((double[]){1.0}, 1);
        Array *b = make_f64_1d((double[]){0.0}, 1);
        Array *r = nextafter_array(a, b);
        ASSERT_NOTNULL(r, "nextafter(1.0, 0.0)");
        double got = ((double*)r->data)[0];
        ASSERT_TRUE(got < 1.0, "nextafter(1.0, 0.0) < 1.0");
        free_a(r); free_a(b); free_a(a);
    }

    /* ── spacing_array ────────────────────────────────────────────── */
    TEST_SECTION("spacing_array");

    {
        Array *a = make_f64_1d((double[]){1.0}, 1);
        Array *r = spacing_array(a);
        ASSERT_NOTNULL(r, "spacing(1.0)");
        double got = ((double*)r->data)[0];
        ASSERT_TRUE(got > 0, "spacing(1.0) > 0");
        ASSERT_TRUE(got <= DBL_EPSILON * 1.1, "spacing(1.0) ~= DBL_EPSILON");
        free_a(r); free_a(a);
    }

    {
        Array *a = make_f64_1d((double[]){0.0}, 1);
        Array *r = spacing_array(a);
        ASSERT_NOTNULL(r, "spacing(0.0)");
        double got = ((double*)r->data)[0];
        /* spacing(0.0) returns 0.0 in this implementation */
        ASSERT_EQ_DBL(got, 0.0, TOL_F64, "spacing(0.0) = 0");
        free_a(r); free_a(a);
    }

    /* ── frexp_array ──────────────────────────────────────────────── */
    TEST_SECTION("frexp_array");

    {
        Array *a = make_f64_1d((double[]){4.0}, 1);
        Array *mant = NULL, *exp = NULL;
        int ret = frexp_array(a, &mant, &exp);
        ASSERT_EQ_INT(ret, 0, "frexp(4.0) ret=0");
        ASSERT_NOTNULL(mant, "frexp mantissa not null");
        ASSERT_NOTNULL(exp, "frexp exponent not null");
        /* 4.0 = 0.5 * 2^3
         * mantissa is FLOAT64 (read as double*). Exponent is INT32 (read as int*). */
        ASSERT_EQ_ARR((double*)mant->data, ((double[]){0.5}), 1, TOL_F64, "frexp mantissa = 0.5");
        ASSERT_EQ_IARR((int*)exp->data, ((int[]){3}), 1, "frexp exponent = 3");
        free_a(exp); free_a(mant); free_a(a);
    }

    {
        Array *a = make_f64_1d((double[]){0.0}, 1);
        Array *mant = NULL, *exp = NULL;
        int ret = frexp_array(a, &mant, &exp);
        ASSERT_EQ_INT(ret, 0, "frexp(0.0) ret=0");
        ASSERT_NOTNULL(mant, "frexp(0.0) mant not null");
        /* frexp(0.0) -> mant=0.0, exp=0 */
        ASSERT_EQ_ARR((double*)mant->data, ((double[]){0.0}), 1, TOL_F64, "frexp(0.0) mant = 0");
        free_a(exp); free_a(mant); free_a(a);
    }

    /* ── modf_array ───────────────────────────────────────────────── */
    TEST_SECTION("modf_array");

    {
        Array *a = make_f64_1d((double[]){1.5, 2.7, 3.0}, 3);
        Array *frac = NULL, *integ = NULL;
        int ret = modf_array(a, &frac, &integ);
        ASSERT_EQ_INT(ret, 0, "modf ret=0");
        ASSERT_NOTNULL(frac, "modf fractional not null");
        ASSERT_NOTNULL(integ, "modf integer not null");
        ASSERT_EQ_ARR((double*)frac->data, ((double[]){0.5, 0.7, 0.0}), 3, TOL_F64, "modf fractional parts");
        ASSERT_EQ_ARR((double*)integ->data, ((double[]){1, 2, 3}), 3, TOL_F64, "modf integer parts");
        free_a(integ); free_a(frac); free_a(a);
    }

    {
        /* negative numbers */
        Array *a = make_f64_1d((double[]){-1.5, -2.7}, 2);
        Array *frac = NULL, *integ = NULL;
        int ret = modf_array(a, &frac, &integ);
        ASSERT_EQ_INT(ret, 0, "modf negative ret=0");
        ASSERT_NOTNULL(frac, "modf negative frac not null");
        ASSERT_EQ_ARR((double*)frac->data, ((double[]){-0.5, -0.7}), 2, TOL_F64, "modf negative fractional");
        ASSERT_EQ_ARR((double*)integ->data, ((double[]){-1, -2}), 2, TOL_F64, "modf negative integer");
        free_a(integ); free_a(frac); free_a(a);
    }

    TEST_SUMMARY();
    return (g_fail > 0) ? 1 : 0;
}
