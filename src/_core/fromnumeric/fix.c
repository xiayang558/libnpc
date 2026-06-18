#include "array.h"


Array* fix(Array *arr) {
    if (arr == NULL) {
        fprintf(stderr, "fix: NULL array argument\n");
        return NULL;
    }
    // Create result array, same shape and dtype as input
    Array *res = create_array(arr->shape, arr->ndim, arr->dtype);
    if (res == NULL) return NULL;
    size_t elem_sz = dtype_size(arr->dtype);
    char *src = (char*)arr->data;
    char *dst = (char*)res->data;

    for (int i = 0; i < arr->size; i++) {
        void *p_src = src + i * elem_sz;
        void *p_dst = dst + i * elem_sz;
        switch (arr->dtype) {
            case INT8: case INT16: case INT32: case INT64:
            case UINT8: case UINT16: case UINT32: case UINT64:
                // integer: direct copy
                memcpy(p_dst, p_src, elem_sz);
                break;
            case FLOAT32: {
                float val = *(float*)p_src;
                *(float*)p_dst = truncf(val);
                break;
            }
            case FLOAT64: {
                double val = *(double*)p_src;
                *(double*)p_dst = trunc(val);
                break;
            }
            case FLOAT128: {
                long double val = *(long double*)p_src;
                *(long double*)p_dst = truncl(val);
                break;
            }
            case COMPLEX64: {
                complex float val = *(complex float*)p_src;
                float real = truncf(crealf(val));
                float imag = truncf(cimagf(val));
                *(complex float*)p_dst = real + imag * I;
                break;
            }
            case COMPLEX128: {
                complex double val = *(complex double*)p_src;
                double real = trunc(creal(val));
                double imag = trunc(cimag(val));
                *(complex double*)p_dst = real + imag * I;
                break;
            }
            default:
                fprintf(stderr, "fix: unsupported data type %s\n", dtype_name(arr->dtype));
                free_array(res);
                return NULL;
        }
    }
    return res;
}