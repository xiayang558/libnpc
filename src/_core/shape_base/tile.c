#include <stdlib.h>
#include <string.h>
#include "array.h"
#include "utils.h"

/**
 * tile — construct array by repeating input (like numpy.tile)
 *
 * @param arr   Input array
 * @param reps  Repetition count array
 * @param nreps Number of repetition dimensions
 * @return      New array with repeated blocks
 */
Array* tile(Array *arr, int *reps, int nreps) {
    if (arr == NULL || reps == NULL || nreps < 1) return NULL;

    /* Compute output ndim and shape */
    int out_ndim = (nreps > arr->ndim) ? nreps : arr->ndim;
    int *out_shape = malloc(out_ndim * sizeof(int));
    if (!out_shape) return NULL;

    for (int d = 0; d < out_ndim; d++) {
        int arr_dim = d + arr->ndim - out_ndim; /* Align to the right */
        int rep_dim = d + nreps - out_ndim;
        int arr_len = (arr_dim >= 0) ? arr->shape[arr_dim] : 1;
        int rep_count = (rep_dim >= 0) ? reps[rep_dim] : 1;
        if (rep_count < 1) rep_count = 1;
        out_shape[d] = arr_len * rep_count;
    }

    Array *result = create_array(out_shape, out_ndim, arr->dtype);
    if (!result) { free(out_shape); return NULL; }
    free(out_shape);

    size_t esz = dtype_size(arr->dtype);
    char *src = (char*)arr->data;
    char *dst = (char*)result->data;

    /* Flat iteration over output, mapping each element back to source */
    int total = result->size;
    int *idx = calloc(out_ndim, sizeof(int));
    if (!idx) { free_array(result); return NULL; }

    int *src_strides = malloc(arr->ndim * sizeof(int));
    int *rstrides = malloc(out_ndim * sizeof(int));
    compute_strides(arr->shape, arr->ndim, src_strides);
    compute_strides(result->shape, out_ndim, rstrides);

    for (int flat = 0; flat < total; flat++) {
        /* Decode flat to multi-index */
        int rem = flat;
        for (int d = out_ndim - 1; d >= 0; d--) {
            idx[d] = rem % result->shape[d];
            rem /= result->shape[d];
        }

        /* Map to source index: idx[d] % arr_shape[aligned_dim] */
        int src_off = 0;
        int off = out_ndim - arr->ndim; /* prepended dimension offset */
        for (int d = 0; d < arr->ndim; d++) {
            int si = idx[off + d] % arr->shape[d];
            src_off += si * src_strides[d];
        }

        memcpy(dst + flat * esz, src + src_off * esz, esz);
    }

    free(idx);
    free(src_strides);
    free(rstrides);
    return result;
}
