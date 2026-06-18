#include "array.h"


/** *****************************************************************
 * 
 * @brief Clip (limit) the values in an array. 
 * Given an interval, values outside the interval are clipped to the interval edges.
 * For example, if an interval of [0, 1] is specified, values smaller than 0 become 0, 
 * and values larger than 1 become 1.
 * 
 * Equivalent to but faster than "minimum(a_max, maximum(a, a_min))".
 * 
 * No check is performed to ensure "a_min < a_max".
 * 
 * @param arr Input array.
 * @param min Minimum value. If an array, must be broadcastable to the shape of arr.
 * @param max Maximum value. If an array, must be broadcastable to the shape
 * 
 * @return A new array with the same shape and type as arr, where values are clipped 
 * to the specified min and max.
 * *****************************************************************
*/
Array* clip(Array *arr, Array *a_min, Array *a_max) {
    if (!check_null(arr, "clip") || 
        !check_null(a_min, "clip") || 
        !check_null(a_max, "clip")) {
        return NULL;
    }

    // Check data types are consistent (simplified: requires all three types to be the same)
    if (arr->dtype != a_min->dtype || arr->dtype != a_max->dtype) {
        utils_error_log("Error", "clip", 
                        "Data types of arr, a_min, a_max must be the same");
        return NULL;
    }

    // Check broadcast feasibility: a_min and a_max must have the same shape as arr, or be scalar (0-d)
    int min_ok = (a_min->ndim == 0) ? 1 : 0;
    int max_ok = (a_max->ndim == 0) ? 1 : 0;

    if (!min_ok && a_min->ndim == arr->ndim) {
        min_ok = 1;
        for (int i = 0; i < arr->ndim; i++) {
            if (a_min->shape[i] != arr->shape[i]) {
                min_ok = 0;
                break;
            }
        }
    }
    if (!max_ok && a_max->ndim == arr->ndim) {
        max_ok = 1;
        for (int i = 0; i < arr->ndim; i++) {
            if (a_max->shape[i] != arr->shape[i]) {
                max_ok = 0;
                break;
            }
        }
    }

    if (!min_ok) {
        utils_error_log("Error", "clip", "a_min array shape is not broadcastable");
        return NULL;
    }
    if (!max_ok) {
        utils_error_log("Error", "clip", "a_max array shape is not broadcastable");
        return NULL;
    }

    // Create result array (same shape and type as arr)
    Array *result = create_array(arr->shape, arr->ndim, arr->dtype);
    if (result == NULL) {
        utils_error_log("Error", "clip", "Failed to create result array");
        return NULL;
    }

    size_t elem_size = dtype_size(arr->dtype);
    int min_is_scalar = (a_min->ndim == 0);
    int max_is_scalar = (a_max->ndim == 0);

    // Element-wise processing by data type
    switch (arr->dtype) {
        case INT8: {
            int8_t *a = (int8_t*)arr->data;
            int8_t *r = (int8_t*)result->data;
            int8_t *min_data = (int8_t*)a_min->data;
            int8_t *max_data = (int8_t*)a_max->data;
            for (int i = 0; i < arr->size; i++) {
                int8_t val = a[i];
                int8_t minv = min_is_scalar ? *min_data : min_data[i];
                int8_t maxv = max_is_scalar ? *max_data : max_data[i];
                r[i] = (val < minv) ? minv : ((val > maxv) ? maxv : val);
            }
            break;
        }
        case INT16: {
            int16_t *a = (int16_t*)arr->data;
            int16_t *r = (int16_t*)result->data;
            int16_t *min_data = (int16_t*)a_min->data;
            int16_t *max_data = (int16_t*)a_max->data;
            for (int i = 0; i < arr->size; i++) {
                int16_t val = a[i];
                int16_t minv = min_is_scalar ? *min_data : min_data[i];
                int16_t maxv = max_is_scalar ? *max_data : max_data[i];
                r[i] = (val < minv) ? minv : ((val > maxv) ? maxv : val);
            }
            break;
        }
        case INT32: {
            int32_t *a = (int32_t*)arr->data;
            int32_t *r = (int32_t*)result->data;
            int32_t *min_data = (int32_t*)a_min->data;
            int32_t *max_data = (int32_t*)a_max->data;
            for (int i = 0; i < arr->size; i++) {
                int32_t val = a[i];
                int32_t minv = min_is_scalar ? *min_data : min_data[i];
                int32_t maxv = max_is_scalar ? *max_data : max_data[i];
                r[i] = (val < minv) ? minv : ((val > maxv) ? maxv : val);
            }
            break;
        }
        case INT64: {
            int64_t *a = (int64_t*)arr->data;
            int64_t *r = (int64_t*)result->data;
            int64_t *min_data = (int64_t*)a_min->data;
            int64_t *max_data = (int64_t*)a_max->data;
            for (int i = 0; i < arr->size; i++) {
                int64_t val = a[i];
                int64_t minv = min_is_scalar ? *min_data : min_data[i];
                int64_t maxv = max_is_scalar ? *max_data : max_data[i];
                r[i] = (val < minv) ? minv : ((val > maxv) ? maxv : val);
            }
            break;
        }
        case UINT8: {
            uint8_t *a = (uint8_t*)arr->data;
            uint8_t *r = (uint8_t*)result->data;
            uint8_t *min_data = (uint8_t*)a_min->data;
            uint8_t *max_data = (uint8_t*)a_max->data;
            for (int i = 0; i < arr->size; i++) {
                uint8_t val = a[i];
                uint8_t minv = min_is_scalar ? *min_data : min_data[i];
                uint8_t maxv = max_is_scalar ? *max_data : max_data[i];
                r[i] = (val < minv) ? minv : ((val > maxv) ? maxv : val);
            }
            break;
        }
        case UINT16: {
            uint16_t *a = (uint16_t*)arr->data;
            uint16_t *r = (uint16_t*)result->data;
            uint16_t *min_data = (uint16_t*)a_min->data;
            uint16_t *max_data = (uint16_t*)a_max->data;
            for (int i = 0; i < arr->size; i++) {
                uint16_t val = a[i];
                uint16_t minv = min_is_scalar ? *min_data : min_data[i];
                uint16_t maxv = max_is_scalar ? *max_data : max_data[i];
                r[i] = (val < minv) ? minv : ((val > maxv) ? maxv : val);
            }
            break;
        }
        case UINT32: {
            uint32_t *a = (uint32_t*)arr->data;
            uint32_t *r = (uint32_t*)result->data;
            uint32_t *min_data = (uint32_t*)a_min->data;
            uint32_t *max_data = (uint32_t*)a_max->data;
            for (int i = 0; i < arr->size; i++) {
                uint32_t val = a[i];
                uint32_t minv = min_is_scalar ? *min_data : min_data[i];
                uint32_t maxv = max_is_scalar ? *max_data : max_data[i];
                r[i] = (val < minv) ? minv : ((val > maxv) ? maxv : val);
            }
            break;
        }
        case UINT64: {
            uint64_t *a = (uint64_t*)arr->data;
            uint64_t *r = (uint64_t*)result->data;
            uint64_t *min_data = (uint64_t*)a_min->data;
            uint64_t *max_data = (uint64_t*)a_max->data;
            for (int i = 0; i < arr->size; i++) {
                uint64_t val = a[i];
                uint64_t minv = min_is_scalar ? *min_data : min_data[i];
                uint64_t maxv = max_is_scalar ? *max_data : max_data[i];
                r[i] = (val < minv) ? minv : ((val > maxv) ? maxv : val);
            }
            break;
        }
        case FLOAT32: {
            float *a = (float*)arr->data;
            float *r = (float*)result->data;
            float *min_data = (float*)a_min->data;
            float *max_data = (float*)a_max->data;
            for (int i = 0; i < arr->size; i++) {
                float val = a[i];
                float minv = min_is_scalar ? *min_data : min_data[i];
                float maxv = max_is_scalar ? *max_data : max_data[i];
                r[i] = (val < minv) ? minv : ((val > maxv) ? maxv : val);
            }
            break;
        }
        case FLOAT64: {
            double *a = (double*)arr->data;
            double *r = (double*)result->data;
            double *min_data = (double*)a_min->data;
            double *max_data = (double*)a_max->data;
            for (int i = 0; i < arr->size; i++) {
                double val = a[i];
                double minv = min_is_scalar ? *min_data : min_data[i];
                double maxv = max_is_scalar ? *max_data : max_data[i];
                r[i] = (val < minv) ? minv : ((val > maxv) ? maxv : val);
            }
            break;
        }
        case COMPLEX64:
        case COMPLEX128:
            utils_error_log("Error", "clip", "Complex types not supported for clip");
            free_array(result);
            return NULL;
        default:
            utils_error_log("Error", "clip", "Unsupported data type %s", 
                            dtype_name(arr->dtype));
            free_array(result);
            return NULL;
    }

    return result;
}