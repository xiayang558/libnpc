/**
 * test_npzfile.c — Tests for npzfile_open, npzfile_close, npzfile_keys,
 *                  npzfile_contains, npzfile_get, bagobj_get,
 *                  parse_npy_header, descr_to_dtype
 */
#include <string.h>
#include "test_utils.h"
#include "io.h"
#include "npzfile.h"
#include <unistd.h>

int main(void) {
    TEST_MAIN("npzfile_open / close / keys / contains / get / bagobj / "
              "parse_npy_header / descr_to_dtype");

    /* ── Create a .npz file for testing ─────────────────────────────── */
    {
        Array *a = make_i32_1d((int[]){100, 200, 300}, 3);
        Array *b = make_f64_1d((double[]){1.1, 2.2, 3.3, 4.4}, 4);
        const char *keys[] = {"arr_a", "arr_b"};
        Array *vals[] = {a, b};
        int ret = savez("/tmp/test_npzfile.npz", 0, NULL, 2, keys, vals);
        ASSERT_EQ_INT(ret, 0, "savez for npzfile test");
        free_a(a); free_a(b);
    }

    /* ── npzfile_open ───────────────────────────────────────────────── */
    {
        NpzFile *npz = npzfile_open("/tmp/test_npzfile.npz", false, 0);
        ASSERT_NOTNULL((void*)npz, "npzfile_open");

        if (npz) {
            /* ── npzfile_keys ───────────────────────────────────────── */
            int nkeys = 0;
            const char **keys = npzfile_keys(npz, &nkeys);
            ASSERT_NOTNULL((void*)keys, "npzfile_keys");
            ASSERT_EQ_INT(nkeys, 2, "npzfile_keys count=2");
            if (keys && nkeys >= 2) {
                /* verify key names contain arr_a and arr_b */
                int found_a = 0, found_b = 0;
                for (int i = 0; i < nkeys; i++) {
                    if (strcmp(keys[i], "arr_a") == 0) found_a = 1;
                    if (strcmp(keys[i], "arr_b") == 0) found_b = 1;
                }
                ASSERT_TRUE(found_a, "keys contains arr_a");
                ASSERT_TRUE(found_b, "keys contains arr_b");
            }

            /* ── npzfile_contains ───────────────────────────────────── */
            ASSERT_TRUE(npzfile_contains(npz, "arr_a"), "contains arr_a -> true");
            ASSERT_TRUE(!npzfile_contains(npz, "nonexistent"), "contains nonexistent -> false");

            /* ── npzfile_get ────────────────────────────────────────── */
            Array *ga = npzfile_get(npz, "arr_a");
            ASSERT_NOTNULL(ga, "npzfile_get arr_a");
            if (ga) {
                ASSERT_DTYPE(ga, INT32, "npzfile_get dtype INT32");
                ASSERT_SIZE(ga, 3, "npzfile_get size=3");
                int expected[] = {100, 200, 300};
                ASSERT_EQ_IARR((int*)ga->data, expected, 3, "npzfile_get arr_a values");
            }

            Array *gb = npzfile_get(npz, "arr_b");
            ASSERT_NOTNULL(gb, "npzfile_get arr_b");
            if (gb) {
                ASSERT_DTYPE(gb, FLOAT64, "npzfile_get dtype FLOAT64");
                ASSERT_SIZE(gb, 4, "npzfile_get arr_b size=4");
                double expected[] = {1.1, 2.2, 3.3, 4.4};
                ASSERT_EQ_ARR((double*)gb->data, expected, 4, TOL_F64, "npzfile_get arr_b values");
            }

            /* ── bagobj_get ─────────────────────────────────────────── */
            BagObj *bag = npzfile_get_bag(npz);
            ASSERT_NOTNULL((void*)bag, "npzfile_get_bag");
            if (bag) {
                Array *bg = bagobj_get(bag, "arr_a");
                ASSERT_NOTNULL(bg, "bagobj_get arr_a");
                if (bg) {
                    ASSERT_SIZE(bg, 3, "bagobj_get size=3");
                }
                free_a(bg);
            }

            free_a(ga); free_a(gb);
            npzfile_close(npz);
        }
    }

    /* ── parse_npy_header ───────────────────────────────────────────── */
    {
        /* Create a temporary .npy file and read its header */
        Array *a = make_i32_1d((int[]){1, 2, 3}, 3);
        save_npy("/tmp/test_parse_header.npy", a);
        free_a(a);

        /* Read the header from the file */
        FILE *f = fopen("/tmp/test_parse_header.npy", "rb");
        ASSERT_TRUE(f != NULL, "open npy for header parsing");
        if (f) {
            /* read magic string */
            char magic[6];
            fread(magic, 1, 6, f);
            /* read version */
            uint8_t major, minor;
            fread(&major, 1, 1, f);
            fread(&minor, 1, 1, f);
            /* read header length */
            uint16_t hdr_len = 0;
            if (major == 1) {
                fread(&hdr_len, 2, 1, f);
            } else if (major >= 2) {
                uint32_t hdr_len32 = 0;
                fread(&hdr_len32, 4, 1, f);
                hdr_len = (uint16_t)hdr_len32;
            }
            /* read header string */
            char *hdr = (char*)malloc(hdr_len + 1);
            if (hdr) {
                fread(hdr, 1, hdr_len, f);
                hdr[hdr_len] = '\0';

                char *descr = NULL;
                bool fortran_order = false;
                int *shape = NULL;
                int ndim = 0;
                int ret = parse_npy_header(hdr, &descr, &fortran_order, &shape, &ndim);
                ASSERT_EQ_INT(ret, 0, "parse_npy_header returns 0");
                ASSERT_TRUE(descr != NULL, "parse_npy_header descr");
                ASSERT_EQ_INT(ndim, 1, "parse_npy_header ndim=1");
                if (shape) {
                    ASSERT_EQ_INT(shape[0], 3, "parse_npy_header shape[0]=3");
                }
                free(descr); free(shape);
                free(hdr);
            }
            fclose(f);
        }
        unlink("/tmp/test_parse_header.npy");
    }

    /* ── descr_to_dtype ─────────────────────────────────────────────── */
    {
        ASSERT_EQ_INT((int)descr_to_dtype("<i4"), (int)INT32, "descr_to_dtype <i4 -> INT32");
        ASSERT_EQ_INT((int)descr_to_dtype("<f8"), (int)FLOAT64, "descr_to_dtype <f8 -> FLOAT64");
        ASSERT_EQ_INT((int)descr_to_dtype("|b1"), (int)BOOL, "descr_to_dtype |b1 -> BOOL");
        ASSERT_EQ_INT((int)descr_to_dtype("<i8"), (int)INT64, "descr_to_dtype <i8 -> INT64");
    }

    /* ── Edge: npzfile_contains after close should not crash ────────── */
    /* We test a second open/close to ensure no leaks */
    {
        NpzFile *npz = npzfile_open("/tmp/test_npzfile.npz", false, 0);
        ASSERT_NOTNULL((void*)npz, "npzfile_open second time");
        if (npz) {
            /* get again to verify idempotent */
            Array *ga = npzfile_get(npz, "arr_a");
            ASSERT_NOTNULL(ga, "npzfile_get second time");
            free_a(ga);
            npzfile_close(npz);
        }
    }

    /* ── Edge: npzfile_get nonexistent key ──────────────────────────── */
    {
        NpzFile *npz = npzfile_open("/tmp/test_npzfile.npz", false, 0);
        if (npz) {
            Array *ga = npzfile_get(npz, "nonexistent_key");
            ASSERT_TRUE(ga == NULL, "npzfile_get nonexistent -> NULL");
            free_a(ga);
            npzfile_close(npz);
        }
    }

    /* Clean up */
    unlink("/tmp/test_npzfile.npz");

    TEST_SUMMARY();
    return (g_fail > 0) ? 1 : 0;
}
