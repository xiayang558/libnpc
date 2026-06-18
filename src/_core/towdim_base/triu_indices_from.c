#include "def.h"
#include "array.h"
#include "shape.h"

/**
 * triu_indices_from -- get upper triangle indices from array (like numpy.triu_indices_from)
 *
 * Equivalent to triu_indices(arr->shape[-2], k, arr->shape[-1]
 *
 * @param arr Input array (at least 2D)
 * @param k   Diagonal offset
 * @return 2×count INT64 index array
 */
Array* triu_indices_from(Array *arr, int k) {
    if (arr == NULL) {
        fprintf(stderr, "triu_indices_from: NULL array argument\n");
        return NULL;
    }
    if (arr->ndim < 2) {
        fprintf(stderr, "triu_indices_from: array must have at least 2 dimensions\n");
        return NULL;
    }
    int n = arr->shape[arr->ndim - 2];
    int m = arr->shape[arr->ndim - 1];
    return triu_indices(n, k, m);
}
