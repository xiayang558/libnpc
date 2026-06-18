#include "def.h"
#include "array.h"

Array* argwhere(Array *arr) {
    if (!check_null(arr, "argwhere")) return NULL;
    size_t elem_size = dtype_size(arr->dtype);
    char *data = (char*)arr->data;
    int nonzero_count = 0;
    for (int i = 0; i < arr->size; i++) {
        if (is_nonzero(data + i * elem_size, arr->dtype)) nonzero_count++;
    }
    int result_shape[2] = { nonzero_count, arr->ndim };
    Array *result = create_array(result_shape, 2, INT64);
    if (result == NULL) { utils_error_log("Error", "argwhere", "Failed to create result array"); return NULL; }
    if (nonzero_count == 0) return result;
    int *strides = malloc(arr->ndim * sizeof(int));
    if (strides == NULL) { free_array(result); utils_error_log("Error", "argwhere", "Memory allocation failed"); return NULL; }
    strides[arr->ndim - 1] = 1;
    for (int i = arr->ndim - 2; i >= 0; i--) strides[i] = strides[i + 1] * arr->shape[i + 1];
    int64_t *res_data = (int64_t*)result->data;
    int row = 0;
    for (int flat = 0; flat < arr->size; flat++) {
        if (is_nonzero(data + flat * elem_size, arr->dtype)) {
            int remaining = flat;
            for (int dim = 0; dim < arr->ndim; dim++) {
                int coord = remaining / strides[dim];
                remaining %= strides[dim];
                res_data[row * arr->ndim + dim] = coord;
            }
            row++;
        }
    }
    free(strides);
    return result;
}
