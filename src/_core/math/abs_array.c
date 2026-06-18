#include "np_math_macros.h"

Array* abs_array(Array *arr) {
    if (arr == NULL) { NP_UTILS_ERROR_LOG("Error", "NULL array argument"); return NULL; }
    DataType result_dtype;
    switch (arr->dtype) { case COMPLEX64: result_dtype=FLOAT32; break; case COMPLEX128: result_dtype=FLOAT64; break; case COMPLEX256: result_dtype=FLOAT128; break; default: result_dtype=arr->dtype; break; }
    Array *result = create_array(arr->shape, arr->ndim, result_dtype);
    if (result == NULL) { NP_UTILS_ERROR_LOG("Error", "Failed to create result array"); return NULL; }
    switch (arr->dtype) {
        NP_INT_ABS_OP(INT8,int8_t) NP_INT_ABS_OP(INT16,int16_t) NP_INT_ABS_OP(INT32,int32_t) NP_INT_ABS_OP(INT64,int64_t)
        NP_INT_ABS_OP(UINT8,uint8_t) NP_INT_ABS_OP(UINT16,uint16_t) NP_INT_ABS_OP(UINT32,uint32_t) NP_INT_ABS_OP(UINT64,uint64_t)
        NP_FLOAT_ABS_OP(FLOAT32,float,fabsf) NP_FLOAT_ABS_OP(FLOAT64,double,fabs) NP_FLOAT_ABS_OP(FLOAT128,long double,fabsl)
        NP_COMPLEX_ABS_OP(COMPLEX64,complex float,cabsf,float) NP_COMPLEX_ABS_OP(COMPLEX128,complex double,cabs,double) NP_COMPLEX_ABS_OP(COMPLEX256,long double complex,cabsl,long double)
        default: NP_UTILS_ERROR_LOG("Error","Unsupported data type (%s)",dtype_name(arr->dtype)); NP_UTILS_SAFE_FREE_ARRAY(result); return NULL;
    }
    return result;
}
