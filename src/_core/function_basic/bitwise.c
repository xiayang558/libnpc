#include "array.h"


// Generic binary bitwise operation function
static Array* bitwise_binary_op(Array *arr1, Array *arr2, const char *op_name,
                                 uint64_t (*op_func)(uint64_t arr, uint64_t b)) {
    if (arr1 == NULL || arr2 == NULL) {
        utils_error_log("Error", op_name, "NULL array argument");
        return NULL;
    }
    if (!is_integer(arr1->dtype) || !is_integer(arr2->dtype)) {
        utils_error_log("Error", op_name, "Operands must be integer type");
        return NULL;
    }
    if (arr1->dtype != arr2->dtype) {
        utils_error_log("Error", op_name, "Data types must be the same");
        return NULL;
    }

    // broadcast
    Array *arrays[2] = {arr1, arr2};
    int out_ndim;
    int *out_shape = broadcast_shapes(2, arrays, &out_ndim);
    if (out_shape == NULL) {
        utils_error_log("Error", op_name, "Broadcast failed");
        return NULL;
    }

    Array *result = create_array(out_shape, out_ndim, arr1->dtype);
    free(out_shape);
    if (result == NULL) {
        utils_error_log("Error", op_name, "Failed to create result array");
        return NULL;
    }

    // compute strides
    int *strides1 = malloc(arr1->ndim * sizeof(int));
    int *strides2 = malloc(arr2->ndim * sizeof(int));
    int *res_strides = malloc(result->ndim * sizeof(int));
    if (strides1 == NULL || strides2 == NULL || res_strides == NULL) {
        free(strides1); free(strides2); free(res_strides);
        free_array(result);
        utils_error_log("Error", op_name, "Memory allocation failed");
        return NULL;
    }
    compute_strides(arr1->shape, arr1->ndim, strides1);
    compute_strides(arr2->shape, arr2->ndim, strides2);
    compute_strides(result->shape, result->ndim, res_strides);

    size_t elem_size = dtype_size(arr1->dtype);
    char *data1 = (char*)arr1->data;
    char *data2 = (char*)arr2->data;
    char *res_data = (char*)result->data;

    int *indices = calloc(result->ndim, sizeof(int));
    if (indices == NULL) {
        free(strides1); free(strides2); free(res_strides);
        free_array(result);
        utils_error_log("Error", op_name, "Memory allocation failed");
        return NULL;
    }

    for (int flat = 0; flat < result->size; flat++) {
        // get multi-dimensional index
        int temp = flat;
        for (int i = result->ndim - 1; i >= 0; i--) {
            indices[i] = temp % result->shape[i];
            temp /= result->shape[i];
        }

        int idx1 = get_broadcast_index(indices, result->ndim, arr1, strides1);
        int idx2 = get_broadcast_index(indices, result->ndim, arr2, strides2);

        void *p1 = data1 + idx1 * elem_size;
        void *p2 = data2 + idx2 * elem_size;
        void *pr = res_data + flat * elem_size;

        // extract integer value based on type (promote uniformly to uint64_t for bitwise operations, then write back)
        uint64_t arr = 0, b = 0;
        switch (arr1->dtype) {
            case BOOL:
            case INT8:   arr = (uint64_t)*(int8_t*)p1; b = (uint64_t)*(int8_t*)p2; break;
            case INT16:  arr = (uint64_t)*(int16_t*)p1; b = (uint64_t)*(int16_t*)p2; break;
            case INT32:  arr = (uint64_t)*(int32_t*)p1; b = (uint64_t)*(int32_t*)p2; break;
            case INT64:  arr = (uint64_t)*(int64_t*)p1; b = (uint64_t)*(int64_t*)p2; break;
            case UINT8:  arr = *(uint8_t*)p1; b = *(uint8_t*)p2; break;
            case UINT16: arr = *(uint16_t*)p1; b = *(uint16_t*)p2; break;
            case UINT32: arr = *(uint32_t*)p1; b = *(uint32_t*)p2; break;
            case UINT64: arr = *(uint64_t*)p1; b = *(uint64_t*)p2; break;
            default: break;
        }

        uint64_t r = op_func(arr, b);

        // write back to result
        switch (arr1->dtype) {
            case BOOL:
            case INT8:   *(int8_t*)pr = (int8_t)r; break;
            case INT16:  *(int16_t*)pr = (int16_t)r; break;
            case INT32:  *(int32_t*)pr = (int32_t)r; break;
            case INT64:  *(int64_t*)pr = (int64_t)r; break;
            case UINT8:  *(uint8_t*)pr = (uint8_t)r; break;
            case UINT16: *(uint16_t*)pr = (uint16_t)r; break;
            case UINT32: *(uint32_t*)pr = (uint32_t)r; break;
            case UINT64: *(uint64_t*)pr = r; break;
            default: break;
        }
    }

    free(strides1); free(strides2); free(res_strides); free(indices);
    return result;
}


