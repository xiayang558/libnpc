#include "array.h"
#include <math.h>

/**
 * npc_cbrt — Return cube root element-wise (like numpy.cbrt)
 *
 * Integer → FLOAT64 output
 * Float → same float output
 * Uses cbrt() from math.h for double, cbrtf() for float.
 * Negative inputs produce real negative roots.
 *
 * @param x Input array
 * @return Result array, or NULL on error
 */
Array* npc_cbrt(Array *x) {
    if (x == NULL) {
        fprintf(stderr, "cbrt: NULL array argument\n");
        return NULL;
    }

    DataType out_dtype;
    switch (x->dtype) {
        case BOOL:
        case INT8:   case INT16:  case INT32:  case INT64:
        case UINT8:  case UINT16: case UINT32: case UINT64:
            out_dtype = FLOAT64;
            break;
        case FLOAT32:  out_dtype = FLOAT32;  break;
        case FLOAT64:  out_dtype = FLOAT64;  break;
        case FLOAT128: out_dtype = FLOAT128; break;
        default:
            fprintf(stderr, "cbrt: unsupported dtype %s (complex not supported)\n",
                    dtype_name(x->dtype));
            return NULL;
    }

    Array *res = create_array(x->shape, x->ndim, out_dtype);
    if (res == NULL) {
        fprintf(stderr, "cbrt: failed to create result array\n");
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
                *(double*)po = cbrt(val);
                break;
            }
            case FLOAT32: {
                float v = *(float*)pi;
                *(float*)po = cbrtf(v);
                break;
            }
            case FLOAT64: {
                double v = *(double*)pi;
                *(double*)po = cbrt(v);
                break;
            }
            case FLOAT128: {
                long double v = *(long double*)pi;
                *(long double*)po = cbrtl(v);
                break;
            }
            default:
                break;
        }
    }

    return res;
}
