#include "array.h"


Array* append(Array *arr, Array *values, int axis) {
    if (!check_null(arr, "append") || !check_null(values, "append")) {
        return NULL;
    }

    // check dtype compatibility (simplified, should have type promotion in practice)
    if (arr->dtype != values->dtype) {
        utils_error_log("Error", "append",
                        "Data types must be the same (arr: %s, values: %s)",
                        dtype_name(arr->dtype), dtype_name(values->dtype));
        return NULL;
    }

    // handle axis == -1 (flattened mode)
    if (axis == -1) {
        // create 1D result array, length = arr->size + values->size
        int result_shape[1] = { arr->size + values->size };
        DataType res_type = promote_type(arr->dtype, values->dtype);
        Array *result = create_array(result_shape, 1, res_type);
        if (result == NULL) {
            utils_error_log("Error", "append", "Failed to create result array");
            return NULL;
        }

        size_t elem_size = dtype_size(res_type);
        char *dest = (char*)result->data;

        // copy arr data (in contiguous memory order)
        memcpy(dest, arr->data, arr->size * elem_size);
        dest += arr->size * elem_size;

        // copy values data
        memcpy(dest, values->data, values->size * elem_size);

        return result;
    }

    // check axis validity
    if (axis < 0 || axis >= arr->ndim) {
        utils_error_log("Error", "append",
                        "Invalid axis %d for array with %d dimensions",
                        axis, arr->ndim);
        return NULL;
    }

    // check if ndim matches
    if (arr->ndim != values->ndim) {
        utils_error_log("Error", "append",
                        "Array dimensions must match when axis is specified "
                        "(arr ndim: %d, values ndim: %d)",
                        arr->ndim, values->ndim);
        return NULL;
    }

    // check that all dimensions except the concatenation axis match
    for (int i = 0; i < arr->ndim; i++) {
        if (i != axis && arr->shape[i] != values->shape[i]) {
            utils_error_log("Error", "append",
                            "Shape mismatch at dimension %d: arr shape[%d]=%d, values shape[%d]=%d",
                            i, i, arr->shape[i], i, values->shape[i]);
            return NULL;
        }
    }

    // use the existing concatenate function to perform concatenation
    Array *result = concatenate(arr, values, axis);
    if (result == NULL) {
        utils_error_log("Error", "append", "concatenate failed");
        return NULL;
    }

    return result;
}