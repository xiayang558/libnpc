#include "array.h"


// ====================== argpartition main function ======================
Array* argpartition(Array *arr, int kth, int axis) {
    if (!check_null(arr, "argpartition")) return NULL;

    // Check data type (complex not yet supported)
    if (arr->dtype == COMPLEX64 || arr->dtype == COMPLEX128 || arr->dtype == COMPLEX256) {
        utils_error_log("Error", "argpartition", "Complex types not supported");
        return NULL;
    }

    // Handle default axis
    if (axis == -1) axis = arr->ndim - 1;
    if (axis < 0 || axis >= arr->ndim) {
        utils_error_log("Error", "argpartition", "Invalid axis %d for array with %d dimensions",
                        axis, arr->ndim);
        return NULL;
    }

    int axis_len = arr->shape[axis];
    // Handle negative kth
    if (kth < 0) kth += axis_len;
    if (kth < 0 || kth >= axis_len) {
        utils_error_log("Error", "argpartition", "kth out of bounds (0 <= kth < %d)", axis_len);
        return NULL;
    }

    // Create result array, same shape as input, type is INT64
    Array *result = create_array(arr->shape, arr->ndim, INT64);
    if (result == NULL) {
        utils_error_log("Error", "argpartition", "Failed to create result array");
        return NULL;
    }

    // Compute input and output byte strides
    size_t *in_strides = malloc(arr->ndim * sizeof(size_t));
    size_t *out_strides = malloc(arr->ndim * sizeof(size_t));
    if (in_strides == NULL || out_strides == NULL) {
        free(in_strides); free(out_strides);
        free_array(result);
        utils_error_log("Error", "argpartition", "Memory allocation failed");
        return NULL;
    }
    compute_byte_strides(arr->shape, arr->ndim, dtype_size(arr->dtype), in_strides);
    compute_byte_strides(arr->shape, arr->ndim, sizeof(int64_t), out_strides);

    // Prepare for traversing all dimensions except the axis
    int outer_ndim = arr->ndim - 1;
    int *outer_dims = malloc(outer_ndim * sizeof(int));   // Store which dimensions are outer
    int *outer_shape = malloc(outer_ndim * sizeof(int));
    int *outer_indices = calloc(outer_ndim, sizeof(int));
    if (outer_dims == NULL || outer_shape == NULL || outer_indices == NULL) {
        free(in_strides); free(out_strides);
        free(outer_dims); free(outer_shape); free(outer_indices);
        free_array(result);
        utils_error_log("Error", "argpartition", "Memory allocation failed");
        return NULL;
    }

    int idx = 0;
    for (int i = 0; i < arr->ndim; i++) {
        if (i != axis) {
            outer_dims[idx] = i;
            outer_shape[idx] = arr->shape[i];
            idx++;
        }
    }

    // Total number of slices
    int num_slices = 1;
    for (int i = 0; i < outer_ndim; i++) num_slices *= outer_shape[i];

    char *in_base = (char*)arr->data;
    char *out_base = (char*)result->data;
    size_t in_axis_stride = in_strides[axis];
    size_t out_axis_stride = out_strides[axis];

    // Temporary index array
    int64_t *temp_indices = malloc(axis_len * sizeof(int64_t));
    if (temp_indices == NULL) {
        free(in_strides); free(out_strides);
        free(outer_dims); free(outer_shape); free(outer_indices);
        free_array(result);
        utils_error_log("Error", "argpartition", "Memory allocation failed");
        return NULL;
    }

    // Traverse all slices
    for (int s = 0; s < num_slices; s++) {
        // Compute the start offset of the current slice in both input and output
        size_t in_offset = 0;
        size_t out_offset = 0;
        for (int j = 0; j < outer_ndim; j++) {
            int dim = outer_dims[j];
            in_offset += outer_indices[j] * in_strides[dim];
            out_offset += outer_indices[j] * out_strides[dim];
        }

        // Initialize temporary index array
        for (int i = 0; i < axis_len; i++) {
            temp_indices[i] = i;
        }

        // Perform partial sort on the current slice indices
        quickselect(temp_indices, 0, axis_len - 1, kth,
                    in_base + in_offset, in_axis_stride, arr->dtype);

        // Write partially sorted indices back to the result slice
        char *out_slice = out_base + out_offset;
        for (int i = 0; i < axis_len; i++) {
            *(int64_t*)(out_slice + i * out_axis_stride) = temp_indices[i];
        }

        // Update outer_indices to move to the next slice
        int carry = 1;
        for (int j = outer_ndim - 1; j >= 0 && carry; j--) {
            outer_indices[j]++;
            if (outer_indices[j] >= outer_shape[j]) {
                outer_indices[j] = 0;
                carry = 1;
            } else {
                carry = 0;
            }
        }
        if (carry) break; // All slices traversed
    }

    // Cleanup
    free(in_strides);
    free(out_strides);
    free(outer_dims);
    free(outer_shape);
    free(outer_indices);
    free(temp_indices);

    return result;
}