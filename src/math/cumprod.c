#include "array.h"
#include "def.h"
#include "utils.h"


Array* cumprod(Array *arr, int axis) {
    NP_UTILS_CHECK_NULL(arr, NULL);
    
    if (axis < -1 || axis >= arr->ndim) {
        NP_UTILS_ERROR_LOG("Error", "Invalid axis %d for array with %d dimensions", 
                          axis, arr->ndim);
        return NULL;
    }
    
    // if axis is -1, flatten array and compute cumulative product
    if (axis == -1 || arr->ndim == 1) {
        // create result array
        Array *result = create_array(arr->shape, arr->ndim, arr->dtype);
        if (result == NULL) {
            NP_UTILS_ERROR_LOG("Error", "Failed to create result array");
            return NULL;
        }
        
        // compute cumulative product
        switch (arr->dtype) {
            case INT32: {
                int *data = (int*)arr->data;
                int *res_data = (int*)result->data;
                res_data[0] = data[0];
                for (int i = 1; i < arr->size; i++) {
                    res_data[i] = res_data[i-1] * data[i];
                }
                break;
            }
            case FLOAT32: {
                float *data = (float*)arr->data;
                float *res_data = (float*)result->data;
                res_data[0] = data[0];
                for (int i = 1; i < arr->size; i++) {
                    res_data[i] = res_data[i-1] * data[i];
                }
                break;
            }
            case FLOAT64: {
                double *data = (double*)arr->data;
                double *res_data = (double*)result->data;
                res_data[0] = data[0];
                for (int i = 1; i < arr->size; i++) {
                    res_data[i] = res_data[i-1] * data[i];
                }
                break;
            }
            case COMPLEX64: {
                complex float *data = (complex float*)arr->data;
                complex float *res_data = (complex float*)result->data;
                res_data[0] = data[0];
                for (int i = 1; i < arr->size; i++) {
                    res_data[i] = res_data[i-1] * data[i];
                }
                break;
            }
            case COMPLEX128: {
                complex double *data = (complex double*)arr->data;
                complex double *res_data = (complex double*)result->data;
                res_data[0] = data[0];
                for (int i = 1; i < arr->size; i++) {
                    res_data[i] = res_data[i-1] * data[i];
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
    
    // compute cumulative product along a specific axis
    NP_UTILS_ERROR_LOG("Warning", "Axis-specific cumprod not fully implemented, using flattened cumprod");
    return cumprod(arr, -1);
}