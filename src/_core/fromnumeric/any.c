#include "array.h"


// any function implementation
Array* any(Array *arr, int axis, int keepdims) {
    if (!check_null(arr, "any")) return NULL;
    if (!check_axis(arr, axis, "any")) return NULL;

    // Handle all-elements case (axis == -1)
    if (axis == -1) {
        size_t elem_size = dtype_size(arr->dtype);
        char *data = (char*)arr->data;
        int found = 0;
        for (int i = 0; i < arr->size; i++) {
            if (is_nonzero(data + i * elem_size, arr->dtype)) {
                found = 1;
                break;
            }
        }

        // Create result scalar array
        int ndim = keepdims ? arr->ndim : 0;
        int *shape = keepdims ? malloc(arr->ndim * sizeof(int)) : NULL;
        if (keepdims) {
            for (int i = 0; i < arr->ndim; i++) shape[i] = 1;
        }
        Array *result = create_array(shape, ndim, BOOL);
        if (shape) free(shape);
        if (result == NULL) {
            utils_error_log("Error", "any", "Failed to create result array");
            return NULL;
        }
        *(uint8_t*)result->data = found ? 1 : 0;
        return result;
    }

    // Compute output shape
    int out_ndim = keepdims ? arr->ndim : arr->ndim - 1;
    int *out_shape = malloc(out_ndim * sizeof(int));
    if (out_shape == NULL) {
        utils_error_log("Error", "any", "Memory allocation failed");
        return NULL;
    }
    int out_idx = 0;
    for (int i = 0; i < arr->ndim; i++) {
        if (i == axis) {
            if (keepdims) out_shape[out_idx++] = 1;
            // Otherwise skip
        } else {
            out_shape[out_idx++] = arr->shape[i];
        }
    }

    // Create result array
    Array *result = create_array(out_shape, out_ndim, BOOL);
    free(out_shape);
    if (result == NULL) {
        utils_error_log("Error", "any", "Failed to create result array");
        return NULL;
    }

    // Compute strides for original array and result array (for indexing)
    int *arr_strides = malloc(arr->ndim * sizeof(int));
    int *res_strides = malloc(result->ndim * sizeof(int));
    if (arr_strides == NULL || res_strides == NULL) {
        free(arr_strides); free(res_strides);
        free_array(result);
        utils_error_log("Error", "any", "Memory allocation failed");
        return NULL;
    }
    compute_strides(arr->shape, arr->ndim, arr_strides);
    compute_strides(result->shape, result->ndim, res_strides);

    // Allocate output index array
    int *res_indices = calloc(result->ndim, sizeof(int));
    if (res_indices == NULL) {
        free(arr_strides); free(res_strides);
        free_array(result);
        utils_error_log("Error", "any", "Memory allocation failed");
        return NULL;
    }

    size_t elem_size = dtype_size(arr->dtype);
    uint8_t *res_data = (uint8_t*)result->data;
    char *arr_data = (char*)arr->data;

    // Traverse each element of the result array
    while (1) {
        // Compute the start offset of the corresponding slice in the original array
        // (dimensions other than axis are determined by res_indices)
        int base_offset = 0;
        int out_idx = 0;
        for (int i = 0; i < arr->ndim; i++) {
            if (i == axis) continue;
            int coord = res_indices[out_idx++];
            base_offset += coord * arr_strides[i];
        }

        // Traverse all elements along axis
        int found = 0;
        for (int k = 0; k < arr->shape[axis]; k++) {
            int offset = base_offset + k * arr_strides[axis];
            if (is_nonzero(arr_data + offset * elem_size, arr->dtype)) {
                found = 1;
                break;
            }
        }

        // Write result
        int res_flat = 0;
        for (int i = 0; i < result->ndim; i++) {
            res_flat += res_indices[i] * res_strides[i];
        }
        res_data[res_flat] = found ? 1 : 0;

        // Update result index (move to next element)
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

    // Cleanup
    free(arr_strides);
    free(res_strides);
    free(res_indices);

    return result;
}