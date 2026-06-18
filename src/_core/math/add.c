#include "np_math_macros.h"

Array* add(Array *arr1, Array *arr2) {
    if (!check_elementwise_op(arr1, arr2)) return NULL;
    Array *result = create_array(arr1->shape, arr1->ndim, arr1->dtype);
    if (result == NULL) { NP_UTILS_ERROR_LOG("Error", "Failed to create result array"); return NULL; }
    switch (arr1->dtype) {
        NP_INTEGER_OP("addition", NP_ADD_OVERFLOW, INT8, int8_t)
        NP_INTEGER_OP("addition", NP_ADD_OVERFLOW, INT16, int16_t)
        NP_INTEGER_OP("addition", NP_ADD_OVERFLOW, INT32, int32_t)
        NP_INTEGER_OP("addition", NP_ADD_OVERFLOW, INT64, int64_t)
        NP_INTEGER_OP("addition", NP_ADD_OVERFLOW, UINT8, uint8_t)
        NP_INTEGER_OP("addition", NP_ADD_OVERFLOW, UINT16, uint16_t)
        NP_INTEGER_OP("addition", NP_ADD_OVERFLOW, UINT32, uint32_t)
        NP_INTEGER_OP("addition", NP_ADD_OVERFLOW, UINT64, uint64_t)
        NP_ELEMENTWISE_OP("addition", +, FLOAT32, float)
        NP_ELEMENTWISE_OP("addition", +, FLOAT64, double)
        NP_ELEMENTWISE_OP("addition", +, COMPLEX64, complex float)
        NP_ELEMENTWISE_OP("addition", +, COMPLEX128, complex double)
        NP_ELEMENTWISE_OP("addition", +, COMPLEX256, complex long double)
        default:
            NP_UTILS_ERROR_LOG("Error", "Unsupported data type (%s)", dtype_name(arr1->dtype));
            NP_UTILS_SAFE_FREE_ARRAY(result);
            return NULL;
    }
    return result;
}