// bitwise AND operation function
static uint64_t bitwise_and_op(uint64_t arr, uint64_t b) { return arr & b; }

Array* bitwise_and(Array *arr1, Array *arr2) {
    return bitwise_binary_op(arr1, arr2, "bitwise_and", bitwise_and_op);
}

// bitwise OR operation function
static uint64_t bitwise_or_op(uint64_t arr, uint64_t b) { return arr | b; }

Array* bitwise_or(Array *arr1, Array *arr2) {
    return bitwise_binary_op(arr1, arr2, "bitwise_or", bitwise_or_op);
}

// bitwise XOR operation function
static uint64_t bitwise_xor_op(uint64_t arr, uint64_t b) { return arr ^ b; }

Array* bitwise_xor(Array *arr1, Array *arr2) {
    return bitwise_binary_op(arr1, arr2, "bitwise_xor", bitwise_xor_op);
}

// bitwise NOT (unary operation)
Array* bitwise_not(Array *arr) {
    if (arr == NULL) {
        utils_error_log("Error", "bitwise_not", "NULL array argument");
        return NULL;
    }
    if (!is_integer(arr->dtype)) {
        utils_error_log("Error", "bitwise_not", "Operand must be integer type");
        return NULL;
    }

    // output has the same shape as input
    Array *result = create_array(arr->shape, arr->ndim, arr->dtype);
    if (result == NULL) {
        utils_error_log("Error", "bitwise_not", "Failed to create result array");
        return NULL;
    }

    size_t elem_size = dtype_size(arr->dtype);
    char *src = (char*)arr->data;
    char *dst = (char*)result->data;

    for (int i = 0; i < arr->size; i++) {
        void *p = src + i * elem_size;
        void *pr = dst + i * elem_size;

        uint64_t val = 0;
        switch (arr->dtype) {
            case BOOL:
            case INT8:   val = (uint64_t)*(int8_t*)p; break;
            case INT16:  val = (uint64_t)*(int16_t*)p; break;
            case INT32:  val = (uint64_t)*(int32_t*)p; break;
            case INT64:  val = (uint64_t)*(int64_t*)p; break;
            case UINT8:  val = *(uint8_t*)p; break;
            case UINT16: val = *(uint16_t*)p; break;
            case UINT32: val = *(uint32_t*)p; break;
            case UINT64: val = *(uint64_t*)p; break;
            default: break;
        }

        uint64_t r = ~val;

        // truncate based on type
        switch (arr->dtype) {
            case BOOL:
            case INT8:   *(int8_t*)pr = (int8_t)r; break;
            case INT16:  *(int16_t*)pr = (int16_t)r; break;
            case INT32:  *(int32_t*)pr = (int32_t)r; break;
            case INT64:  *(int64_t*)pr = (int64_t)r; break;
            case UINT8:  *(uint8_t*)pr = (uint8_t)r; break;
            case UINT16: *(uint16_t*)pr = (uint16_t)r; break;
            case UINT32: *(uint32_t*)pr = (uint32_t)r; break;
            case UINT64: *(uint64_t*)pr = r; break;
            default: break;
        }
    }

    return result;
}