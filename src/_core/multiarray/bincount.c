#include "def.h"
#include "array.h"

Array* bincount(Array *arr, Array *weights, int minlength) {
    if (arr == NULL) {
        fprintf(stderr, "bincount: NULL array argument\n");
        return NULL;
    }
    if (arr->ndim != 1) {
        fprintf(stderr, "bincount: arr must be 1-dimensional\n");
        return NULL;
    }
    if (arr->size == 0) {
        int shape[1] = {0};
        return create_array(shape, 1, INT64);
    }

    switch (arr->dtype) {
        case INT8:
        case INT16:
        case INT32:
        case INT64:
        case UINT8:
        case UINT16:
        case UINT32:
        case UINT64:
            break;
        default:
            fprintf(stderr, "bincount: a must be integer type\n");
            return NULL;
    }

    if (weights != NULL) {
        if (weights->ndim != 1) {
            fprintf(stderr, "bincount: weights must be 1-dimensional\n");
            return NULL;
        }
        if (weights->size != arr->size) {
            fprintf(stderr, "bincount: weights must have the same size as a\n");
            return NULL;
        }
    }

    int64_t max_val = -1;
    size_t elem_size_x = dtype_size(arr->dtype);
    char *x_data = (char*)arr->data;

    for (int i = 0; i < arr->size; i++) {
        void *ptr = x_data + i * elem_size_x;
        int64_t val;
        switch (arr->dtype) {
            case INT8:   val = *(int8_t*)ptr; break;
            case INT16:  val = *(int16_t*)ptr; break;
            case INT32:  val = *(int32_t*)ptr; break;
            case INT64:  val = *(int64_t*)ptr; break;
            case UINT8:  val = *(uint8_t*)ptr; break;
            case UINT16: val = *(uint16_t*)ptr; break;
            case UINT32: val = *(uint32_t*)ptr; break;
            case UINT64: val = *(uint64_t*)ptr; break;
            default:     val = 0; break;
        }
        if (val < 0) {
            fprintf(stderr, "bincount: a contains negative value %lld\n", (long long)val);
            return NULL;
        }
        if (val > max_val) max_val = val;
    }

    int out_len = (int)(max_val + 1);
    if (minlength > 0 && minlength > out_len) {
        out_len = minlength;
    }
    if (out_len < 0) {
        fprintf(stderr, "bincount: output length overflow\n");
        return NULL;
    }

    DataType out_dtype;
    if (weights == NULL) {
        out_dtype = INT64;
    } else {
        out_dtype = weights->dtype;
    }

    int shape[1] = {out_len};
    Array *result = create_array(shape, 1, out_dtype);
    if (result == NULL) {
        fprintf(stderr, "bincount: failed to create result array\n");
        return NULL;
    }

    size_t elem_size_out = dtype_size(out_dtype);
    char *res_data = (char*)result->data;
    if (weights == NULL) {
        memset(res_data, 0, out_len * elem_size_out);
    } else {
        for (int i = 0; i < out_len; i++) {
            void *ptr = res_data + i * elem_size_out;
            switch (out_dtype) {
                case INT8:    *(int8_t*)ptr = 0; break;
                case INT16:   *(int16_t*)ptr = 0; break;
                case INT32:   *(int32_t*)ptr = 0; break;
                case INT64:   *(int64_t*)ptr = 0; break;
                case UINT8:   *(uint8_t*)ptr = 0; break;
                case UINT16:  *(uint16_t*)ptr = 0; break;
                case UINT32:  *(uint32_t*)ptr = 0; break;
                case UINT64:  *(uint64_t*)ptr = 0; break;
                case FLOAT32: *(float*)ptr = 0.0f; break;
                case FLOAT64: *(double*)ptr = 0.0; break;
                case FLOAT128: *(long double*)ptr = 0.0L; break;
                case COMPLEX64: *(complex float*)ptr = 0.0f + 0.0f*I; break;
                case COMPLEX128: *(complex double*)ptr = 0.0 + 0.0*I; break;
                case COMPLEX256: *(complex long double*)ptr = 0.0L + 0.0L*I; break;
                default: break;
            }
        }
    }

    if (weights == NULL) {
        int64_t *counts = (int64_t*)res_data;
        for (int i = 0; i < arr->size; i++) {
            void *ptr = x_data + i * elem_size_x;
            int64_t idx;
            switch (arr->dtype) {
                case INT8:   idx = *(int8_t*)ptr; break;
                case INT16:  idx = *(int16_t*)ptr; break;
                case INT32:  idx = *(int32_t*)ptr; break;
                case INT64:  idx = *(int64_t*)ptr; break;
                case UINT8:  idx = *(uint8_t*)ptr; break;
                case UINT16: idx = *(uint16_t*)ptr; break;
                case UINT32: idx = *(uint32_t*)ptr; break;
                case UINT64: idx = *(uint64_t*)ptr; break;
                default:     idx = 0; break;
            }
            counts[idx]++;
        }
    } else {
        size_t elem_size_w = dtype_size(weights->dtype);
        char *w_data = (char*)weights->data;

        for (int i = 0; i < arr->size; i++) {
            void *x_ptr = x_data + i * elem_size_x;
            int64_t idx;
            switch (arr->dtype) {
                case INT8:   idx = *(int8_t*)x_ptr; break;
                case INT16:  idx = *(int16_t*)x_ptr; break;
                case INT32:  idx = *(int32_t*)x_ptr; break;
                case INT64:  idx = *(int64_t*)x_ptr; break;
                case UINT8:  idx = *(uint8_t*)x_ptr; break;
                case UINT16: idx = *(uint16_t*)x_ptr; break;
                case UINT32: idx = *(uint32_t*)x_ptr; break;
                case UINT64: idx = *(uint64_t*)x_ptr; break;
                default:     idx = 0; break;
            }

            void *w_ptr = w_data + i * elem_size_w;
            void *res_ptr = res_data + idx * elem_size_out;

            switch (out_dtype) {
                case INT8:    *(int8_t*)res_ptr += *(int8_t*)w_ptr; break;
                case INT16:   *(int16_t*)res_ptr += *(int16_t*)w_ptr; break;
                case INT32:   *(int32_t*)res_ptr += *(int32_t*)w_ptr; break;
                case INT64:   *(int64_t*)res_ptr += *(int64_t*)w_ptr; break;
                case UINT8:   *(uint8_t*)res_ptr += *(uint8_t*)w_ptr; break;
                case UINT16:  *(uint16_t*)res_ptr += *(uint16_t*)w_ptr; break;
                case UINT32:  *(uint32_t*)res_ptr += *(uint32_t*)w_ptr; break;
                case UINT64:  *(uint64_t*)res_ptr += *(uint64_t*)w_ptr; break;
                case FLOAT32: *(float*)res_ptr += *(float*)w_ptr; break;
                case FLOAT64: *(double*)res_ptr += *(double*)w_ptr; break;
                case FLOAT128: *(long double*)res_ptr += *(long double*)w_ptr; break;
                case COMPLEX64: *(complex float*)res_ptr += *(complex float*)w_ptr; break;
                case COMPLEX128: *(complex double*)res_ptr += *(complex double*)w_ptr; break;
                case COMPLEX256: *(complex long double*)res_ptr += *(complex long double*)w_ptr; break;
                default: break;
            }
        }
    }

    return result;
}
