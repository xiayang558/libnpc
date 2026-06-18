#include "array.h"
#include "def.h"
#include "utils.h"


Array* min(Array *arr, int axis, int keepdims) {
    NP_UTILS_CHECK_NULL(arr, NULL);
    NP_UTILS_CHECK_SIZE(arr, NULL);
    
    if (axis < -1 || axis >= arr->ndim) {
        NP_UTILS_ERROR_LOG("Error", "Invalid axis %d for array with %d dimensions", 
                          axis, arr->ndim);
        return NULL;
    }
    
    // if axis is -1, compute all elements minimum
    if (axis == -1) {
        // create buffer to store minimum
        void *min_val = malloc(dtype_size(arr->dtype));
        if (min_val == NULL) {
            NP_UTILS_ERROR_LOG("Error", "Memory allocation failed");
            return NULL;
        }
        
        DataType result_type = UNKNOWN;
        
        switch (arr->dtype) {
            // signed integer
            NP_UTILS_INT_MIN(INT8, int8_t, min_val, &result_type)
            NP_UTILS_INT_MIN(INT16, int16_t, min_val, &result_type)
            NP_UTILS_INT_MIN(INT32, int32_t, min_val, &result_type)
            NP_UTILS_INT_MIN(INT64, int64_t, min_val, &result_type)
            
            // unsigned integer
            NP_UTILS_INT_MIN(UINT8, uint8_t, min_val, &result_type)
            NP_UTILS_INT_MIN(UINT16, uint16_t, min_val, &result_type)
            NP_UTILS_INT_MIN(UINT32, uint32_t, min_val, &result_type)
            NP_UTILS_INT_MIN(UINT64, uint64_t, min_val, &result_type)
            
            // floating point numbers
            NP_UTILS_FLOAT_MIN(FLOAT32, float_t, min_val, &result_type)
            NP_UTILS_FLOAT_MIN(FLOAT64, double_t, min_val, &result_type)
            
            // complex (by magnitude)
            NP_UTILS_COMPLEX_MIN(COMPLEX64, complex float, cabsf, min_val, &result_type)
            NP_UTILS_COMPLEX_MIN(COMPLEX128, complex double, cabs, min_val, &result_type)
            NP_UTILS_COMPLEX_MIN(COMPLEX256, complex long double, cabsl, min_val, &result_type)
            
            default:
                NP_UTILS_ERROR_LOG("Error", "Unsupported data type (%s)", dtype_name(arr->dtype));
                free(min_val);
                return NULL;
        }
        
        // create result array
        int *shape = keepdims ? malloc(arr->ndim * sizeof(int)) : NULL;
        int ndim = keepdims ? arr->ndim : 0;
        
        if (keepdims) {
            for (int i = 0; i < arr->ndim; i++) {
                shape[i] = 1;
            }
        }
        
        Array *result = create_array(shape, ndim, result_type);
        if (shape) free(shape);
        
        if (result == NULL) {
            NP_UTILS_ERROR_LOG("Error", "Failed to create result array");
            free(min_val);
            return NULL;
        }
        
        // copy result value
        size_t elem_size = dtype_size(result_type);
        memcpy(result->data, min_val, elem_size);
        free(min_val);
        
        return result;
    }
    
    // compute minimum along a specific axis
    NP_UTILS_ERROR_LOG("Warning", "Axis-specific min not fully implemented, using full min");
    return min(arr, -1, keepdims);
}