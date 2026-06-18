#include "np_math_macros.h"

Array* divide(Array *arr1, Array *arr2) {
    if (!check_elementwise_op(arr1, arr2)) return NULL;
    Array *result = create_array(arr1->shape, arr1->ndim, arr1->dtype);
    if (result == NULL) { NP_UTILS_ERROR_LOG("Error", "Failed to create result array"); return NULL; }
    switch (arr1->dtype) {
        NP_INTEGER_DIV(INT8, int8_t)
        NP_INTEGER_DIV(INT16, int16_t)
        NP_INTEGER_DIV(INT32, int32_t)
        NP_INTEGER_DIV(INT64, int64_t)
        NP_INTEGER_DIV(UINT8, uint8_t)
        NP_INTEGER_DIV(UINT16, uint16_t)
        NP_INTEGER_DIV(UINT32, uint32_t)
        NP_INTEGER_DIV(UINT64, uint64_t)
        NP_FLOAT_DIV(FLOAT32, float)
        NP_FLOAT_DIV(FLOAT64, double)
        NP_COMPLEX_DIV(COMPLEX64, complex float)
        NP_COMPLEX_DIV(COMPLEX128, complex double)
        NP_COMPLEX_DIV(COMPLEX256, complex long double)
        default:
            NP_UTILS_ERROR_LOG("Error", "Unsupported data type (%s)", dtype_name(arr1->dtype));
            NP_UTILS_SAFE_FREE_ARRAY(result);
            return NULL;
    }
    return result;
}
