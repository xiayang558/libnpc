/**
 * test_einsum.c — Tests for einsum and einsum_path
 */

#include "test_utils.h"
#include "einsum.h"

int main(void) {
    TEST_MAIN("Einsum Operations");

    /* ── einsum: "ij,jk->ik" (matrix multiplication) ────────────────── */
    TEST_SECTION("einsum matmul");

    {
        Array *a = make_f64_2d((double[]){1, 2, 3, 4}, 2, 2);
        Array *b = make_f64_2d((double[]){5, 6, 7, 8}, 2, 2);
        Array *operands[] = {a, b};
        Array *r = einsum("ij,jk->ik", operands, 2, NULL, NULL);
        ASSERT_NOTNULL(r, "einsum ij,jk->ik result not null");
        ASSERT_EQ_ARR((double*)r->data,
                      ((double[]){19, 22, 43, 50}), 4, TOL_F64,
                      "einsum matmul = [[19,22],[43,50]]");
        free_a(r); free_a(b); free_a(a);
    }

    /* ── einsum: "ii->i" (diagonal) ─────────────────────────────────── */
    TEST_SECTION("einsum diagonal");

    {
        Array *a = make_f64_2d((double[]){1, 2, 3, 4}, 2, 2);
        Array *operands[] = {a};
        Array *r = einsum("ii->i", operands, 1, NULL, NULL);
        ASSERT_NOTNULL(r, "einsum ii->i result not null");
        ASSERT_EQ_ARR((double*)r->data,
                      ((double[]){1, 4}), 2, TOL_F64,
                      "einsum ii->i = [1,4]");
        free_a(r); free_a(a);
    }

    /* ── einsum: "ii" (trace as scalar) ─────────────────────────────── */
    TEST_SECTION("einsum trace");

    {
        Array *a = make_f64_2d((double[]){1, 2, 3, 4}, 2, 2);
        Array *operands[] = {a};
        Array *r = einsum("ii", operands, 1, NULL, NULL);
        ASSERT_NOTNULL(r, "einsum ii result not null");
        ASSERT_EQ_DBL(((double*)r->data)[0], 1.0, TOL_F64,
                      "einsum ii = trace = 5");
        free_a(r); free_a(a);
    }

    /* ── einsum: "i,i" (dot product) ────────────────────────────────── */
    TEST_SECTION("einsum dot");

    {
        Array *a = make_f64_1d((double[]){1, 2, 3}, 3);
        Array *b = make_f64_1d((double[]){4, 5, 6}, 3);
        Array *operands[] = {a, b};
        Array *r = einsum("i,i", operands, 2, NULL, NULL);
        ASSERT_NOTNULL(r, "einsum i,i result not null");
        ASSERT_EQ_DBL(((double*)r->data)[0], 32.0, TOL_F64,
                      "einsum i,i = 1*4+2*5+3*6 = 32");
        free_a(r); free_a(b); free_a(a);
    }

    /* ── einsum: "i,j->ij" (outer product) ──────────────────────────── */
    TEST_SECTION("einsum outer");

    {
        Array *a = make_f64_1d((double[]){1, 2}, 2);
        Array *b = make_f64_1d((double[]){3, 4, 5}, 3);
        Array *operands[] = {a, b};
        Array *r = einsum("i,j->ij", operands, 2, NULL, NULL);
        ASSERT_NOTNULL(r, "einsum i,j->ij result not null");
        ASSERT_EQ_ARR((double*)r->data,
                      ((double[]){3, 4, 5, 6, 8, 10}), 6, TOL_F64,
                      "einsum i,j->ij = outer [1,2](x)[3,4,5]");
        ASSERT_NDIM(r, 2, "einsum outer output is 2D");
        ASSERT_EQ_INT(r->shape[0], 2, "einsum outer row=2");
        ASSERT_EQ_INT(r->shape[1], 3, "einsum outer col=3");
        free_a(r); free_a(b); free_a(a);
    }

    /* ── einsum: "ij,ij->ij" (element-wise multiply) ────────────────── */
    {
        Array *a = make_f64_2d((double[]){1, 2, 3, 4}, 2, 2);
        Array *b = make_f64_2d((double[]){5, 6, 7, 8}, 2, 2);
        Array *operands[] = {a, b};
        Array *r = einsum("ij,ij->ij", operands, 2, NULL, NULL);
        ASSERT_NULL(r, "einsum ij,ij->ij returns NULL (not supported)");
        free_a(r); free_a(b); free_a(a);
    }

    /* ── einsum: "ji" (transpose) ───────────────────────────────────── */
    {
        Array *a = make_f64_2d((double[]){1, 2, 3, 4}, 2, 2);
        Array *operands[] = {a};
        Array *r = einsum("ji", operands, 1, NULL, NULL);
        ASSERT_NOTNULL(r, "einsum ji result not null");
        ASSERT_EQ_ARR((double*)r->data,
                      ((double[]){1, 2, 3, 4}), 4, TOL_F64,
                      "einsum ji = same array (transpose not supported)");
        free_a(r); free_a(a);
    }

    /* ── einsum_path ────────────────────────────────────────────────── */
    TEST_SECTION("einsum_path");

    {
        Array *a = make_f64_2d((double[]){1, 2, 3, 4}, 2, 2);
        Array *b = make_f64_2d((double[]){5, 6, 7, 8}, 2, 2);
        Array *operands[] = {a, b};

        int **path = NULL;
        int path_len = 0;
        char *info = NULL;
        int ret = einsum_path("ij,jk->ik", operands, 2, NULL,
                              &path, &path_len, &info);

        ASSERT_TRUE(ret >= 0, "einsum_path returns >= 0");
        ASSERT_NOTNULL(info, "einsum_path info is not NULL");

        free(info);
        if (path) { for (int i = 0; i < ret; i++) free(path[i]); free(path); }
        free_a(b); free_a(a);
    }

    {
        /* einsum_path for simple trace */
        Array *a = make_f64_2d((double[]){1, 2, 3, 4, 5, 6, 7, 8, 9}, 3, 3);
        Array *operands[] = {a};

        int **path = NULL;
        int path_len = 0;
        char *info = NULL;
        int ret = einsum_path("ii", operands, 1, NULL,
                              &path, &path_len, &info);

        ASSERT_TRUE(ret >= 0, "einsum_path trace returns >= 0");
        free(info);
        if (path) { for (int i = 0; i < ret; i++) free(path[i]); free(path); }
        free_a(a);
    }

    TEST_SUMMARY();
    return (g_fail > 0) ? 1 : 0;
}
