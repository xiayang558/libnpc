#include "np_math_macros.h"

Array* add_scalar(Array *arr, void *scalar) {
    if (arr == NULL || scalar == NULL) { NP_UTILS_ERROR_LOG("Error", "NULL argument"); return NULL; }
    Array *result = create_array(arr->shape, arr->ndim, arr->dtype);
    if (result == NULL) { NP_UTILS_ERROR_LOG("Error", "Failed to create result array"); return NULL; }
    switch (arr->dtype) {
        NP_INTEGER_SCALAR_OP("addition", NP_ADD_OVERFLOW, INT8, int8_t)
        NP_INTEGER_SCALAR_OP("addition", NP_ADD_OVERFLOW, INT16, int16_t)
        NP_INTEGER_SCALAR_OP("addition", NP_ADD_OVERFLOW, INT32, int32_t)
        NP_INTEGER_SCALAR_OP("addition", NP_ADD_OVERFLOW, INT64, int64_t)
        NP_INTEGER_SCALAR_OP("addition", NP_ADD_OVERFLOW, UINT8, uint8_t)
        NP_INTEGER_SCALAR_OP("addition", NP_ADD_OVERFLOW, UINT16, uint16_t)
        NP_INTEGER_SCALAR_OP("addition", NP_ADD_OVERFLOW, UINT32, uint32_t)
        NP_INTEGER_SCALAR_OP("addition", NP_ADD_OVERFLOW, UINT64, uint64_t)
        NP_SCALAR_OP("addition", +, FLOAT32, float)
        NP_SCALAR_OP("addition", +, FLOAT64, double)
        NP_SCALAR_OP("addition", +, COMPLEX64, complex float)
        NP_SCALAR_OP("addition", +, COMPLEX128, complex double)
        NP_SCALAR_OP("addition", +, COMPLEX256, complex long double)
        default:
            NP_UTILS_ERROR_LOG("Error", "Unsupported data type (%s)", dtype_name(arr->dtype));
            NP_UTILS_SAFE_FREE_ARRAY(result);
            return NULL;
    }
    return result;
}
