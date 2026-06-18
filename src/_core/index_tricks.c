#include "array.h"
#include "def.h"
// #include "shape.h"


void fill_diagonal(Array *arr, void *val) {
    if (arr == NULL) {
        fprintf(stderr, "fill_diagonal: NULL array argument\n");
        return;
    }
    if (arr->ndim < 2) {
        fprintf(stderr, "fill_diagonal: array must have at least 2 dimensions\n");
        return;
    }
    int last_dim = arr->ndim - 1;
    int second_last_dim = arr->ndim - 2;
    int rows = arr->shape[second_last_dim];
    int cols = arr->shape[last_dim];
    int diag_len = (rows < cols) ? rows : cols;

    size_t elem_size = dtype_size(arr->dtype);
    char *data = (char*)arr->data;

    // compute strides: for the last two axes, strides are last dim stride and second-to-last dim stride
    // since we can't traverse generically, directly use coordinate to compute offset
    // simplified version: directly loop i from 0 to diag_len-1, then set data[i * (cols+1)] not general (because ndim can be greater than 2)
    // correct approach: we need to traverse preceding ndim combinations, set diagonal in each slice.

    // for n-d array, the last two dims form a matrix, preceding ndim combinations correspond to one matrix slice.
    int outer_ndim = arr->ndim - 2;
    int *outer_shape = (outer_ndim > 0) ? malloc(outer_ndim * sizeof(int)) : NULL;
    if (outer_ndim > 0 && outer_shape == NULL) {
        fprintf(stderr, "fill_diagonal: memory allocation failed\n");
        return;
    }
    for (int i = 0; i < outer_ndim; i++) outer_shape[i] = arr->shape[i];

    int outer_size = 1;
    for (int i = 0; i < outer_ndim; i++) outer_size *= outer_shape[i];

    // compute total stride (in units of elements) for locating each slice start position
    int *strides = malloc(arr->ndim * sizeof(int));
    if (!strides) {
        free(outer_shape);
        return;
    }
    compute_strides(arr->shape, arr->ndim, strides);  // element stride, i.e. per-ndim increment of 1 index equals element offset

    // for each outer layer slice
    for (int outer_idx = 0; outer_idx < outer_size; outer_idx++) {
        // compute current slice base offset: based on outer multi-dimensional index
        int base_offset = 0;
        int temp = outer_idx;
        for (int i = outer_ndim - 1; i >= 0; i--) {
            int coord = temp % outer_shape[i];
            temp /= outer_shape[i];
            base_offset += coord * strides[i];
        }

        // fill this slice's main diagonal
        for (int i = 0; i < diag_len; i++) {
            int offset = base_offset + i * strides[second_last_dim] + i * strides[last_dim];
            void *dst = data + offset * elem_size;
            // based on arr type convert val
            switch (arr->dtype) {
                case INT8:   *(int8_t*)dst = *(int8_t*)val; break;
                case INT16:  *(int16_t*)dst = *(int16_t*)val; break;
                case INT32:  *(int32_t*)dst = *(int32_t*)val; break;
                case INT64:  *(int64_t*)dst = *(int64_t*)val; break;
                case UINT8:  *(uint8_t*)dst = *(uint8_t*)val; break;
                case UINT16: *(uint16_t*)dst = *(uint16_t*)val; break;
                case UINT32: *(uint32_t*)dst = *(uint32_t*)val; break;
                case UINT64: *(uint64_t*)dst = *(uint64_t*)val; break;
                case FLOAT32: *(float*)dst = *(float*)val; break;
                case FLOAT64: *(double*)dst = *(double*)val; break;
                case COMPLEX64: *(complex float*)dst = *(complex float*)val; break;
                case COMPLEX128: *(complex double*)dst = *(complex double*)val; break;
                default: break;
            }
        }
    }

    free(outer_shape);
    free(strides);
}

/**
 * ix_  --  construct an open mesh index (like numpy.ix_)
 */
int ix_(Array **arrays, int n, Array ***out) {
    if (!arrays || n < 1 || !out) return -1;

    Array **results = malloc(n * sizeof(Array*));
    if (!results) return -1;

    for (int i = 0; i < n; i++) {
        if (!arrays[i] || arrays[i]->ndim != 1) {
            for (int j = 0; j < i; j++) free_array(results[j]);
            free(results); return -1;
        }
        int *new_shape = calloc(n, sizeof(int));
        new_shape[i] = arrays[i]->size;
        for (int d = 0; d < n; d++)
            if (d != i) new_shape[d] = 1;

        size_t esz = dtype_size(arrays[i]->dtype);
        results[i] = create_array(new_shape, n, arrays[i]->dtype);
        free(new_shape);
        if (!results[i]) {
            for (int j = 0; j < i; j++) free_array(results[j]);
            free(results); return -1;
        }
        memcpy(results[i]->data, arrays[i]->data, arrays[i]->size * esz);
    }
    *out = results;
    return 0;
}