#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "array.h"

Array* broadcast_to(Array *arr, int *shape, int ndim) {
    if (arr == NULL || shape == NULL || ndim <= 0) {
        fprintf(stderr, "broadcast_to: Invalid arguments\n");
        return NULL;
    }
    int offset = ndim - arr->ndim;
    for (int i = 0; i < arr->ndim; i++) {
        int target_dim = shape[offset + i];
        int arr_dim = arr->shape[i];
        if (arr_dim != 1 && arr_dim != target_dim) {
            fprintf(stderr, "broadcast_to: Cannot broadcast array shape to target\n");
            return NULL;
        }
    }
    Array *result = create_array(shape, ndim, arr->dtype);
    if (result == NULL) {
        fprintf(stderr, "broadcast_to: Failed to create result array\n");
        return NULL;
    }
    int *arr_strides = malloc(arr->ndim * sizeof(int));
    if (arr_strides == NULL) {
        free_array(result);
        fprintf(stderr, "broadcast_to: Memory allocation failed\n");
        return NULL;
    }
    compute_elem_strides(arr->shape, arr->ndim, arr_strides);
    size_t elem_size = dtype_size(arr->dtype);
    char *src = (char*)arr->data;
    char *dst = (char*)result->data;
    int *indices = malloc(ndim * sizeof(int));
    if (indices == NULL) {
        free(arr_strides);
        free_array(result);
        fprintf(stderr, "broadcast_to: Memory allocation failed\n");
        return NULL;
    }
    for (int flat = 0; flat < result->size; flat++) {
        int temp = flat;
        for (int i = ndim - 1; i >= 0; i--) {
            indices[i] = temp % result->shape[i];
            temp /= result->shape[i];
        }
        int src_flat = 0;
        if (arr->ndim == 0) {
            src_flat = 0;
        } else {
            for (int i = 0; i < arr->ndim; i++) {
                int res_idx = offset + i;
                int dim_size = arr->shape[i];
                int coord = (dim_size == 1) ? 0 : indices[res_idx];
                src_flat += coord * arr_strides[i];
            }
        }
        memcpy(dst + flat * elem_size, src + src_flat * elem_size, elem_size);
    }
    free(arr_strides);
    free(indices);
    return result;
}
