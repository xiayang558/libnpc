#include "def.h"
#include "array.h"
#include "shape.h"
#include <string.h>

/**
 * tril  --  Return lower triangle of array, set the rest to zero (like numpy.tril)
 *
 * For 2D matrix: element[i,j], set to zero when j > i + k
 *
 * @param m Input array (at least 2D)
 * @param k diagonal offset, default 0
 * @return lower triangle new array (copy)
 */
Array* tril(Array *m, int k) {
    if (m == NULL) {
        fprintf(stderr, "tril: NULL array argument\n");
        return NULL;
    }
    if (m->ndim < 2) {
        fprintf(stderr, "tril: array must have at least 2 dimensions\n");
        return NULL;
    }

    /* Copy the array */
    Array *result = copy_array(m);
    if (result == NULL) return NULL;

    int N = m->shape[m->ndim - 2];   /* rows */
    int M = m->shape[m->ndim - 1];   /* cols */
    size_t elem_sz = dtype_size(m->dtype);
    char *data = (char*)result->data;

    /*
     * For each batch of leading dimensions, zero out elements above diagonal.
     * The last two dimensions are treated as (N, M) matrix.
     */
    int batch_size = result->size / (N * M);
    char zero_buf[32];
    memset(zero_buf, 0, elem_sz);

    for (int b = 0; b < batch_size; b++) {
        char *batch = data + b * N * M * elem_sz;
        for (int i = 0; i < N; i++) {
            for (int j = i + k + 1; j < M; j++) {
                memcpy(batch + (i * M + j) * elem_sz, zero_buf, elem_sz);
            }
        }
    }

    return result;
}
