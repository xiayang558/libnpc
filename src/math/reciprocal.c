#include "array.h"

/**
 * reciprocal — Return 1/x element-wise (like numpy.reciprocal)
 *
 * Integer inputs → FLOAT64 output
 * Float types → same float type output
 * Complex types → same complex type output
 *
 * @param x Input array
 * @return Result array with reciprocal values, or NULL on error
 */
Array* reciprocal(Array *x) {
    if (x == NULL) {
        fprintf(stderr, "reciprocal: NULL array argument\n");
        return NULL;
    }

    DataType out_dtype;
    switch (x->dtype) {
        case INT8:   case INT16:  case INT32:  case INT64:
        case UINT8:  case UINT16: case UINT32: case UINT64:
        case BOOL:
            out_dtype = FLOAT64;
            break;
        case FLOAT32:  out_dtype = FLOAT32;  break;
        case FLOAT64:  out_dtype = FLOAT64;  break;
        case FLOAT128: out_dtype = FLOAT128; break;
        case COMPLEX64:  out_dtype = COMPLEX64;  break;
        case COMPLEX128: out_dtype = COMPLEX128; break;
        case COMPLEX256: out_dtype = COMPLEX256; break;
        default:
            fprintf(stderr, "reciprocal: unsupported dtype %s\n", dtype_name(x->dtype));
            return NULL;
    }

    Array *res = create_array(x->shape, x->ndim, out_dtype);
    if (res == NULL) {
        fprintf(stderr, "reciprocal: failed to create result array\n");
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
            case BOOL:
            case INT8:   case INT16:  case INT32:  case INT64:
            case UINT8:  case UINT16: case UINT32: case UINT64: {
                double val;
                switch (x->dtype) {
                    case BOOL:   val = *(uint8_t*)pi;  break;
                    case INT8:   val = *(int8_t*)pi;   break;
                    case INT16:  val = *(int16_t*)pi;  break;
                    case INT32:  val = *(int32_t*)pi;  break;
                    case INT64:  val = *(int64_t*)pi;  break;
                    case UINT8:  val = *(uint8_t*)pi;  break;
                    case UINT16: val = *(uint16_t*)pi; break;
                    case UINT32: val = *(uint32_t*)pi; break;
                    case UINT64: val = *(uint64_t*)pi; break;
                    default: val = 0.0; break;
                }
                *(double*)po = (val == 0.0) ? INFINITY : 1.0 / val;
                break;
            }
            case FLOAT32: {
                float v = *(float*)pi;
                *(float*)po = (v == 0.0f) ? INFINITY : 1.0f / v;
                break;
            }
            case FLOAT64: {
                double v = *(double*)pi;
                *(double*)po = (v == 0.0) ? INFINITY : 1.0 / v;
                break;
            }
            case FLOAT128: {
                long double v = *(long double*)pi;
                *(long double*)po = (v == 0.0L) ? INFINITY : 1.0L / v;
                break;
            }
            case COMPLEX64: {
                complex float v = *(complex float*)pi;
                complex float one = 1.0f + 0.0f * I;
                *(complex float*)po = one / v;
                break;
            }
            case COMPLEX128: {
                complex double v = *(complex double*)pi;
                complex double one = 1.0 + 0.0 * I;
                *(complex double*)po = one / v;
                break;
            }
            case COMPLEX256: {
                complex long double v = *(complex long double*)pi;
                complex long double one = 1.0L + 0.0L * I;
                *(complex long double*)po = one / v;
                break;
            }
            default:
                break;
        }
    }
    return res;
}
