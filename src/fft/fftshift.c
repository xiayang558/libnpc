#include <stdlib.h>
#include <string.h>
#include "array.h"
#include "utils.h"

/**
 * fftshift — Shift zero-frequency component to center of spectrum (like numpy.fft.fftshift)
 *
 * Swap the first half with the second half along each axis.
 *
 * @param arr Input array (any ndim, any dtype)
 * @return New array with same shape and dtype
 */
Array* fftshift(Array *arr) {
    if (arr == NULL) {
        fprintf(stderr, "fftshift: NULL argument\n");
        return NULL;
    }

    int ndim = arr->ndim;
    if (ndim == 0) {
        /* 0-d array: return a copy */
        Array *result = create_array((int[]){1}, 1, arr->dtype);
        if (!result) return NULL;
        memcpy(result->data, arr->data, dtype_size(arr->dtype));
        return result;
    }

    size_t esz = dtype_size(arr->dtype);
    char *src = (char*)arr->data;
    char *dst_data = malloc(arr->size * esz);
    if (!dst_data) return NULL;

    int *strides = malloc(ndim * sizeof(int));
    if (!strides) { free(dst_data); return NULL; }
    compute_strides(arr->shape, ndim, strides);

    /* Copy each element to shifted position */
    for (int flat = 0; flat < arr->size; flat++) {
        /* Decode flat index to multi-index */
        int src_pos[ndim];
        int remaining = flat;
        for (int d = ndim - 1; d >= 0; d--) {
            src_pos[d] = remaining % arr->shape[d];
            remaining /= arr->shape[d];
        }

        /* Compute shifted index for each dimension */
        int dst_flat = 0;
        for (int d = 0; d < ndim; d++) {
            int n = arr->shape[d];
            int shift = n / 2;
            int dst_idx = (src_pos[d] + shift) % n;
            dst_flat += dst_idx * strides[d];
        }

        memcpy(dst_data + dst_flat * esz, src + flat * esz, esz);
    }

    free(strides);

    Array *result = create_array(arr->shape, arr->ndim, arr->dtype);
    if (!result) { free(dst_data); return NULL; }
    memcpy(result->data, dst_data, arr->size * esz);
    free(dst_data);
    return result;
}
