#include <math.h>
#include <float.h>
#include <complex.h>
#include "array.h"

Array* nan_to_num(Array *arr) {
    if (arr == NULL) {
        fprintf(stderr, "nan_to_num: NULL array argument\n");
        return NULL;
    }
    // create result copy (same shape, same type)
    Array *res = create_array(arr->shape, arr->ndim, arr->dtype);
    if (res == NULL) return NULL;

    size_t elem_sz = dtype_size(arr->dtype);
    char *src = (char*)arr->data;
    char *dst = (char*)res->data;

    for (int i = 0; i < arr->size; ++i) {
        void *p_src = src + i * elem_sz;
        void *p_dst = dst + i * elem_sz;

        switch (arr->dtype) {
            case INT8:   *(int8_t*)p_dst = *(int8_t*)p_src; break;
            case INT16:  *(int16_t*)p_dst = *(int16_t*)p_src; break;
            case INT32:  *(int32_t*)p_dst = *(int32_t*)p_src; break;
            case INT64:  *(int64_t*)p_dst = *(int64_t*)p_src; break;
            case UINT8:  *(uint8_t*)p_dst = *(uint8_t*)p_src; break;
            case UINT16: *(uint16_t*)p_dst = *(uint16_t*)p_src; break;
            case UINT32: *(uint32_t*)p_dst = *(uint32_t*)p_src; break;
            case UINT64: *(uint64_t*)p_dst = *(uint64_t*)p_src; break;
            /* integer: no need to handle NaN/Inf */

            case FLOAT32: {
                float val = *(float*)p_src;
                if (isnan(val)) {
                    *(float*)p_dst = 0.0f;
                } else if (isinf(val)) {
                    if (val > 0) *(float*)p_dst = FLT_MAX;
                    else *(float*)p_dst = -FLT_MAX;
                } else {
                    *(float*)p_dst = val;
                }
                break;
            }
            case FLOAT64: {
                double val = *(double*)p_src;
                if (isnan(val)) {
                    *(double*)p_dst = 0.0;
                } else if (isinf(val)) {
                    if (val > 0) *(double*)p_dst = DBL_MAX;
                    else *(double*)p_dst = -DBL_MAX;
                } else {
                    *(double*)p_dst = val;
                }
                break;
            }
            case FLOAT128: {
                long double val = *(long double*)p_src;
                if (isnan(val)) {
                    *(long double*)p_dst = 0.0L;
                } else if (isinf(val)) {
                    if (val > 0) *(long double*)p_dst = LDBL_MAX;
                    else *(long double*)p_dst = -LDBL_MAX;
                } else {
                    *(long double*)p_dst = val;
                }
                break;
            }
            case COMPLEX64: {
                complex float val = *(complex float*)p_src;
                float real = crealf(val);
                float imag = cimagf(val);
                if (isnan(real)) real = 0.0f;
                else if (isinf(real)) real = (real > 0) ? FLT_MAX : -FLT_MAX;
                if (isnan(imag)) imag = 0.0f;
                else if (isinf(imag)) imag = (imag > 0) ? FLT_MAX : -FLT_MAX;
                *(complex float*)p_dst = real + imag * I;
                break;
            }
            case COMPLEX128: {
                complex double val = *(complex double*)p_src;
                double real = creal(val);
                double imag = cimag(val);
                if (isnan(real)) real = 0.0;
                else if (isinf(real)) real = (real > 0) ? DBL_MAX : -DBL_MAX;
                if (isnan(imag)) imag = 0.0;
                else if (isinf(imag)) imag = (imag > 0) ? DBL_MAX : -DBL_MAX;
                *(complex double*)p_dst = real + imag * I;
                break;
            }
            case COMPLEX256: {
                complex long double val = *(complex long double*)p_src;
                long double real = creall(val);
                long double imag = cimagl(val);
                if (isnan(real)) real = 0.0L;
                else if (isinf(real)) real = (real > 0) ? LDBL_MAX : -LDBL_MAX;
                if (isnan(imag)) imag = 0.0L;
                else if (isinf(imag)) imag = (imag > 0) ? LDBL_MAX : -LDBL_MAX;
                *(complex long double*)p_dst = real + imag * I;
                break;
            }
            default:
                fprintf(stderr, "nan_to_num: unsupported data type %s\n", dtype_name(arr->dtype));
                free_array(res);
                return NULL;
        }
    }
    return res;
}