#include "array.h"


Array* choose(Array *arr, int num_choices, Array **choices, int mode) {
    // Parameter check
    if (!check_null(arr, "choose")) return NULL;
    if (num_choices <= 0) {
        utils_error_log("Error", "choose", "num_choices must be positive");
        return NULL;
    }
    if (choices == NULL) {
        utils_error_log("Error", "choose", "choices array is NULL");
        return NULL;
    }
    for (int i = 0; i < num_choices; i++) {
        if (!check_null(choices[i], "choose")) return NULL;
    }

    // Check if arr is an integer type
    DataType arr_dtype = arr->dtype;
    if (!(arr_dtype == INT8 || arr_dtype == INT16 || arr_dtype == INT32 || arr_dtype == INT64 ||
          arr_dtype == UINT8 || arr_dtype == UINT16 || arr_dtype == UINT32 || arr_dtype == UINT64)) {
        utils_error_log("Error", "choose", "arr must be integer type");
        return NULL;
    }

    // Check if choice data types are consistent (simplified version)
    DataType choices_dtype = choices[0]->dtype;
    for (int i = 1; i < num_choices; i++) {
        if (choices[i]->dtype != choices_dtype) {
            utils_error_log("Error", "choose", "All choices must have the same data type");
            return NULL;
        }
    }

    // Build array list for broadcast
    int num_arrays = 1 + num_choices;
    Array **arrays = (Array**)malloc(num_arrays * sizeof(Array*));
    if (arrays == NULL) {
        utils_error_log("Error", "choose", "Memory allocation failed");
        return NULL;
    }
    arrays[0] = arr;
    for (int i = 0; i < num_choices; i++) {
        arrays[1 + i] = choices[i];
    }

    // Compute shape after broadcast
    int res_ndim;
    int *res_shape = broadcast_shapes(num_arrays, arrays, &res_ndim);
    free(arrays);
    if (res_shape == NULL) {
        return NULL;
    }

    // Create result array
    Array *result = create_array(res_shape, res_ndim, choices_dtype);
    free(res_shape);
    if (result == NULL) {
        utils_error_log("Error", "choose", "Failed to create result array");
        return NULL;
    }

    // Compute result array strides (contiguous)
    int *res_strides = (int*)malloc(res_ndim * sizeof(int));
    if (res_strides == NULL) {
        utils_error_log("Error", "choose", "Memory allocation failed");
        free_array(result);
        return NULL;
    }
    compute_strides(result->shape, result->ndim, res_strides);

    // Pre-compute arr strides
    int *arr_strides = NULL;
    if (arr->ndim > 0) {
        arr_strides = (int*)malloc(arr->ndim * sizeof(int));
        compute_strides(arr->shape, arr->ndim, arr_strides);
    }

    // Pre-compute per-choice strides
    int **choices_strides = (int**)malloc(num_choices * sizeof(int*));
    for (int i = 0; i < num_choices; i++) {
        if (choices[i]->ndim > 0) {
            choices_strides[i] = (int*)malloc(choices[i]->ndim * sizeof(int));
            compute_strides(choices[i]->shape, choices[i]->ndim, choices_strides[i]);
        } else {
            choices_strides[i] = NULL;
        }
    }

    // Allocate index array
    int *indices = (int*)malloc(res_ndim * sizeof(int));
    if (indices == NULL) {
        utils_error_log("Error", "choose", "Memory allocation failed");
        free(arr_strides);
        for (int i = 0; i < num_choices; i++) free(choices_strides[i]);
        free(choices_strides);
        free(res_strides);
        free_array(result);
        return NULL;
    }

    // Traverse all result elements
    for (int flat_idx = 0; flat_idx < result->size; flat_idx++) {
        // Convert flat_idx to multi-dimensional index
        int temp = flat_idx;
        for (int i = res_ndim - 1; i >= 0; i--) {
            indices[i] = temp % result->shape[i];
            temp /= result->shape[i];
        }

        // Get index value from arr
        int arr_idx_flat = (arr->ndim == 0) ? 0 : get_broadcast_index(indices, res_ndim, arr, arr_strides);
        long long idx_val = 0;
        switch (arr->dtype) {
            case INT8:   idx_val = *(int8_t*)((char*)arr->data + arr_idx_flat * dtype_size(arr->dtype)); break;
            case INT16:  idx_val = *(int16_t*)((char*)arr->data + arr_idx_flat * dtype_size(arr->dtype)); break;
            case INT32:  idx_val = *(int32_t*)((char*)arr->data + arr_idx_flat * dtype_size(arr->dtype)); break;
            case INT64:  idx_val = *(int64_t*)((char*)arr->data + arr_idx_flat * dtype_size(arr->dtype)); break;
            case UINT8:  idx_val = *(uint8_t*)((char*)arr->data + arr_idx_flat * dtype_size(arr->dtype)); break;
            case UINT16: idx_val = *(uint16_t*)((char*)arr->data + arr_idx_flat * dtype_size(arr->dtype)); break;
            case UINT32: idx_val = *(uint32_t*)((char*)arr->data + arr_idx_flat * dtype_size(arr->dtype)); break;
            case UINT64: idx_val = *(uint64_t*)((char*)arr->data + arr_idx_flat * dtype_size(arr->dtype)); break;
            default: break;
        }

        // Handle index out of bounds
        int choice_idx;
        if (mode == CHOOSE_RAISE) {
            if (idx_val < 0 || idx_val >= num_choices) {
                utils_error_log("Error", "choose", "Index out of range: %lld (num_choices=%d)", idx_val, num_choices);
                free(indices);
                free(arr_strides);
                for (int i = 0; i < num_choices; i++) free(choices_strides[i]);
                free(choices_strides);
                free(res_strides);
                free_array(result);
                return NULL;
            }
            choice_idx = (int)idx_val;
        } else if (mode == CHOOSE_WRAP) {
            if (idx_val < 0) {
                idx_val = num_choices - ((-idx_val) % num_choices);
            }
            choice_idx = idx_val % num_choices;
        } else if (mode == CHOOSE_CLIP) {
            if (idx_val < 0) choice_idx = 0;
            else if (idx_val >= num_choices) choice_idx = num_choices - 1;
            else choice_idx = (int)idx_val;
        } else {
            utils_error_log("Error", "choose", "Invalid mode");
            free(indices);
            free(arr_strides);
            for (int i = 0; i < num_choices; i++) free(choices_strides[i]);
            free(choices_strides);
            free(res_strides);
            free_array(result);
            return NULL;
        }

        // Get value from the corresponding choice
        Array *choice_arr = choices[choice_idx];
        int choice_flat = (choice_arr->ndim == 0) ? 0 : get_broadcast_index(indices, res_ndim, choice_arr, choices_strides[choice_idx]);
        size_t elem_size = dtype_size(choices_dtype);
        memcpy((char*)result->data + flat_idx * elem_size,
               (char*)choice_arr->data + choice_flat * elem_size,
               elem_size);
    }

    // Free temporary resources
    free(indices);
    free(arr_strides);
    for (int i = 0; i < num_choices; i++) free(choices_strides[i]);
    free(choices_strides);
    free(res_strides);

    return result;
}