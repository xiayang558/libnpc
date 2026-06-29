#include "test_utils.h"

int main(void) {
    TEST_MAIN("Math New Functions");

    /* ─── ndim ─── */
    TEST_SECTION("ndim");
    {
        Array *a = make_f64_1d((double[]){1, 2, 3}, 3);
        ASSERT_EQ_INT(ndim(a), 1, "ndim 1D [1,2,3] = 1");
        free_a(a);
    }
    {
        Array *a = make_f64_2d((double[]){1,2,3,4,5,6}, 2, 3);
        int d = ndim(a);
        ASSERT_EQ_INT(d, 2, "ndim 2D 2x3 = 2");
        free_a(a);
    }
    {
        int d = ndim(NULL);
        ASSERT_EQ_INT(d, -1, "ndim NULL = -1");
    }

    /* ─── npc_ldexp ─── */
    TEST_SECTION("npc_ldexp");
    {
        Array *x = make_f64_1d((double[]){1, 2, 3}, 3);
        Array *exp = make_i32_1d((int[]){1, 1, 1}, 3);
        Array *r = npc_ldexp(x, exp);
        ASSERT_NOTNULL(r, "npc_ldexp not null");
        ASSERT_EQ_ARR((double*)r->data, ((double[]){2, 4, 6}), 3, TOL_F64, "npc_ldexp [1,2,3]*2^[1,1,1] = [2,4,6]");
        free_a(r); free_a(exp); free_a(x);
    }
    {
        Array *x = make_f64_1d((double[]){0.5}, 1);
        Array *exp = make_i32_1d((int[]){3}, 1);
        Array *r = npc_ldexp(x, exp);
        ASSERT_NOTNULL(r, "npc_ldexp float not null");
        ASSERT_EQ_ARR((double*)r->data, ((double[]){4}), 1, TOL_F64, "npc_ldexp [0.5]*2^[3] = [4]");
        free_a(r); free_a(exp); free_a(x);
    }

    /* ─── reciprocal ─── */
    TEST_SECTION("reciprocal");
    {
        Array *a = make_f64_1d((double[]){1, 2, 4}, 3);
        Array *r = reciprocal(a);
        ASSERT_NOTNULL(r, "reciprocal not null");
        ASSERT_EQ_ARR((double*)r->data, ((double[]){1, 0.5, 0.25}), 3, TOL_F64, "reciprocal [1,2,4] = [1,0.5,0.25]");
        free_a(r); free_a(a);
    }
    {
        Array *a = make_i32_1d((int[]){2, 4}, 2);
        Array *r = reciprocal(a);
        ASSERT_NOTNULL(r, "reciprocal INT32 not null");
        ASSERT_EQ_ARR((double*)r->data, ((double[]){0.5, 0.25}), 2, TOL_F64, "reciprocal INT32 [2,4] = [0.5,0.25]");
        ASSERT_DTYPE(r, FLOAT64, "reciprocal INT32 -> FLOAT64");
        free_a(r); free_a(a);
    }

    /* ─── positive ─── */
    TEST_SECTION("positive");
    {
        Array *a = make_f64_1d((double[]){1, -2, 3}, 3);
        Array *r = positive(a);
        ASSERT_NOTNULL(r, "positive not null");
        ASSERT_EQ_ARR((double*)r->data, ((double[]){1, -2, 3}), 3, TOL_F64, "positive [1,-2,3] = [1,-2,3] values");
        ASSERT_NDIM(r, 1, "positive ndim unchanged");
        free_a(r); free_a(a);
    }
    {
        Array *a = make_f64_1d((double[]){5.0}, 1);
        Array *r = positive(a);
        ASSERT_NOTNULL(r, "positive scalar");
        ASSERT_EQ_DBL(((double*)r->data)[0], 5.0, TOL_F64, "positive [5.0] = 5.0");
        free_a(r); free_a(a);
    }

    /* ─── negative ─── */
    TEST_SECTION("negative");
    {
        Array *a = make_f64_1d((double[]){1, -2, 3}, 3);
        Array *r = negative(a);
        ASSERT_NOTNULL(r, "negative not null");
        ASSERT_EQ_ARR((double*)r->data, ((double[]){-1, 2, -3}), 3, TOL_F64, "negative [1,-2,3] = [-1,2,-3]");
        free_a(r); free_a(a);
    }
    {
        Array *a = make_i32_1d((int[]){1, 2}, 2);
        Array *r = negative(a);
        ASSERT_NOTNULL(r, "negative INT32 not null");
        int64_t *neg_d = (int64_t*)r->data;
        ASSERT_EQ_I64(neg_d[0], -1, "negative INT32 [0] = -1");
        ASSERT_EQ_I64(neg_d[1], -2, "negative INT32 [1] = -2");
        ASSERT_DTYPE(r, INT64, "negative INT32 -> INT64");
        free_a(r); free_a(a);
    }

    /* ─── fmod_arr ─── */
    TEST_SECTION("fmod_arr");
    {
        Array *a = make_f64_1d((double[]){5, 7, 9}, 3);
        Array *b = make_f64_1d((double[]){3, 3, 3}, 3);
        Array *r = fmod_arr(a, b);
        ASSERT_NOTNULL(r, "fmod_arr not null");
        ASSERT_EQ_ARR((double*)r->data, ((double[]){2, 1, 0}), 3, TOL_F64, "fmod_arr [5,7,9]%[3,3,3] = [2,1,0]");
        free_a(r); free_a(b); free_a(a);
    }
    {
        Array *a = make_f64_1d((double[]){-5}, 1);
        Array *b = make_f64_1d((double[]){3}, 1);
        Array *r = fmod_arr(a, b);
        ASSERT_NOTNULL(r, "fmod_arr negative not null");
        ASSERT_EQ_ARR((double*)r->data, ((double[]){-2}), 1, TOL_F64, "fmod_arr [-5]%[3] = [-2]");
        free_a(r); free_a(b); free_a(a);
    }

    /* ─── npc_cbrt ─── */
    TEST_SECTION("npc_cbrt");
    {
        Array *a = make_f64_1d((double[]){0, 1, 8, 27}, 4);
        Array *r = npc_cbrt(a);
        ASSERT_NOTNULL(r, "npc_cbrt not null");
        ASSERT_EQ_ARR((double*)r->data, ((double[]){0, 1, 2, 3}), 4, TOL_F64, "npc_cbrt [0,1,8,27] = [0,1,2,3]");
        free_a(r); free_a(a);
    }
    {
        Array *a = make_f64_1d((double[]){-8}, 1);
        Array *r = npc_cbrt(a);
        ASSERT_NOTNULL(r, "npc_cbrt negative not null");
        ASSERT_EQ_ARR((double*)r->data, ((double[]){-2}), 1, TOL_F64, "npc_cbrt [-8] = [-2]");
        free_a(r); free_a(a);
    }

    /* ─── square ─── */
    TEST_SECTION("square");
    {
        Array *a = make_f64_1d((double[]){1, 2, 3, 4}, 4);
        Array *r = square(a);
        ASSERT_NOTNULL(r, "square not null");
        ASSERT_EQ_ARR((double*)r->data, ((double[]){1, 4, 9, 16}), 4, TOL_F64, "square [1,2,3,4] = [1,4,9,16]");
        free_a(r); free_a(a);
    }
    {
        Array *a = make_i32_1d((int[]){2, 3}, 2);
        Array *r = square(a);
        ASSERT_NOTNULL(r, "square INT32 not null");
        int64_t *sq_d = (int64_t*)r->data;
        ASSERT_EQ_I64(sq_d[0], 4, "square INT32 [0] = 4");
        ASSERT_EQ_I64(sq_d[1], 9, "square INT32 [1] = 9");
        ASSERT_DTYPE(r, INT64, "square INT32 -> INT64");
        free_a(r); free_a(a);
    }

    TEST_SUMMARY();
    return (g_fail > 0) ? 1 : 0;
}
