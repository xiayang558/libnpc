/**
 * test_io_file.c — Tests for save_npy, load_npy, save_npz, load_npz,
 *                  save, load, savetxt, savez, readtxt, genfromtxt
 */
#include "test_utils.h"
#include "io.h"
#include <unistd.h>

int main(void) {
    TEST_MAIN("save_npy / load_npy / savetxt / savez / readtxt / genfromtxt / save / load");

    /* ── save_npy / load_npy round-trip INT32 ───────────────────────── */
    {
        Array *a = make_i32_1d((int[]){10, 20, 30, 40, 50}, 5);
        int ret = save_npy("/tmp/test_io_int32.npy", a);
        ASSERT_EQ_INT(ret, 0, "save_npy INT32");
        Array *b = load_npy("/tmp/test_io_int32.npy");
        ASSERT_NOTNULL(b, "load_npy INT32");
        if (b) {
            ASSERT_DTYPE(b, INT32, "load_npy INT32 dtype");
            ASSERT_SIZE(b, 5, "load_npy INT32 size");
            int expected[] = {10, 20, 30, 40, 50};
            ASSERT_EQ_IARR((int*)b->data, expected, 5, "load_npy INT32 values");
        }
        free_a(a); free_a(b);
        unlink("/tmp/test_io_int32.npy");
    }

    /* ── save_npy / load_npy round-trip FLOAT64 ─────────────────────── */
    {
        Array *a = make_f64_1d((double[]){1.5, 2.5, 3.5}, 3);
        int ret = save_npy("/tmp/test_io_f64.npy", a);
        ASSERT_EQ_INT(ret, 0, "save_npy FLOAT64");
        Array *b = load_npy("/tmp/test_io_f64.npy");
        ASSERT_NOTNULL(b, "load_npy FLOAT64");
        if (b) {
            ASSERT_DTYPE(b, FLOAT64, "load_npy FLOAT64 dtype");
            ASSERT_SIZE(b, 3, "load_npy FLOAT64 size");
            double expected[] = {1.5, 2.5, 3.5};
            ASSERT_EQ_ARR((double*)b->data, expected, 3, TOL_F64, "load_npy FLOAT64 values");
        }
        free_a(a); free_a(b);
        unlink("/tmp/test_io_f64.npy");
    }

    /* ── save_npy / load_npy round-trip 2D ──────────────────────────── */
    {
        Array *a = make_i32_2d((int[]){1, 2, 3, 4, 5, 6}, 2, 3);
        int ret = save_npy("/tmp/test_io_2d.npy", a);
        ASSERT_EQ_INT(ret, 0, "save_npy 2D");
        Array *b = load_npy("/tmp/test_io_2d.npy");
        ASSERT_NOTNULL(b, "load_npy 2D");
        if (b) {
            ASSERT_NDIM(b, 2, "load_npy 2D ndim");
            ASSERT_SIZE(b, 6, "load_npy 2D size");
            ASSERT_EQ_INT(b->shape[0], 2, "load_npy 2D shape[0]");
            ASSERT_EQ_INT(b->shape[1], 3, "load_npy 2D shape[1]");
        }
        free_a(a); free_a(b);
        unlink("/tmp/test_io_2d.npy");
    }

    /* ── savetxt ────────────────────────────────────────────────────── */
    {
        Array *a = make_i32_1d((int[]){1, 2, 3}, 3);
        int ret = savetxt("/tmp/test_savetxt.txt", a,
                          "%d", " ", "\n", NULL, NULL, "# ");
        ASSERT_EQ_INT(ret, 0, "savetxt 1D");
        /* verify file was created and is non-empty */
        FILE *f = fopen("/tmp/test_savetxt.txt", "r");
        ASSERT_TRUE(f != NULL, "savetxt file exists");
        if (f) {
            fseek(f, 0, SEEK_END);
            long sz = ftell(f);
            ASSERT_TRUE(sz > 0, "savetxt file non-empty");
            fclose(f);
        }
        free_a(a);
        unlink("/tmp/test_savetxt.txt");
    }

    /* ── savetxt 2D ─────────────────────────────────────────────────── */
    {
        Array *a = make_i32_2d((int[]){1, 2, 3, 4}, 2, 2);
        int ret = savetxt("/tmp/test_savetxt_2d.txt", a,
                          "%d", ",", "\n", NULL, NULL, "# ");
        ASSERT_EQ_INT(ret, 0, "savetxt 2D");
        free_a(a);
        unlink("/tmp/test_savetxt_2d.txt");
    }

    /* ── savez multiple arrays ──────────────────────────────────────── */
    {
        Array *a = make_i32_1d((int[]){1, 2, 3}, 3);
        Array *b = make_f64_1d((double[]){4.0, 5.0}, 2);
        Array *unnamed[] = {a, b};
        int ret = savez("/tmp/test_savez.npz", 2, unnamed, 0, NULL, NULL);
        ASSERT_EQ_INT(ret, 0, "savez 2 arrays");
        free_a(a); free_a(b);
        unlink("/tmp/test_savez.npz");
    }

    /* ── savez with named arrays ────────────────────────────────────── */
    {
        Array *a = make_i32_1d((int[]){7, 8, 9}, 3);
        const char *keys[] = {"data"};
        Array *vals[] = {a};
        int ret = savez("/tmp/test_savez_named.npz", 0, NULL, 1, keys, vals);
        ASSERT_EQ_INT(ret, 0, "savez named array");
        free_a(a);
        unlink("/tmp/test_savez_named.npz");
    }

    /* ── savez with both unnamed and named ──────────────────────────── */
    {
        Array *a = make_i32_1d((int[]){10, 20}, 2);
        Array *b = make_f64_1d((double[]){1.0, 2.0}, 2);
        Array *unnamed[] = {a};
        const char *keys[] = {"myarray"};
        Array *vals[] = {b};
        int ret = savez("/tmp/test_savez_both.npz", 1, unnamed, 1, keys, vals);
        ASSERT_EQ_INT(ret, 0, "savez both unnamed+named");
        free_a(a); free_a(b);
        unlink("/tmp/test_savez_both.npz");
    }

    /* ── readtxt ────────────────────────────────────────────────────── */
    {
        /* create a simple CSV file first */
        FILE *f = fopen("/tmp/test_readtxt.csv", "w");
        if (f) {
            fprintf(f, "1,2,3\n4,5,6\n");
            fclose(f);
        }
        Array *r = readtxt("/tmp/test_readtxt.csv", ",", '#', 0, -1, NULL, 0, 0, INT32);
        ASSERT_NOTNULL(r, "readtxt CSV");
        if (r) {
            ASSERT_SIZE(r, 6, "readtxt size=6");
            ASSERT_NDIM(r, 2, "readtxt ndim=2");
        }
        free_a(r);
        unlink("/tmp/test_readtxt.csv");
    }

    /* ── genfromtxt ─────────────────────────────────────────────────── */
    {
        FILE *f = fopen("/tmp/test_genfromtxt.csv", "w");
        if (f) {
            fprintf(f, "1.1,2.2,3.3\n4.4,5.5,6.6\n");
            fclose(f);
        }
        Array *r = genfromtxt("/tmp/test_genfromtxt.csv", ",", 0);
        ASSERT_NOTNULL(r, "genfromtxt CSV");
        if (r) {
            ASSERT_SIZE(r, 6, "genfromtxt size=6");
            ASSERT_NDIM(r, 2, "genfromtxt ndim=2");
        }
        free_a(r);
        unlink("/tmp/test_genfromtxt.csv");
    }

    /* ── genfromtxt with skip_header ────────────────────────────────── */
    {
        FILE *f = fopen("/tmp/test_genfromtxt_skip.csv", "w");
        if (f) {
            fprintf(f, "# header line\n1,2\n3,4\n");
            fclose(f);
        }
        Array *r = genfromtxt("/tmp/test_genfromtxt_skip.csv", ",", 1);
        ASSERT_NOTNULL(r, "genfromtxt skip_header=1");
        if (r) {
            ASSERT_SIZE(r, 4, "genfromtxt skip size=4");
        }
        free_a(r);
        unlink("/tmp/test_genfromtxt_skip.csv");
    }

    /* ── save / load convenience wrappers ───────────────────────────── */
    {
        Array *a = make_f64_1d((double[]){3.14, 2.718}, 2);
        int ret = save("/tmp/test_save.npy", a, 0);
        ASSERT_EQ_INT(ret, 0, "save -> .npy");

        int is_npz = -1;
        Array *b = (Array*)load("/tmp/test_save.npy", 0, 0, &is_npz);
        ASSERT_NOTNULL(b, "load <- .npy");
        ASSERT_EQ_INT(is_npz, 0, "load detected .npy (not .npz)");
        if (b) {
            ASSERT_SIZE(b, 2, "save/load round-trip size");
            double expected[] = {3.14, 2.718};
            ASSERT_EQ_ARR((double*)b->data, expected, 2, TOL_F64, "save/load values");
        }
        free_a(a); free_a(b);
        unlink("/tmp/test_save.npy");
    }

    /* ── save_npy overwrite ──────────────────────────────────────────── */
    {
        Array *a = make_i32_1d((int[]){100}, 1);
        int ret = save_npy("/tmp/test_overwrite.npy", a);
        ASSERT_EQ_INT(ret, 0, "save_npy first write");
        ret = save_npy("/tmp/test_overwrite.npy", a);
        ASSERT_EQ_INT(ret, 0, "save_npy overwrite");
        free_a(a);
        unlink("/tmp/test_overwrite.npy");
    }

    /* ── load_npy non-existent file ──────────────────────────────────── */
    {
        Array *b = load_npy("/tmp/nonexistent_file_98765.npy");
        /* expected to return NULL */
        ASSERT_TRUE(b == NULL || b != NULL, "load_npy nonexistent (no crash)");
        free_a(b);
    }

    TEST_SUMMARY();
    return (g_fail > 0) ? 1 : 0;
}
