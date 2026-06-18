#include <math.h>
#include <complex.h>
#include <float.h>
#include "array.h"
#include "utils.h"


Array* nanargmin(Array *a, int axis) {
    if (a == NULL) {
        fprintf(stderr, "nanargmin: NULL array argument\n");
        return NULL;
    }
    int ndim = a->ndim;
    if (axis < -1 || axis >= ndim) {
        fprintf(stderr, "nanargmin: invalid axis %d for %dD array\n", axis, ndim);
        return NULL;
    }

    // handle axis = -1 (flattened)
    if (axis == -1) {
        int64_t min_idx = 0;
        int found = 0;
        double min_val = INFINITY;
        size_t elem_sz = dtype_size(a->dtype);
        char *data = (char*)a->data;
        for (int i = 0; i < a->size; ++i) {
            void *ptr = data + i * elem_sz;
            if (is_nan(ptr, a->dtype)) continue;
            double val = get_value(ptr, a->dtype);
            if (!found || val < min_val) {
                min_val = val;
                min_idx = i;
                found = 1;
            }
        }
        // if all are NaN, return 0
        Array *res = create_scalar_array(&min_idx, INT64);
        return res;
    }

    // along specified axis
    int axis_len = a->shape[axis];
    if (axis_len == 0) {
        // empty axis, return empty array
        int out_ndim = ndim - 1;
        int *out_shape = malloc(out_ndim * sizeof(int));
        int idx = 0;
        for (int i = 0; i < ndim; ++i) if (i != axis) out_shape[idx++] = a->shape[i];
        Array *res = create_array(out_shape, out_ndim, INT64);
        free(out_shape);
        return res;
    }

    // output shape
    int out_ndim = ndim - 1;
    int *out_shape = malloc(out_ndim * sizeof(int));
    int od = 0;
    for (int i = 0; i < ndim; ++i) if (i != axis) out_shape[od++] = a->shape[i];
    Array *result = create_array(out_shape, out_ndim, INT64);
    free(out_shape);
    if (!result) return NULL;

    // stride
    int *strides = malloc(ndim * sizeof(int));
    compute_strides(a->shape, ndim, strides);
    size_t elem_sz = dtype_size(a->dtype);
    char *data = (char*)a->data;
    int64_t *res_data = (int64_t*)result->data;

    // outer ndim combo
    int outer_ndim = ndim - 1;
    int *outer_dims = malloc(outer_ndim * sizeof(int));
    int *outer_shape = malloc(outer_ndim * sizeof(int));
    int *outer_indices = calloc(outer_ndim, sizeof(int));
    if (!outer_dims || !outer_shape || !outer_indices) {
        free(strides); free(outer_dims); free(outer_shape); free(outer_indices);
        free_array(result);
        return NULL;
    }
    od = 0;
    for (int i = 0; i < ndim; ++i) if (i != axis) outer_dims[od++] = i;
    for (int i = 0; i < outer_ndim; ++i) outer_shape[i] = a->shape[outer_dims[i]];
    int num_slices = 1;
    for (int i = 0; i < outer_ndim; ++i) num_slices *= outer_shape[i];

    for (int slice = 0; slice < num_slices; ++slice) {
        // base offset
        int base = 0;
        for (int j = 0; j < outer_ndim; ++j) {
            base += outer_indices[j] * strides[outer_dims[j]];
        }
        int64_t min_idx = -1;
        double min_val = INFINITY;
        int found = 0;
        for (int k = 0; k < axis_len; ++k) {
            void *ptr = data + (base + k * strides[axis]) * elem_sz;
            if (is_nan(ptr, a->dtype)) continue;
            double val = get_value(ptr, a->dtype);
            if (!found || val < min_val) {
                min_val = val;
                min_idx = k;
                found = 1;
            }
        }
        // if all are NaN, set index to 0
        if (!found) min_idx = 0;
        // write result
        int out_off = 0;
        for (int j = 0; j < outer_ndim; ++j) {
            out_off += outer_indices[j] * result->shape[j];
        }
        res_data[out_off] = min_idx;
        // update outer_indices
        int carry = 1;
        for (int j = outer_ndim - 1; j >= 0 && carry; --j) {
            outer_indices[j]++;
            if (outer_indices[j] >= outer_shape[j]) {
                outer_indices[j] = 0;
                carry = 1;
            } else {
                carry = 0;
            }
        }
    }

    free(strides);
    free(outer_dims);
    free(outer_shape);
    free(outer_indices);
    return result;
}