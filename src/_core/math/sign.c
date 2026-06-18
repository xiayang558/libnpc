#include "array.h"
#include <math.h>
#include <string.h>

/**
 * sign  --  Returnarrayeach elementsign (like numpy.sign)
 *
 * Returns same dtype as input, value range {-1, 0, 1}
 */
Array* sign(Array *arr) {
    if (arr == NULL) {
        fprintf(stderr, "sign: NULL array argument\n");
        return NULL;
    }

    DataType out_dtype = arr->dtype;
    Array *res = create_array(arr->shape, arr->ndim, out_dtype);
    if (res == NULL) return NULL;

    size_t sz = dtype_size(arr->dtype);
    char *in = (char*)arr->data, *out = (char*)res->data;

    for (int i = 0; i < arr->size; i++) {
        void *pi = in + i * sz, *po = out + i * sz;

        switch (arr->dtype) {
            case BOOL:
                *(uint8_t*)po = (*(uint8_t*)pi) ? 1 : 0;
                break;
            case INT8: {
                int8_t v = *(int8_t*)pi;
                *(int8_t*)po = (v > 0) ? 1 : ((v < 0) ? -1 : 0);
                break;
            }
            case INT16: {
                int16_t v = *(int16_t*)pi;
                *(int16_t*)po = (v > 0) ? 1 : ((v < 0) ? -1 : 0);
                break;
            }
            case INT32: {
                int32_t v = *(int32_t*)pi;
                *(int32_t*)po = (v > 0) ? 1 : ((v < 0) ? -1 : 0);
                break;
            }
            case INT64: {
                int64_t v = *(int64_t*)pi;
                *(int64_t*)po = (v > 0) ? 1 : ((v < 0) ? -1 : 0);
                break;
            }
            case UINT8: {
                uint8_t v = *(uint8_t*)pi;
                *(uint8_t*)po = (v > 0) ? 1 : 0;
                break;
            }
            case UINT16: {
                uint16_t v = *(uint16_t*)pi;
                *(uint16_t*)po = (v > 0) ? 1 : 0;
                break;
            }
            case UINT32: {
                uint32_t v = *(uint32_t*)pi;
                *(uint32_t*)po = (v > 0) ? 1 : 0;
                break;
            }
            case UINT64: {
                uint64_t v = *(uint64_t*)pi;
                *(uint64_t*)po = (v > 0) ? 1 : 0;
                break;
            }
            case FLOAT32: {
                float v = *(float*)pi;
                if (v > 0) *(float*)po = 1.0f;
                else if (v < 0) *(float*)po = -1.0f;
                else *(float*)po = (v == 0) ? 0.0f : v; /* NaN stays NaN */
                break;
            }
            case FLOAT64: {
                double v = *(double*)pi;
                if (v > 0) *(double*)po = 1.0;
                else if (v < 0) *(double*)po = -1.0;
                else *(double*)po = (v == 0) ? 0.0 : v; /* NaN stays NaN */
                break;
            }
            default:
                fprintf(stderr, "sign: unsupported dtype\n");
                free_array(res);
                return NULL;
        }
    }
    return res;
}
