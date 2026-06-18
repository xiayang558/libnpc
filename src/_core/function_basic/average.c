#include "array.h"


Array* average(Array *arr, Array *weights, int axis, int keepdims) {
    if (!check_null(arr, "average")) return NULL;
    if (!check_axis(arr, axis, "average")) return NULL;

    // if no weights provided, compute ordinary mean
    if (weights == NULL) {
        return mean(arr, axis, keepdims);
    }

    // check weights array not null
    if (!check_null(weights, "average_array")) return NULL;

    // check weights array shape compatibility with arr
    // simplified implementation: require weights shape and arr fully identical, or can broadcast (simplified here, only require identical)
    if (!compare_shapes(arr, weights)) {
        // more complex compatibility check: if axis specified, weights can be a 1-d array of length matching this axis
        // here we first implement simplified version, output warning and attempt per-axis match
        int compatible = 0;
        if (axis >= 0 && weights->ndim == 1 && weights->shape[0] == arr->shape[axis]) {
            compatible = 1;
        } else if (axis == -1 && weights->size == arr->size) {
            // weights flattened and arr element count same, treat as compatible (assume broadcastable)
            compatible = 1;
        }

        if (!compatible) {
            utils_error_log("Error", "average_array", 
                "Weights shape incompatible with array along specified axis");
            return NULL;
        }
        // if compatible, continue; otherwise report error
    }

    // compute weighted sum = sum(arr * weights)
    Array *product = multiply(arr, weights);
    if (product == NULL) {
        utils_error_log("Error", "average_array", "Failed to compute product array");
        return NULL;
    }

    Array *weighted_sum = sum(product, axis, keepdims);
    free_array(product);
    if (weighted_sum == NULL) {
        utils_error_log("Error", "average_array", "Failed to compute weighted sum");
        return NULL;
    }

    // compute sum of weights = sum(weights)
    Array *weights_sum = sum(weights, axis, keepdims);
    if (weights_sum == NULL) {
        free_array(weighted_sum);
        utils_error_log("Error", "average_array", "Failed to compute weights sum");
        return NULL;
    }

    // check if sum of weights is zero (or close to zero)
    // based on weight sum data type perform zero value check
    int zero_weight = 0;
    switch (weights_sum->dtype) {
        case FLOAT32: {
            float *data = (float*)weights_sum->data;
            for (int i = 0; i < weights_sum->size; i++) {
                if (fabsf(data[i]) < 1e-10f) {
                    zero_weight = 1;
                    break;
                }
            }
            break;
        }
        case FLOAT64: {
            double *data = (double*)weights_sum->data;
            for (int i = 0; i < weights_sum->size; i++) {
                if (fabs(data[i]) < 1e-10) {
                    zero_weight = 1;
                    break;
                }
            }
            break;
        }
        case INT8:
        case INT16:
        case INT32:
        case INT64:
        case UINT8:
        case UINT16:
        case UINT32:
        case UINT64: {
            // for integer types, directly check if zero
            // use generic pointer, check element by element, based on type size
            size_t elem_size = dtype_size(weights_sum->dtype);
            char *data = (char*)weights_sum->data;
            for (int i = 0; i < weights_sum->size; i++) {
                void *ptr = data + i * elem_size;
                int is_zero = 0;
                // check based on type
                switch (weights_sum->dtype) {
                    case INT8:   is_zero = *(int8_t*)ptr == 0; break;
                    case INT16:  is_zero = *(int16_t*)ptr == 0; break;
                    case INT32:  is_zero = *(int32_t*)ptr == 0; break;
                    case INT64:  is_zero = *(int64_t*)ptr == 0; break;
                    case UINT8:  is_zero = *(uint8_t*)ptr == 0; break;
                    case UINT16: is_zero = *(uint16_t*)ptr == 0; break;
                    case UINT32: is_zero = *(uint32_t*)ptr == 0; break;
                    case UINT64: is_zero = *(uint64_t*)ptr == 0; break;
                    default: break;
                }
                if (is_zero) {
                    zero_weight = 1;
                    break;
                }
            }
            break;
        }
        case COMPLEX64: {
            complex float *data = (complex float*)weights_sum->data;
            for (int i = 0; i < weights_sum->size; i++) {
                if (cabsf(data[i]) < 1e-10f) {
                    zero_weight = 1;
                    break;
                }
            }
            break;
        }
        case COMPLEX128: {
            complex double *data = (complex double*)weights_sum->data;
            for (int i = 0; i < weights_sum->size; i++) {
                if (cabs(data[i]) < 1e-10) {
                    zero_weight = 1;
                    break;
                }
            }
            break;
        }
        default:
            utils_error_log("Error", "average_array", "Unsupported weights sum type");
            free_array(weighted_sum);
            free_array(weights_sum);
            return NULL;
    }

    if (zero_weight) {
        free_array(weighted_sum);
        free_array(weights_sum);
        utils_error_log("Error", "average_array", "Sum of weights is zero");
        return NULL;
    }

    // compute weighted average = weighted sum / sum of weights
    Array *result = divide(weighted_sum, weights_sum);

    free_array(weighted_sum);
    free_array(weights_sum);

    return result;
}