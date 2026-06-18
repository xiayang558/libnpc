// argmin main function
#include "array.h"


Array* argmin(Array *arr, int axis, int keepdims) {
    if (arr == NULL) {
        fprintf(stderr, "[Error] argmin: NULL array argument\n");
        return NULL;
    }
    if (axis < -1 || axis >= arr->ndim) {
        fprintf(stderr, "[Error] argmin: Invalid axis %d for array with %d dimensions\n",
                axis, arr->ndim);
        return NULL;
    }

    // Handle axis = -1 (all elements)
    if (axis == -1) {
        size_t elem_size = dtype_size(arr->dtype);
        char *data = (char*)arr->data;
        int min_idx = 0;
        if (arr->size == 0) {
            fprintf(stderr, "[Error] argmin: empty array\n");
            return NULL;
        }
        for (int i = 1; i < arr->size; i++) {
            void *p_current = data + i * elem_size;
            void *p_min = data + min_idx * elem_size;
            if (compare_values(p_current, p_min, arr->dtype) < 0) {
                min_idx = i;
            }
        }

        // Create result scalar array (0-d)
        int8_t scalar_data = 0;
        Array *result = create_scalar_array(&scalar_data, INT64);
        if (result == NULL) {
            fprintf(stderr, "[Error] argmin: Failed to create result array\n");
            return NULL;
        }
        *(int64_t*)result->data = min_idx;
        return result;
    }

    // Handle specified axis
    int out_ndim = keepdims ? arr->ndim : arr->ndim - 1;
    int *out_shape = malloc(out_ndim * sizeof(int));
    if (out_shape == NULL) {
        fprintf(stderr, "[Error] argmin: Memory allocation failed\n");
        return NULL;
    }
    int out_idx = 0;
    for (int i = 0; i < arr->ndim; i++) {
        if (i == axis) {
            if (keepdims) out_shape[out_idx++] = 1;
        } else {
            out_shape[out_idx++] = arr->shape[i];
        }
    }

    Array *result = create_array(out_shape, out_ndim, INT64);
    free(out_shape);
    if (result == NULL) {
        fprintf(stderr, "[Error] argmin: Failed to create result array\n");
        return NULL;
    }

    // Compute strides (in element units)
    int *arr_strides = malloc(arr->ndim * sizeof(int));
    int *res_strides = malloc(result->ndim * sizeof(int));
    if (arr_strides == NULL || res_strides == NULL) {
        free(arr_strides); free(res_strides);
        free_array(result);
        fprintf(stderr, "[Error] argmin: Memory allocation failed\n");
        return NULL;
    }
    compute_strides(arr->shape, arr->ndim, arr_strides);
    compute_strides(result->shape, result->ndim, res_strides);

    // Allocate result index array
    int *res_indices = calloc(result->ndim, sizeof(int));
    if (res_indices == NULL) {
        free(arr_strides); free(res_strides);
        free_array(result);
        fprintf(stderr, "[Error] argmin: Memory allocation failed\n");
        return NULL;
    }

    size_t elem_size = dtype_size(arr->dtype);
    char *arr_data = (char*)arr->data;
    int64_t *res_data = (int64_t*)result->data;

    // Traverse all output elements
    while (1) {
        // Compute the base offset of the current slice in arr (exclude the axis dimension)
        int base_offset = 0;
        int out_dim_idx = 0;
        for (int i = 0; i < arr->ndim; i++) {
            if (i == axis) continue;
            base_offset += res_indices[out_dim_idx++] * arr_strides[i];
        }

        // Find min index along axis
        int min_offset = 0;  // Initialize to first element
        for (int k = 1; k < arr->shape[axis]; k++) {
            void *p_current = arr_data + (base_offset + k * arr_strides[axis]) * elem_size;
            void *p_min = arr_data + (base_offset + min_offset * arr_strides[axis]) * elem_size;
            if (compare_values(p_current, p_min, arr->dtype) < 0) {
                min_offset = k;
            }
        }

        // Compute flat index in output
        int res_flat = 0;
        for (int i = 0; i < result->ndim; i++) {
            res_flat += res_indices[i] * res_strides[i];
        }
        res_data[res_flat] = min_offset;

        // Update res_indices to move to the next slice
        int carry = 1;
        for (int i = result->ndim - 1; i >= 0 && carry; i--) {
            res_indices[i]++;
            if (res_indices[i] >= result->shape[i]) {
                res_indices[i] = 0;
                carry = 1;
            } else {
                carry = 0;
            }
        }
        if (carry) break; // Traversal complete
    }

    free(arr_strides);
    free(res_strides);
    free(res_indices);

    return result;
}