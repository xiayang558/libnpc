#include "def.h"
#include "array.h"
#include "shape.h"
#include <string.h>

/**
 * roll  --  roll array elements along an axis (like numpy.roll)
 *
 * positive shift: elements move towards right/down
 * negative shift: elements move towards left/up
 *
 * @param arr   Input array
 * @param shift offset amount
 * @param axis  axis along which to roll, -1 means flatten then roll back and restore original shape
 * @return rolled new array (copy)
 */
Array* roll(Array *arr, int shift, int axis) {
    if (arr == NULL) {
        fprintf(stderr, "roll: NULL array argument\n");
        return NULL;
    }

    int ndim = arr->ndim;
    size_t elem_sz = dtype_size(arr->dtype);

    if (axis == -1) {
        /* Flatten → roll → preserve original shape */
        int n = arr->size;
        if (n == 0) {
            Array *res = create_array(arr->shape, ndim, arr->dtype);
            return res;
        }

        /* Normalize shift into [0, n) */
        int s = shift % n;
        if (s < 0) s += n;

        char *src = (char*)arr->data;
        Array *res = create_array(arr->shape, ndim, arr->dtype);
        if (res == NULL) return NULL;
        char *dst = (char*)res->data;

        /* src[0..n-s-1] → dst[s..n-1] (first n-s elements shift right) */
        memcpy(dst + s * elem_sz, src, (n - s) * elem_sz);
        /* src[n-s..n-1] → dst[0..s-1] (last s elements wrap to front) */
        memcpy(dst, src + (n - s) * elem_sz, s * elem_sz);
        return res;
    }

    /* Axis-specific roll */
    if (axis < 0) axis += ndim;
    if (axis < 0 || axis >= ndim) {
        fprintf(stderr, "roll: axis %d out of bounds for %d-d array\n", axis, ndim);
        return NULL;
    }

    int axis_len = arr->shape[axis];
    if (axis_len == 0) {
        return create_array(arr->shape, ndim, arr->dtype);
    }

    /* Normalize shift into [0, axis_len) */
    int s = shift % axis_len;
    if (s < 0) s += axis_len;

    Array *res = create_array(arr->shape, ndim, arr->dtype);
    if (res == NULL) return NULL;

    /* Compute element-level strides */
    int *strides = malloc(ndim * sizeof(int));
    if (!strides) { free_array(res); return NULL; }
    compute_strides(arr->shape, ndim, strides);

    int outer_dims = 1;
    for (int i = 0; i < axis; i++) outer_dims *= arr->shape[i];

    int inner_dims = 1;
    for (int i = axis + 1; i < ndim; i++) inner_dims *= arr->shape[i];

    size_t src_slice = strides[axis] * elem_sz;
    size_t block_sz = inner_dims * elem_sz;

    /* roll_len is the length of the axis we're rolling along */
    int roll_len = axis_len;

    for (int outer = 0; outer < outer_dims; outer++) {
        char *src_base = (char*)arr->data + outer * src_slice * roll_len;
        char *dst_base = (char*)res->data + outer * src_slice * roll_len;

        /* src[k] → dst[(k + s) % len] for each block along axis */
        for (int k = 0; k < roll_len; k++) {
            int dst_k = (k + s) % roll_len;
            memcpy(dst_base + dst_k * block_sz,
                   src_base + k * block_sz, block_sz);
        }
    }

    free(strides);
    return res;
}
