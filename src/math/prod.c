#include "array.h"
#include "def.h"
#include "utils.h"


Array* prod(Array *arr, int axis, int keepdims) {
    NP_UTILS_CHECK_NULL(arr, NULL);
    
    if (axis < -1 || axis >= arr->ndim) {
        NP_UTILS_ERROR_LOG("Error", "Invalid axis %d for array with %d dimensions", 
                          axis, arr->ndim);
        return NULL;
    }
    
    // if axis is -1, compute all elements product
    if (axis == -1) {
        // create result array (scalar)
        int *shape = keepdims ? malloc(arr->ndim * sizeof(int)) : NULL;
        int ndim = keepdims ? arr->ndim : 0;
        
        if (keepdims) {
            for (int i = 0; i < arr->ndim; i++) {
                shape[i] = 1;
            }
        }
        
        Array *result = create_array(shape, ndim, arr->dtype);
        if (shape) free(shape);
        
        if (result == NULL) {
            NP_UTILS_ERROR_LOG("Error", "Failed to create result array");
            return NULL;
        }
        
        // compute product
        switch (arr->dtype) {
            case INT32: {
                int *data = (int*)arr->data;
                int *res_data = (int*)result->data;
                int prod = 1;
                for (int i = 0; i < arr->size; i++) {
                    prod *= data[i];
                }
                *res_data = prod;
                break;
            }
            case FLOAT32: {
                float *data = (float*)arr->data;
                float *res_data = (float*)result->data;
                float prod = 1.0f;
                for (int i = 0; i < arr->size; i++) {
                    prod *= data[i];
                }
                *res_data = prod;
                break;
            }
            case FLOAT64: {
                double *data = (double*)arr->data;
                double *res_data = (double*)result->data;
                double prod = 1.0;
                for (int i = 0; i < arr->size; i++) {
                    prod *= data[i];
                }
                *res_data = prod;
                break;
            }
            case COMPLEX64: {
                complex float *data = (complex float*)arr->data;
                complex float *res_data = (complex float*)result->data;
                complex float prod = 1.0f + 0.0f*I;
                for (int i = 0; i < arr->size; i++) {
                    prod *= data[i];
                }
                *res_data = prod;
                break;
            }
            case COMPLEX128: {
                complex double *data = (complex double*)arr->data;
                complex double *res_data = (complex double*)result->data;
                complex double prod = 1.0 + 0.0*I;
                for (int i = 0; i < arr->size; i++) {
                    prod *= data[i];
                }
                *res_data = prod;
                break;
            }
            default:
                NP_UTILS_ERROR_LOG("Error", "Unsupported data type (%s)", dtype_name(arr->dtype));
                NP_UTILS_SAFE_FREE_ARRAY(result);
                return NULL;
        }
        
        return result;
    }
    
    // compute product along a specific axis
    NP_UTILS_ERROR_LOG("Warning", "Axis-specific product not fully implemented, using full product");
    return prod(arr, -1, keepdims);
}

