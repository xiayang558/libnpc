#include "array.h"
#include <math.h>

/**
 * negative — Return -x element-wise (like numpy.negative)
 *
 * Integer inputs → INT64 output (like numpy)
 * Float → same float, Complex → same complex
 *
 * @param x Input array
 * @return Negated array, or NULL on error
 */
Array* negative(Array *x) {
    if (x == NULL) {
        fprintf(stderr, "negative: NULL array argument\n");
        return NULL;
    }

    DataType out_dtype;
    switch (x->dtype) {
        case BOOL:
        case INT8:   case INT16:  case INT32:  case INT64:
        case UINT8:  case UINT16: case UINT32: case UINT64:
            out_dtype = INT64;
            break;
        case FLOAT32:  out_dtype = FLOAT32;  break;
        case FLOAT64:  out_dtype = FLOAT64;  break;
        case FLOAT128: out_dtype = FLOAT128; break;
        case COMPLEX64:  out_dtype = COMPLEX64;  break;
        case COMPLEX128: out_dtype = COMPLEX128; break;
        case COMPLEX256: out_dtype = COMPLEX256; break;
        default:
            fprintf(stderr, "negative: unsupported dtype %s\n", dtype_name(x->dtype));
            return NULL;
    }

    Array *res = create_array(x->shape, x->ndim, out_dtype);
    if (res == NULL) {
        fprintf(stderr, "negative: failed to create result array\n");
        return NULL;
    }

    size_t in_sz = dtype_size(x->dtype);
    size_t out_sz = dtype_size(out_dtype);
    char *in = (char*)x->data;
    char *out = (char*)res->data;

    for (int i = 0; i < x->size; i++) {
        void *pi = in + i * in_sz;
        void *po = out + i * out_sz;

        switch (x->dtype) {
            case BOOL: {
                uint8_t v = *(uint8_t*)pi;
                *(int64_t*)po = v ? -1 : 0;
                break;
            }
            case INT8: {
                int8_t v = *(int8_t*)pi;
                *(int64_t*)po = -(int64_t)v;
                break;
            }
            case INT16: {
                int16_t v = *(int16_t*)pi;
                *(int64_t*)po = -(int64_t)v;
                break;
            }
            case INT32: {
                int32_t v = *(int32_t*)pi;
                *(int64_t*)po = -(int64_t)v;
                break;
            }
            case INT64: {
                int64_t v = *(int64_t*)pi;
                *(int64_t*)po = -v;
                break;
            }
            case UINT8: {
                uint8_t v = *(uint8_t*)pi;
                *(int64_t*)po = -(int64_t)v;
                break;
            }
            case UINT16: {
                uint16_t v = *(uint16_t*)pi;
                *(int64_t*)po = -(int64_t)v;
                break;
            }
            case UINT32: {
                uint32_t v = *(uint32_t*)pi;
                *(int64_t*)po = -(int64_t)v;
                break;
            }
            case UINT64: {
                uint64_t v = *(uint64_t*)pi;
                *(int64_t*)po = -(int64_t)v;
                break;
            }
            case FLOAT32: {
                float v = *(float*)pi;
                *(float*)po = -v;  /* NaN stays NaN */
                break;
            }
            case FLOAT64: {
                double v = *(double*)pi;
                *(double*)po = -v;  /* NaN stays NaN */
                break;
            }
            case FLOAT128: {
                long double v = *(long double*)pi;
                *(long double*)po = -v;
                break;
            }
            case COMPLEX64: {
                complex float v = *(complex float*)pi;
                *(complex float*)po = -v;
                break;
            }
            case COMPLEX128: {
                complex double v = *(complex double*)pi;
                *(complex double*)po = -v;
                break;
            }
            case COMPLEX256: {
                complex long double v = *(complex long double*)pi;
                *(complex long double*)po = -v;
                break;
            }
            default:
                break;
        }
    }

    return res;
}
