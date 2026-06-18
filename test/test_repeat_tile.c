#include "test_utils.h"
#include <stdint.h>


int main(void) {
    TEST_MAIN("test_repeat_tile");

    /* ── repeat requires INT64 repeats ────────────────────────────── */
    TEST_SECTION("repeat");

    /* Helper: create INT64 1D array from int64_t data */
    /* repeat [1,2,3] with repeats=[2,2,2] axis=-1 -> [1,1,2,2,3,3] */
    {
        double data[] = {1, 2, 3};
        double expected[] = {1, 1, 2, 2, 3, 3};
        Array *a = make_f64_1d(data, 3);
        int64_t rep[] = {2, 2, 2};
        Array *repeats = create_array((int[]){3}, 1, INT64);
        if (repeats) memcpy(repeats->data, rep, 3 * sizeof(int64_t));
        Array *r = repeat(a, repeats, -1);
        ASSERT_NOTNULL(r, "repeat F64 non-null");
        ASSERT_SIZE(r, 6, "repeat F64 size=6");
        ASSERT_EQ_ARR((double*)r->data, expected, 6, TOL_F64, "repeat F64 values");
        free_a(r);
        free_a(repeats);
        free_a(a);
    }

    /* repeat INT32 with varied repeats */
    {
        int data[] = {10, 20, 30};
        int32_t expected[] = {10, 20, 20, 30, 30, 30};
        Array *a = make_i32_1d(data, 3);
        int64_t rep[] = {1, 2, 3};
        Array *repeats = create_array((int[]){3}, 1, INT64);
        if (repeats) memcpy(repeats->data, rep, 3 * sizeof(int64_t));
        Array *r = repeat(a, repeats, -1);
        ASSERT_NOTNULL(r, "repeat INT32 non-null");
        ASSERT_SIZE(r, 6, "repeat INT32 size=6");
        ASSERT_EQ_IARR((int32_t*)r->data, expected, 6, "repeat INT32 values");
        free_a(r);
        free_a(repeats);
        free_a(a);
    }

    /* repeat 2D with axis=0 (repeat output is FLOAT64 for INT32 input) */
    {
        double data[] = {1, 2, 3, 4}; /* 2x2 */
        double expected[] = {1, 2, 1, 2, 3, 4, 3, 4};
        Array *a = make_f64_2d(data, 2, 2);
        int64_t rep[] = {2, 2};
        Array *repeats = create_array((int[]){2}, 1, INT64);
        if (repeats) memcpy(repeats->data, rep, 2 * sizeof(int64_t));
        Array *r = repeat(a, repeats, 0);
        ASSERT_NOTNULL(r, "repeat 2D axis=0 non-null");
        ASSERT_NDIM(r, 2, "repeat 2D ndim=2");
        ASSERT_EQ_INT(r->shape[0], 4, "repeat 2D shape[0]=4");
        ASSERT_EQ_INT(r->shape[1], 2, "repeat 2D shape[1]=2");
        ASSERT_EQ_ARR((double*)r->data, expected, 8, TOL_F64, "repeat 2D axis=0 values");
        free_a(r);
        free_a(repeats);
        free_a(a);
    }

    /* ── tile ────────────────────────────────────────────────────── */
    TEST_SECTION("tile");

    /* tile [1,2] with reps=[2] nreps=1 -> [1,2,1,2] */
    {
        double data[] = {1, 2};
        double expected[] = {1, 2, 1, 2};
        Array *a = make_f64_1d(data, 2);
        int reps[] = {2};
        Array *t = tile(a, reps, 1);
        ASSERT_NOTNULL(t, "tile 1D non-null");
        ASSERT_NDIM(t, 1, "tile 1D ndim=1");
        ASSERT_SIZE(t, 4, "tile 1D size=4");
        ASSERT_EQ_ARR((double*)t->data, expected, 4, TOL_F64, "tile 1D values");
        free_a(t);
        free_a(a);
    }

    /* tile [1,2] with reps=[2,2] nreps=2 -> [[1,2,1,2],[1,2,1,2]] */
    {
        double data[] = {1, 2};
        double expected[] = {1, 2, 1, 2, 1, 2, 1, 2};
        Array *a = make_f64_1d(data, 2);
        int reps[] = {2, 2};
        Array *t = tile(a, reps, 2);
        ASSERT_NOTNULL(t, "tile 2D non-null");
        ASSERT_NDIM(t, 2, "tile 2D ndim=2");
        ASSERT_EQ_INT(t->shape[0], 2, "tile 2D shape[0]=2");
        ASSERT_EQ_INT(t->shape[1], 4, "tile 2D shape[1]=4");
        ASSERT_EQ_ARR((double*)t->data, expected, 8, TOL_F64, "tile 2D values");
        free_a(t);
        free_a(a);
    }

    /* tile INT32 */
    {
        int data[] = {10, 20, 30};
        int expected[] = {10, 20, 30, 10, 20, 30};
        Array *a = make_i32_1d(data, 3);
        int reps[] = {2};
        Array *t = tile(a, reps, 1);
        ASSERT_NOTNULL(t, "tile INT32 non-null");
        ASSERT_SIZE(t, 6, "tile INT32 size=6");
        ASSERT_EQ_IARR((int*)t->data, expected, 6, "tile INT32 values");
        free_a(t);
        free_a(a);
    }

    /* ── roll ────────────────────────────────────────────────────── */
    TEST_SECTION("roll");

    /* roll [1,2,3,4,5] shift=2 axis=-1 -> [4,5,1,2,3] */
    {
        double data[] = {1, 2, 3, 4, 5};
        double expected[] = {4, 5, 1, 2, 3};
        Array *a = make_f64_1d(data, 5);
        Array *r = roll(a, 2, -1);
        ASSERT_NOTNULL(r, "roll shift=2 non-null");
        ASSERT_EQ_ARR((double*)r->data, expected, 5, TOL_F64, "roll shift=2 values");
        free_a(r);
        free_a(a);
    }

    /* roll shift=-1 -> [2,3,4,5,1] */
    {
        double data[] = {1, 2, 3, 4, 5};
        double expected[] = {2, 3, 4, 5, 1};
        Array *a = make_f64_1d(data, 5);
        Array *r = roll(a, -1, -1);
        ASSERT_NOTNULL(r, "roll shift=-1 non-null");
        ASSERT_EQ_ARR((double*)r->data, expected, 5, TOL_F64, "roll shift=-1 values");
        free_a(r);
        free_a(a);
    }

    /* roll shift=0 -> unchanged */
    {
        double data[] = {1, 2, 3};
        double expected[] = {1, 2, 3};
        Array *a = make_f64_1d(data, 3);
        Array *r = roll(a, 0, -1);
        ASSERT_NOTNULL(r, "roll shift=0 non-null");
        ASSERT_EQ_ARR((double*)r->data, expected, 3, TOL_F64, "roll shift=0 values");
        free_a(r);
        free_a(a);
    }

    /* roll INT32 shift=3 */
    {
        int data[] = {10, 20, 30, 40, 50};
        int expected[] = {30, 40, 50, 10, 20};
        Array *a = make_i32_1d(data, 5);
        Array *r = roll(a, 3, -1);
        ASSERT_NOTNULL(r, "roll INT32 non-null");
        ASSERT_EQ_IARR((int*)r->data, expected, 5, "roll INT32 values");
        free_a(r);
        free_a(a);
    }

    /* ── rot90 ───────────────────────────────────────────────────── */
    TEST_SECTION("rot90");

    /* rot90 2x3 k=1 -> 3x2 (counterclockwise) */
    {
        double data[] = {1, 2, 3, 4, 5, 6};
        double expected[] = {3, 6, 2, 5, 1, 4};
        Array *a = make_f64_2d(data, 2, 3);
        Array *r = rot90(a, 1, 0, 1);
        ASSERT_NOTNULL(r, "rot90 k=1 non-null");
        ASSERT_NDIM(r, 2, "rot90 ndim=2");
        ASSERT_EQ_INT(r->shape[0], 3, "rot90 shape[0]=3");
        ASSERT_EQ_INT(r->shape[1], 2, "rot90 shape[1]=2");
        ASSERT_EQ_ARR((double*)r->data, expected, 6, TOL_F64, "rot90 k=1 values");
        free_a(r);
        free_a(a);
    }

    /* rot90 k=2 (180 degrees) */
    {
        double data[] = {1, 2, 3, 4};
        double expected[] = {4, 3, 2, 1};
        Array *a = make_f64_2d(data, 2, 2);
        Array *r = rot90(a, 2, 0, 1);
        ASSERT_NOTNULL(r, "rot90 k=2 non-null");
        ASSERT_NDIM(r, 2, "rot90 k=2 ndim=2");
        ASSERT_EQ_INT(r->shape[0], 2, "rot90 k=2 shape[0]");
        ASSERT_EQ_INT(r->shape[1], 2, "rot90 k=2 shape[1]");
        ASSERT_EQ_ARR((double*)r->data, expected, 4, TOL_F64, "rot90 k=2 values");
        free_a(r);
        free_a(a);
    }

    /* rot90 INT32 */
    {
        int data[] = {1, 2, 3, 4, 5, 6};
        int expected[] = {3, 6, 2, 5, 1, 4};
        Array *a = make_i32_2d(data, 2, 3);
        Array *r = rot90(a, 1, 0, 1);
        ASSERT_NOTNULL(r, "rot90 INT32 non-null");
        ASSERT_NDIM(r, 2, "rot90 INT32 ndim=2");
        ASSERT_EQ_IARR((int*)r->data, expected, 6, "rot90 INT32 values");
        free_a(r);
        free_a(a);
    }

    /* ── take ────────────────────────────────────────────────────── */
    TEST_SECTION("take");

    /* take [10,20,30,40] indices=[0,2,3] -> [10,30,40] */
    {
        double data[] = {10, 20, 30, 40};
        double expected[] = {10, 30, 40};
        Array *a = make_f64_1d(data, 4);
        Array *indices = make_i32_1d((int[]){0, 2, 3}, 3);
        Array *t = take(a, indices, -1);
        ASSERT_NOTNULL(t, "take 1D non-null");
        ASSERT_SIZE(t, 3, "take 1D size=3");
        ASSERT_EQ_ARR((double*)t->data, expected, 3, TOL_F64, "take 1D values");
        free_a(t);
        free_a(indices);
        free_a(a);
    }

    /* take INT32 */
    {
        int data[] = {100, 200, 300, 400};
        int expected[] = {100, 300};
        Array *a = make_i32_1d(data, 4);
        Array *indices = make_i32_1d((int[]){0, 2}, 2);
        Array *t = take(a, indices, -1);
        ASSERT_NOTNULL(t, "take INT32 non-null");
        ASSERT_EQ_IARR((int*)t->data, expected, 2, "take INT32 values");
        free_a(t);
        free_a(indices);
        free_a(a);
    }

    /* take 2D with axis=0 */
    {
        double data[] = {1, 2, 3, 4, 5, 6}; /* 2x3 */
        double expected[] = {4, 5, 6, 1, 2, 3};
        Array *a = make_f64_2d(data, 2, 3);
        Array *indices = make_i32_1d((int[]){1, 0}, 2);
        Array *t = take(a, indices, 0);
        ASSERT_NOTNULL(t, "take 2D axis=0 non-null");
        ASSERT_NDIM(t, 2, "take 2D ndim=2");
        ASSERT_EQ_INT(t->shape[0], 2, "take 2D shape[0]");
        ASSERT_EQ_INT(t->shape[1], 3, "take 2D shape[1]");
        ASSERT_EQ_ARR((double*)t->data, expected, 6, TOL_F64, "take 2D axis=0 values");
        free_a(t);
        free_a(indices);
        free_a(a);
    }

    /* take 2D with axis=1 */
    {
        double data[] = {1, 2, 3, 4, 5, 6}; /* 2x3 */
        double expected[] = {2, 1, 5, 4};
        Array *a = make_f64_2d(data, 2, 3);
        Array *indices = make_i32_1d((int[]){1, 0}, 2);
        Array *t = take(a, indices, 1);
        ASSERT_NOTNULL(t, "take 2D axis=1 non-null");
        ASSERT_NDIM(t, 2, "take 2D axis=1 ndim=2");
        ASSERT_EQ_INT(t->shape[0], 2, "take 2D axis=1 shape[0]");
        ASSERT_EQ_INT(t->shape[1], 2, "take 2D axis=1 shape[1]");
        ASSERT_EQ_ARR((double*)t->data, expected, 4, TOL_F64, "take 2D axis=1 values");
        free_a(t);
        free_a(indices);
        free_a(a);
    }

    TEST_SUMMARY();
    return (g_fail > 0) ? 1 : 0;
}
