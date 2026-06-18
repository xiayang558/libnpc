#include "np_math_macros.h"

Array* sqrt_array(Array *arr) {
    if (arr == NULL) { NP_UTILS_ERROR_LOG("Error", "NULL array argument"); return NULL; }
    DataType result_dtype;
    switch (arr->dtype) { case INT8:case INT16:case INT32:case INT64:case UINT8:case UINT16:case UINT32:case UINT64: result_dtype=FLOAT32; break; default: result_dtype=arr->dtype; break; }
    Array *result = create_array(arr->shape, arr->ndim, result_dtype);
    if (result == NULL) { NP_UTILS_ERROR_LOG("Error", "Failed to create result array"); return NULL; }
    switch (arr->dtype) {
        NP_INT_SQRT_OP(INT8,int8_t) NP_INT_SQRT_OP(INT16,int16_t) NP_INT_SQRT_OP(INT32,int32_t) NP_INT_SQRT_OP(INT64,int64_t)
        NP_UINT_SQRT_OP(UINT8,uint8_t) NP_UINT_SQRT_OP(UINT16,uint16_t) NP_UINT_SQRT_OP(UINT32,uint32_t) NP_UINT_SQRT_OP(UINT64,uint64_t)
        NP_FLOAT_SQRT_OP(FLOAT32,float,sqrtf) NP_FLOAT_SQRT_OP(FLOAT64,double,sqrt)
        NP_COMPLEX_SQRT_OP(COMPLEX64,complex float,csqrtf) NP_COMPLEX_SQRT_OP(COMPLEX128,complex double,csqrt) NP_COMPLEX_SQRT_OP(COMPLEX256,long double complex,csqrtl)
        default: NP_UTILS_ERROR_LOG("Error","Unsupported data type (%s)",dtype_name(arr->dtype)); NP_UTILS_SAFE_FREE_ARRAY(result); return NULL;
    }
    return result;
}
