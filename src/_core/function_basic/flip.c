#include "array.h"


Array* flip(Array *arr, int axis) {
    if (arr == NULL) {
        fprintf(stderr, "flip: NULL array argument\n");
        return NULL;
    }
    int ndim = arr->ndim;
    // handle negative axis
    if (axis < 0) axis += ndim;
    if (axis < 0 || axis >= ndim) {
        fprintf(stderr, "flip: axis %d out of bounds for array of dimension %d\n", axis, ndim);
        return NULL;
    }
    // create result array, same shape, same dtype
    Array *res = create_array(arr->shape, ndim, arr->dtype);
    if (res == NULL) return NULL;

    size_t elem_sz = dtype_size(arr->dtype);
    char *src = (char*)arr->data;
    char *dst = (char*)res->data;

    // compute strides (in units of element count)
    int *strides = malloc(ndim * sizeof(int));
    if (!strides) {
        free_array(res);
        return NULL;
    }
    strides[ndim - 1] = 1;
    for (int i = ndim - 2; i >= 0; i--) {
        strides[i] = strides[i + 1] * arr->shape[i + 1];
    }

    // multi-dimensional index array (used for traversing coordinates)
    int *coords = calloc(ndim, sizeof(int));
    if (!coords) {
        free(strides);
        free_array(res);
        return NULL;
    }

    // iterate over the source elements, compute flipped destination offset
    for (int flat = 0; flat < arr->size; flat++) {
        // compute source offset
        int src_off = 0;
        for (int i = 0; i < ndim; i++) {
            src_off += coords[i] * strides[i];
        }
        // compute destination offset: reverse the coordinate along the axis direction
        int dst_off = 0;
        for (int i = 0; i < ndim; i++) {
            int coord = (i == axis) ? (arr->shape[i] - 1 - coords[i]) : coords[i];
            dst_off += coord * strides[i];
        }
        // copy data
        memcpy(dst + dst_off * elem_sz, src + src_off * elem_sz, elem_sz);

        // update coordinates (increment by one in column-major order)
        int carry = 1;
        for (int i = ndim - 1; i >= 0 && carry; i--) {
            coords[i]++;
            if (coords[i] >= arr->shape[i]) {
                coords[i] = 0;
                carry = 1;
            } else {
                carry = 0;
            }
        }
    }

    free(strides);
    free(coords);
    return res;
}