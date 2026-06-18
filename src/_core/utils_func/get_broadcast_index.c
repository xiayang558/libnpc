#include "array.h"
int get_broadcast_index(const int *res_indices, int res_ndim,
                               Array *arr, const int *arr_strides) {
    if (arr->ndim == 0) return 0;  // scalar
    int ndim = arr->ndim;
    int offset = 0;
    int dim_offset = res_ndim - ndim;  // effective ndim index in result
    for (int i = 0; i < ndim; i++) {
        int res_idx = dim_offset + i;
        int dim_size = arr->shape[i];
        int index = (dim_size == 1) ? 0 : res_indices[res_idx];
        offset += index * arr_strides[i];
    }
    return offset;
}

// check if element is nonzero (for boolean conversion)
