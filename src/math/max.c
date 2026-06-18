#include "array.h"
#include "def.h"
#include "utils.h"


Array* max(Array *arr, int axis, int keepdims) {
    NP_UTILS_CHECK_NULL(arr, NULL);
    NP_UTILS_CHECK_SIZE(arr, NULL);
    
    if (axis < -1 || axis >= arr->ndim) {
        NP_UTILS_ERROR_LOG("Error", "Invalid axis %d for array with %d dimensions", 
                          axis, arr->ndim);
        return NULL;
    }
    
    // if axis is -1, compute all elements maximum
    if (axis == -1) {
        // create buffer to store maximum
        void *max_val = malloc(dtype_size(arr->dtype));
        if (max_val == NULL) {
            NP_UTILS_ERROR_LOG("Error", "Memory allocation failed");
            return NULL;
        }
        
        DataType result_type = UNKNOWN;
        
        switch (arr->dtype) {
            // signed integer
            NP_UTILS_INT_MAX(INT8, int8_t, max_val, &result_type)
            NP_UTILS_INT_MAX(INT16, int16_t, max_val, &result_type)
            NP_UTILS_INT_MAX(INT32, int32_t, max_val, &result_type)
            NP_UTILS_INT_MAX(INT64, int64_t, max_val, &result_type)
            
            // unsigned integer
            NP_UTILS_INT_MAX(UINT8, uint8_t, max_val, &result_type)
            NP_UTILS_INT_MAX(UINT16, uint16_t, max_val, &result_type)
            NP_UTILS_INT_MAX(UINT32, uint32_t, max_val, &result_type)
            NP_UTILS_INT_MAX(UINT64, uint64_t, max_val, &result_type)
            
            // floating point numbers
            NP_UTILS_FLOAT_MAX(FLOAT32, float, max_val, &result_type)
            NP_UTILS_FLOAT_MAX(FLOAT64, double, max_val, &result_type)
            
            // complex (by magnitude)
            NP_UTILS_COMPLEX_MAX(COMPLEX64, complex float, cabsf, max_val, &result_type)
            NP_UTILS_COMPLEX_MAX(COMPLEX128, complex double, cabs, max_val, &result_type)
            NP_UTILS_COMPLEX_MAX(COMPLEX256, complex long double, cabsl, max_val, &result_type)
            
            default:
                NP_UTILS_ERROR_LOG("Error", "Unsupported data type (%s)", dtype_name(arr->dtype));
                free(max_val);
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
            free(max_val);
            return NULL;
        }
        
        // copy result value
        size_t elem_size = dtype_size(result_type);
        memcpy(result->data, max_val, elem_size);
        free(max_val);
        
        return result;
    }
    
    // compute maximum along a specific axis
    NP_UTILS_ERROR_LOG("Warning", "Axis-specific max not fully implemented, using full max");
    return max(arr, -1, keepdims);
}