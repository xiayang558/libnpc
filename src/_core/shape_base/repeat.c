#include "def.h"
#include "array.h"
#include "shape.h"
#include <string.h>

/**
 * repeat — repeat array elements (like numpy.repeat)
 *
 * @param arr     Input array
 * @param repeats INT64 array, scalar or 1D (per-element repeat counts along axis)
 * @param axis    -1 to flatten then repeat, >=0 to repeat along specified axis
 * @return        New array with repeated elements
 */
Array* repeat(Array *arr, Array *repeats, int axis) {
    if (arr == NULL || repeats == NULL) {
        fprintf(stderr, "repeat: NULL argument\n");
        return NULL;
    }
    if (repeats->dtype != INT64) {
        fprintf(stderr, "repeat: repeats must be INT64\n");
        return NULL;
    }
    if (repeats->size == 0) {
        fprintf(stderr, "repeat: repeats must not be empty\n");
        return NULL;
    }

    int64_t *rep = (int64_t*)repeats->data;
    int nrep = repeats->size;
    size_t elem_sz = dtype_size(arr->dtype);

    /* ── axis == -1: flatten then repeat ── */
    if (axis < 0) {
        /* Compute total output size */
        int64_t total = 0;
        for (int i = 0; i < arr->size; i++)
            total += rep[i % nrep];

        int out_shape[1] = {(int)total};
        Array *result = create_array(out_shape, 1, arr->dtype);
        if (result == NULL) return NULL;

        char *src = (char*)arr->data;
        char *dst = (char*)result->data;
        size_t out_pos = 0;

        for (int i = 0; i < arr->size; i++) {
            int64_t r = rep[i % nrep];
            for (int64_t j = 0; j < r; j++) {
                memcpy(dst + out_pos * elem_sz, src + i * elem_sz, elem_sz);
                out_pos++;
            }
        }
        return result;
    }

    /* ── axis >= 0: repeat along specified axis ── */
    if (axis >= arr->ndim) {
        fprintf(stderr, "repeat: axis %d out of bounds for ndim %d\n", axis, arr->ndim);
        return NULL;
    }

    /* Validate repeats length matches axis size (unless scalar) */
    int ax_len = arr->shape[axis];
    if (nrep != 1 && nrep != ax_len) {
        fprintf(stderr, "repeat: repeats size %d must be 1 or match axis size %d\n",
                nrep, ax_len);
        return NULL;
    }

    /* Compute output shape */
    int *out_shape = (int*)malloc(arr->ndim * sizeof(int));
    if (out_shape == NULL) { fprintf(stderr, "repeat: malloc failed\n"); return NULL; }
    int64_t new_ax_len = 0;
    for (int i = 0; i < ax_len; i++)
        new_ax_len += rep[i % nrep];
    for (int d = 0; d < arr->ndim; d++)
        out_shape[d] = (d == axis) ? (int)new_ax_len : arr->shape[d];

    Array *result = create_array(out_shape, arr->ndim, arr->dtype);
    free(out_shape);
    if (result == NULL) return NULL;

    /* strides for input and output */
    int *in_strides  = (int*)malloc(arr->ndim * sizeof(int));
    int *out_strides = (int*)malloc(arr->ndim * sizeof(int));
    int *indices     = (int*)malloc(arr->ndim * sizeof(int));
    if (!in_strides || !out_strides || !indices) {
        free(in_strides); free(out_strides); free(indices);
        free_array(result); return NULL;
    }
    compute_strides(arr->shape, arr->ndim, in_strides);
    compute_strides(result->shape, result->ndim, out_strides);

    char *src = (char*)arr->data;
    char *dst = (char*)result->data;

    /* Iterate over output elements */
    for (int flat = 0; flat < result->size; flat++) {
        /* Decode output flat position to coordinates */
        int tmp = flat;
        for (int d = result->ndim - 1; d >= 0; d--) {
            indices[d] = tmp % result->shape[d];
            tmp /= result->shape[d];
        }

        /* Find source axis coordinate: which source element maps to this output position */
        int ax_c = indices[axis];
        int src_ax = 0;
        int accum = 0;
        for (int k = 0; k < ax_len; k++) {
            accum += (int)rep[k % nrep];
            if (ax_c < accum) { src_ax = k; break; }
        }
        indices[axis] = src_ax;

        /* Compute source byte offset */
        int src_off = 0;
        for (int d = 0; d < arr->ndim; d++)
            src_off += indices[d] * in_strides[d];

        memcpy(dst + flat * elem_sz, src + src_off * elem_sz, elem_sz);
    }

    free(in_strides); free(out_strides); free(indices);
    return result;
}
