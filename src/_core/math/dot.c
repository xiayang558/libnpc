#include "np_math_macros.h"

Array* dot(Array *arr1, Array *arr2) {
    if (arr1 == NULL || arr2 == NULL) { NP_UTILS_ERROR_LOG("Error", "NULL array argument"); return NULL; }
    if (arr1->dtype != arr2->dtype) {
        NP_UTILS_ERROR_LOG("Error", "Data type mismatch (arr1: %s, arr2: %s)", dtype_name(arr1->dtype), dtype_name(arr2->dtype));
        return NULL;
    }
    int ndim1 = arr1->ndim, ndim2 = arr2->ndim;
    int k1 = (ndim1 >= 1) ? arr1->shape[ndim1-1] : 0;
    int k2 = (ndim2 >= 2) ? arr2->shape[ndim2-2] : (ndim2 == 1 ? arr2->shape[0] : 0);
    if (k1 != k2) {
        NP_UTILS_ERROR_LOG("Error", "Incompatible dimensions (arr1 last dim: %d, arr2 penultimate dim: %d)", k1, k2);
        return NULL;
    }
    int *res_shape = NULL; int res_ndim = 0;
    if (ndim1 == 1 && ndim2 == 1) { res_ndim = 1; res_shape = malloc(res_ndim*sizeof(int)); res_shape[0] = 1; }
    else if (ndim1 == 1 && ndim2 >= 2) { res_ndim = ndim2-1; res_shape = malloc(res_ndim*sizeof(int)); memcpy(res_shape, arr2->shape, (ndim2-2)*sizeof(int)); res_shape[res_ndim-1] = arr2->shape[ndim2-1]; }
    else if (ndim1 >= 2 && ndim2 == 1) { res_ndim = ndim1-1; res_shape = malloc(res_ndim*sizeof(int)); memcpy(res_shape, arr1->shape, (ndim1-1)*sizeof(int)); }
    else { res_ndim = (ndim1-1)+1; res_shape = malloc(res_ndim*sizeof(int)); memcpy(res_shape, arr1->shape, (ndim1-1)*sizeof(int)); res_shape[res_ndim-1] = arr2->shape[ndim2-1]; }
    if (!res_shape) { NP_UTILS_ERROR_LOG("Error", "Malloc failed for result shape"); return NULL; }
    Array *result = create_array(res_shape, res_ndim, arr1->dtype);
    free(res_shape);
    if (result == NULL) { NP_UTILS_ERROR_LOG("Error", "Failed to create result array"); return NULL; }
    switch (arr1->dtype) {
        NP_INTEGER_DOT_MULTI_OP(INT8, int8_t) NP_INTEGER_DOT_MULTI_OP(INT16, int16_t)
        NP_INTEGER_DOT_MULTI_OP(INT32, int32_t) NP_INTEGER_DOT_MULTI_OP(INT64, int64_t)
        NP_INTEGER_DOT_MULTI_OP(UINT8, uint8_t) NP_INTEGER_DOT_MULTI_OP(UINT16, uint16_t)
        NP_INTEGER_DOT_MULTI_OP(UINT32, uint32_t) NP_INTEGER_DOT_MULTI_OP(UINT64, uint64_t)
        NP_GENERAL_DOT_MULTI_OP(FLOAT32, float, 0.0f)
        NP_GENERAL_DOT_MULTI_OP(FLOAT64, double, 0.0)
        NP_GENERAL_DOT_MULTI_OP(COMPLEX64, complex float, 0.0f + 0.0f*I)
        NP_GENERAL_DOT_MULTI_OP(COMPLEX128, complex double, 0.0 + 0.0*I)
        NP_GENERAL_DOT_MULTI_OP(COMPLEX256, complex long double, 0.0L + 0.0L*I)
        default:
            NP_UTILS_ERROR_LOG("Error", "Unsupported data type (%s)", dtype_name(arr1->dtype));
            NP_UTILS_SAFE_FREE_ARRAY(result); return NULL;
    }
    return result;
}
