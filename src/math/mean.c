#include "array.h"
#include "def.h"
#include "utils.h"
#include "shape.h"


static Array* divide_scalar_array(Array *arr, void *scalar, DataType scalar_type) {
    if (arr == NULL || scalar == NULL) {
        NP_UTILS_ERROR_LOG("Error", "NULL argument");
        return NULL;
    }

    Array *result = create_array(arr->shape, arr->ndim, arr->dtype);
    if (result == NULL) {
        NP_UTILS_ERROR_LOG("Error", "Failed to create result array");
        return NULL;
    }

    size_t element_size = dtype_size(arr->dtype);
    
    switch (arr->dtype) {
        case INT8: {
            int8_t *data = (int8_t*)arr->data;
            int8_t *res_data = (int8_t*)result->data;
            if (scalar_type == INT8) {
                int8_t s = *(int8_t*)scalar;
                for (int i = 0; i < arr->size; i++) {
                    res_data[i] = data[i] / s;
                }
            } else {
                double s = *(double*)scalar;
                for (int i = 0; i < arr->size; i++) {
                    res_data[i] = (int8_t)(data[i] / s);
                }
            }
            break;
        }
        case INT16: {
            int16_t *data = (int16_t*)arr->data;
            int16_t *res_data = (int16_t*)result->data;
            double s = *(double*)scalar;
            for (int i = 0; i < arr->size; i++) {
                res_data[i] = (int16_t)(data[i] / s);
            }
            break;
        }
        case INT32: {
            int32_t *data = (int32_t*)arr->data;
            int32_t *res_data = (int32_t*)result->data;
            double s = *(double*)scalar;
            for (int i = 0; i < arr->size; i++) {
                res_data[i] = (int32_t)(data[i] / s);
            }
            break;
        }
        case INT64: {
            int64_t *data = (int64_t*)arr->data;
            int64_t *res_data = (int64_t*)result->data;
            double s = *(double*)scalar;
            for (int i = 0; i < arr->size; i++) {
                res_data[i] = (int64_t)(data[i] / s);
            }
            break;
        }
        case FLOAT32: {
            float *data = (float*)arr->data;
            float *res_data = (float*)result->data;
            float s = *(float*)scalar;
            for (int i = 0; i < arr->size; i++) {
                res_data[i] = data[i] / s;
            }
            break;
        }
        case FLOAT64: {
            double *data = (double*)arr->data;
            double *res_data = (double*)result->data;
            double s = *(double*)scalar;
            for (int i = 0; i < arr->size; i++) {
                res_data[i] = data[i] / s;
            }
            break;
        }
        case COMPLEX64: {
            complex float *data = (complex float*)arr->data;
            complex float *res_data = (complex float*)result->data;
            complex float s = *(complex float*)scalar;
            for (int i = 0; i < arr->size; i++) {
                res_data[i] = data[i] / s;
            }
            break;
        }
        case COMPLEX128: {
            complex double *data = (complex double*)arr->data;
            complex double *res_data = (complex double*)result->data;
            complex double s = *(complex double*)scalar;
            for (int i = 0; i < arr->size; i++) {
                res_data[i] = data[i] / s;
            }
            break;
        }
        default:
            NP_UTILS_ERROR_LOG("Error", "Unsupported data type (%s)", dtype_name(arr->dtype));
            NP_UTILS_SAFE_FREE_ARRAY(result);
            return NULL;
    }

    return result;
}


Array* mean(Array *arr, int axis, int keepdims) {
    NP_UTILS_CHECK_NULL(arr, NULL);
    NP_UTILS_CHECK_SIZE(arr, NULL);
    
    if (axis < -1 || axis >= arr->ndim) {
        NP_UTILS_ERROR_LOG("Error", "Invalid axis %d for array with %d dimensions", 
                          axis, arr->ndim);
        return NULL;
    }
    
    // first compute the total sum
    Array *sum_result = sum(arr, axis, keepdims);
    if (sum_result == NULL) {
        return NULL;
    }
    
    // compute the number of elements
    double count = (double)arr->size;
    if (axis != -1) {
        // number of elements along specific axis
        count = (double)arr->shape[axis];
    }
    
    // compute mean = total sum / element count
    Array *result = NULL;
    
    switch (sum_result->dtype) {
        case INT64:
        case UINT64:
        case FLOAT32:
        case FLOAT64: {
            double scalar = count;
            result = divide_scalar_array(sum_result, &scalar, FLOAT64);
            break;
        }
        case COMPLEX64: {
            complex float scalar = count + 0.0f*I;
            result = divide_scalar_array(sum_result, &scalar, COMPLEX64);
            break;
        }
        case COMPLEX128: {
            complex double scalar = count + 0.0*I;
            result = divide_scalar_array(sum_result, &scalar, COMPLEX128);
            break;
        }
        case COMPLEX256: {
            complex long double scalar = count + 0.0L*I;
            result = divide_scalar_array(sum_result, &scalar, COMPLEX256);
            break;
        }
        default:
            NP_UTILS_ERROR_LOG("Error", "Unsupported result type in mean calculation");
            break;
    }
    
    NP_UTILS_SAFE_FREE_ARRAY(sum_result);
    return result;
}