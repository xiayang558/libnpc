#include "np_math_macros.h"

Array* log10_array(Array *arr) {
    if (arr == NULL) { NP_UTILS_ERROR_LOG("Error", "NULL array argument"); return NULL; }
    DataType result_dtype;
    switch (arr->dtype) {
        case INT8:case INT16:case INT32:case INT64:case UINT8:case UINT16:case UINT32:case UINT64:case FLOAT32:case FLOAT64: result_dtype=FLOAT64; break;
        case COMPLEX64:case COMPLEX128: result_dtype=COMPLEX128; break;
        case COMPLEX256: result_dtype=COMPLEX256; break;
        default: NP_UTILS_ERROR_LOG("Error","Unsupported data type (%s)",dtype_name(arr->dtype)); return NULL;
    }
    Array *result = create_array(arr->shape, arr->ndim, result_dtype);
    if (result == NULL) { NP_UTILS_ERROR_LOG("Error", "Failed to create result array"); return NULL; }
    switch (arr->dtype) {
        NP_INT_LOG_OP(INT8,int8_t,log10,"10") NP_INT_LOG_OP(INT16,int16_t,log10,"10")
        NP_INT_LOG_OP(INT32,int32_t,log10,"10") NP_INT_LOG_OP(INT64,int64_t,log10,"10")
        NP_UINT_LOG_OP(UINT8,uint8_t,log10,"10") NP_UINT_LOG_OP(UINT16,uint16_t,log10,"10")
        NP_UINT_LOG_OP(UINT32,uint32_t,log10,"10") NP_UINT_LOG_OP(UINT64,uint64_t,log10,"10")
        NP_FLOAT_LOG_OP(FLOAT32,float,log10,"10") NP_FLOAT_LOG_OP(FLOAT64,double,log10,"10")
        NP_COMPLEX_LOG_OP(COMPLEX64,complex float,clog,10.0+0.0*I,"10")
        NP_COMPLEX_LOG_OP(COMPLEX128,complex double,clog,10.0+0.0*I,"10")
        NP_LONG_COMPLEX_LOG_OP(COMPLEX256,long double complex,clogl,10.0L+0.0L*I,"10")
        default: NP_UTILS_ERROR_LOG("Error","Unsupported data type (%s)",dtype_name(arr->dtype)); NP_UTILS_SAFE_FREE_ARRAY(result); return NULL;
    }
    return result;
}
