#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "array.h"
Array* take_along_axis(Array *arr, Array *indices, int axis) {
    if (!check_null(arr, "take_along_axis") || !check_null(indices, "take_along_axis"))
        return NULL;

    // check that indices dtype must be integer
    DataType idx_dtype = indices->dtype;
    if (!(idx_dtype == INT8 || idx_dtype == INT16 || idx_dtype == INT32 || idx_dtype == INT64 ||
          idx_dtype == UINT8 || idx_dtype == UINT16 || idx_dtype == UINT32 || idx_dtype == UINT64)) {
        utils_error_log("Error", "take_along_axis", "indices must be integer type");
        return NULL;
    }

    // handle axis
    if (axis < 0) axis += arr->ndim;
    if (axis < 0 || axis >= arr->ndim) {
        utils_error_log("Error", "take_along_axis", "Invalid axis %d for array with %d dimensions",
                        axis, arr->ndim);
        return NULL;
    }

    // check ndim: arr and indices must have the same number of dimensions
    if (arr->ndim != indices->ndim) {
        utils_error_log("Error", "take_along_axis",
                        "arr and indices must have the same number of dimensions (arr ndim: %d, indices ndim: %d)",
                        arr->ndim, indices->ndim);
        return NULL;
    }

    // check that all dimensions except axis have the same size
    for (int i = 0; i < arr->ndim; i++) {
        if (i == axis) continue;
        if (arr->shape[i] != indices->shape[i]) {
            utils_error_log("Error", "take_along_axis",
                            "Shape mismatch at dimension %d: arr shape[%d]=%d, indices shape[%d]=%d",
                            i, i, arr->shape[i], i, indices->shape[i]);
            return NULL;
        }
    }

    // create result array, same shape as indices, same dtype as arr
    Array *result = create_array(indices->shape, indices->ndim, arr->dtype);
    if (result == NULL) {
        utils_error_log("Error", "take_along_axis", "Failed to create result array");
        return NULL;
    }

    // compute byte strides for each array (used for fast offset calculation)
    size_t *arr_strides = malloc(arr->ndim * sizeof(size_t));
    size_t *idx_strides = malloc(indices->ndim * sizeof(size_t));
    size_t *res_strides = malloc(result->ndim * sizeof(size_t));
    if (arr_strides == NULL || idx_strides == NULL || res_strides == NULL) {
        free(arr_strides); free(idx_strides); free(res_strides);
        free_array(result);
        utils_error_log("Error", "take_along_axis", "Memory allocation failed");
        return NULL;
    }
    compute_byte_strides(arr->shape, arr->ndim, dtype_size(arr->dtype), arr_strides);
    compute_byte_strides(indices->shape, indices->ndim, dtype_size(indices->dtype), idx_strides);
    compute_byte_strides(result->shape, result->ndim, dtype_size(result->dtype), res_strides);

    // iterate over each element of the result array
    char *arr_base = (char*)arr->data;
    char *idx_base = (char*)indices->data;
    char *res_base = (char*)result->data;

    int *res_indices = calloc(result->ndim, sizeof(int));
    if (res_indices == NULL) {
        free(arr_strides); free(idx_strides); free(res_strides);
        free_array(result);
        utils_error_log("Error", "take_along_axis", "Memory allocation failed");
        return NULL;
    }

    int total_elements = result->size;
    for (int flat = 0; flat < total_elements; flat++) {
        // convert flat index to multi-dimensional index (row-major)
        int temp = flat;
        for (int i = result->ndim - 1; i >= 0; i--) {
            res_indices[i] = temp % result->shape[i];
            temp /= result->shape[i];
        }

        // compute offset into indices
        size_t idx_offset = 0;
        for (int i = 0; i < result->ndim; i++) {
            idx_offset += res_indices[i] * idx_strides[i];
        }

        // get the index value
        int64_t idx_val;
        switch (indices->dtype) {
            case INT8:   idx_val = *(int8_t*)(idx_base + idx_offset); break;
            case INT16:  idx_val = *(int16_t*)(idx_base + idx_offset); break;
            case INT32:  idx_val = *(int32_t*)(idx_base + idx_offset); break;
            case INT64:  idx_val = *(int64_t*)(idx_base + idx_offset); break;
            case UINT8:  idx_val = *(uint8_t*)(idx_base + idx_offset); break;
            case UINT16: idx_val = *(uint16_t*)(idx_base + idx_offset); break;
            case UINT32: idx_val = *(uint32_t*)(idx_base + idx_offset); break;
            case UINT64: idx_val = *(uint64_t*)(idx_base + idx_offset); break;
            default: idx_val = 0;
        }

        // check if index is within bounds
        if (idx_val < 0 || idx_val >= arr->shape[axis]) {
            utils_error_log("Error", "take_along_axis",
                            "Index %lld out of bounds for axis %d (size %d)",
                            (long long)idx_val, axis, arr->shape[axis]);
            free(res_indices);
            free(arr_strides); free(idx_strides); free(res_strides);
            free_array(result);
            return NULL;
        }

        // construct arr multi-dimensional index
        // arr index is composed of res_indices and idx_val: use idx_val for the axis dimension, other dimensions match res_indices
        int *arr_indices = malloc(arr->ndim * sizeof(int));
        if (arr_indices == NULL) {
            free(res_indices);
            free(arr_strides); free(idx_strides); free(res_strides);
            free_array(result);
            utils_error_log("Error", "take_along_axis", "Memory allocation failed");
            return NULL;
        }
        for (int i = 0; i < arr->ndim; i++) {
            if (i == axis) {
                arr_indices[i] = idx_val;
            } else {
                arr_indices[i] = res_indices[i];
            }
        }

        // compute offset into arr
        size_t arr_offset = 0;
        for (int i = 0; i < arr->ndim; i++) {
            arr_offset += arr_indices[i] * arr_strides[i];
        }
        free(arr_indices);

        // compute offset into result
        size_t res_offset = 0;
        for (int i = 0; i < result->ndim; i++) {
            res_offset += res_indices[i] * res_strides[i];
        }

        // copy value
        memcpy(res_base + res_offset, arr_base + arr_offset, dtype_size(arr->dtype));
    }

    free(res_indices);
    free(arr_strides);
    free(idx_strides);
    free(res_strides);

    return result;
}


// ====================== Clean up macro definitions ======================

#undef NP_UTILS_ERROR_LOG
#undef NP_UTILS_SAFE_FREE_ARRAY
#undef NP_UTILS_SAFE_FREE

// compare two elements, return a < b
