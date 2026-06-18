#include <stdlib.h>
#include <string.h>
#include "array.h"

/**
 * rollaxis  --  roll the specified axis to a new position (like numpy.rollaxis)
 *
 * Equivalent to:
 *   if axis < start: start -= 1
 *   moveaxis(a, axis, start)
 *
 * @param a     Input array
 * @param axis  axis to roll
 * @param start target position
 * @return new array with reordered axes (copy)
 */
Array* rollaxis(Array *a, int axis, int start) {
    if (a == NULL) {
        fprintf(stderr, "rollaxis: NULL array argument\n");
        return NULL;
    }
    int ndim = a->ndim;
    if (ndim == 0) {
        return copy_array(a);
    }
    /* Normalize axes */
    if (axis < 0) axis += ndim;
    if (start < 0) start += ndim;
    if (axis < 0 || axis >= ndim || start < 0 || start > ndim) {
        fprintf(stderr, "rollaxis: axis out of range\n");
        return NULL;
    }

    /* rollaxis semantics: if axis < start, effective destination is start-1 */
    if (axis < start) start--;

    if (axis == start) {
        return copy_array(a);
    }

    /* Build new shape: remove source axis, insert at destination */
    int *new_shape = malloc(ndim * sizeof(int));
    if (!new_shape) return NULL;

    int idx = 0;
    for (int i = 0; i < ndim; i++) {
        if (i != axis) new_shape[idx++] = a->shape[i];
    }
    int axis_dim = a->shape[axis];
    for (int i = ndim - 1; i > start; i--) {
        new_shape[i] = new_shape[i - 1];
    }
    new_shape[start] = axis_dim;

    Array *result = create_array(new_shape, ndim, a->dtype);
    free(new_shape);
    if (!result) return NULL;

    /* Compute strides */
    int *a_strides = malloc(ndim * sizeof(int));
    int *res_strides = malloc(ndim * sizeof(int));
    if (!a_strides || !res_strides) {
        free(a_strides); free(res_strides);
        free_array(result);
        return NULL;
    }
    compute_strides(a->shape, ndim, a_strides);
    compute_strides(result->shape, ndim, res_strides);

    size_t elem_sz = dtype_size(a->dtype);
    char *a_data = (char*)a->data;
    char *res_data = (char*)result->data;

    int *indices = malloc(ndim * sizeof(int));
    if (!indices) {
        free(a_strides); free(res_strides);
        free_array(result);
        return NULL;
    }

    int total = result->size;
    for (int flat = 0; flat < total; flat++) {
        /* Decode result indices */
        int tmp = flat;
        for (int i = ndim - 1; i >= 0; i--) {
            indices[i] = tmp % result->shape[i];
            tmp /= result->shape[i];
        }

        /* Map to source indices using moveaxis convention:
         * non-source old axes get sequential new positions (skipping destination),
         * source old axis gets the destination position */
        int a_off = 0;
        int new_pos = 0;
        for (int i = 0; i < ndim; i++) {
            if (i == axis) continue;
            if (new_pos == start) new_pos++;
            a_off += indices[new_pos++] * a_strides[i];
        }
        a_off += indices[start] * a_strides[axis];

        size_t res_off = 0;
        for (int i = 0; i < ndim; i++) {
            res_off += indices[i] * res_strides[i];
        }
        memcpy(res_data + res_off * elem_sz, a_data + a_off * elem_sz, elem_sz);
    }

    free(a_strides);
    free(res_strides);
    free(indices);
    return result;
}
