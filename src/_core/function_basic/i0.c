#include "array.h"


Array* i0(Array *arr) {
    if (arr == NULL) {
        fprintf(stderr, "i0: NULL array argument\n");
        return NULL;
    }
    // output type is always FLOAT64
    Array *res = create_array(arr->shape, arr->ndim, FLOAT64);
    if (res == NULL) return NULL;

    size_t in_sz = dtype_size(arr->dtype);
    size_t out_sz = dtype_size(FLOAT64);
    char *in_data = (char*)arr->data;
    char *out_data = (char*)res->data;

    for (int i = 0; i < arr->size; ++i) {
        void *pin = in_data + i * in_sz;
        double val = 0.0;
        switch (arr->dtype) {
            case INT8:   val = *(int8_t*)pin; break;
            case INT16:  val = *(int16_t*)pin; break;
            case INT32:  val = *(int32_t*)pin; break;
            case INT64:  val = (double)*(int64_t*)pin; break;
            case UINT8:  val = *(uint8_t*)pin; break;
            case UINT16: val = *(uint16_t*)pin; break;
            case UINT32: val = *(uint32_t*)pin; break;
            case UINT64: val = (double)*(uint64_t*)pin; break;
            case FLOAT32: val = *(float*)pin; break;
            case FLOAT64: val = *(double*)pin; break;
            default:
                fprintf(stderr, "i0: unsupported data type %s\n", dtype_name(arr->dtype));
                free_array(res);
                return NULL;
        }
        double result = bessel_i0(val);
        memcpy(out_data + i * out_sz, &result, out_sz);
    }
    return res;
}
