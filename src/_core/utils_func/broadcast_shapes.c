#include <stdlib.h>
#include "array.h"
int* broadcast_shapes(int n, Array **arrays, int *out_ndim) {
    int max_ndim = 0;
    for (int i = 0; i < n; i++) { if (arrays[i]->ndim > max_ndim) max_ndim = arrays[i]->ndim; }
    int *res_shape = (int*)malloc(max_ndim * sizeof(int));
    if (res_shape == NULL) return NULL;
    for (int i = 0; i < max_ndim; i++) res_shape[i] = 1;
    for (int i = 0; i < n; i++) {
        Array *arr = arrays[i];
        int ndim = arr->ndim;
        for (int j = 0; j < ndim; j++) {
            int res_idx = max_ndim - ndim + j;
            int dim_size = arr->shape[j];
            if (dim_size != 1 && res_shape[res_idx] != 1 && res_shape[res_idx] != dim_size) {
                utils_error_log("Error", "broadcast_shapes", "Cannot broadcast: shape[%d]=%d vs %d", res_idx, res_shape[res_idx], dim_size);
                free(res_shape);
                return NULL;
            }
            if (dim_size > res_shape[res_idx]) res_shape[res_idx] = dim_size;
        }
    }
    *out_ndim = max_ndim;
    return res_shape;
}
