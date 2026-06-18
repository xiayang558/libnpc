#include "array.h"
#include "shape.h"


Array* ediff1d(Array *y, Array *to_begin, Array *to_end) {
    if (y == NULL) {
        fprintf(stderr, "ediff1d: NULL array argument\n");
        return NULL;
    }
    // flatten to 1D
    Array *flat = flatten(y);
    if (flat == NULL) return NULL;
    int n = flat->size;
    // compute output length
    int pre_len = (to_begin != NULL) ? to_begin->size : 0;
    int post_len = (to_end != NULL) ? to_end->size : 0;
    int diff_len = (n > 0) ? n - 1 : 0;
    int out_len = pre_len + diff_len + post_len;
    if (out_len < 0) out_len = 0;
    
    // determine output type: if original array is integer, diff can overflow, but NumPy keeps original type. we use the same type as flat.
    DataType out_dtype = flat->dtype;
    Array *res = create_array((int[]){out_len}, 1, out_dtype);
    if (res == NULL) {
        free_array(flat);
        return NULL;
    }
    
    size_t elem_sz = dtype_size(out_dtype);
    char *res_data = (char*)res->data;
    int pos = 0;
    
    // copy to_begin
    if (pre_len > 0) {
        // to_begin needs to be flattened
        Array *begin_flat = flatten(to_begin);
        if (begin_flat == NULL) {
            free_array(flat); free_array(res);
            return NULL;
        }
        // type cast to out_dtype
        if (begin_flat->dtype != out_dtype) {
            Array *tmp = astype(begin_flat, out_dtype);
            free_array(begin_flat);
            begin_flat = tmp;
            if (begin_flat == NULL) {
                free_array(flat); free_array(res);
                return NULL;
            }
        }
        memcpy(res_data + pos * elem_sz, begin_flat->data, pre_len * elem_sz);
        pos += pre_len;
        free_array(begin_flat);
    }
    
    // compute difference
    if (diff_len > 0) {
        char *flat_data = (char*)flat->data;
        for (int i = 0; i < diff_len; i++) {
            void *p1 = flat_data + (i+1) * elem_sz;
            void *p2 = flat_data + i * elem_sz;
            void *pout = res_data + (pos + i) * elem_sz;
            // we could use a generic subtract function, but compute directly by type here for simplicity, doing subtraction based on out_dtype
            switch (out_dtype) {
                case INT8: {
                    int8_t diff = *(int8_t*)p1 - *(int8_t*)p2;
                    memcpy(pout, &diff, elem_sz);
                    break;
                }
                case INT16: {
                    int16_t diff = *(int16_t*)p1 - *(int16_t*)p2;
                    memcpy(pout, &diff, elem_sz);
                    break;
                }
                case INT32: {
                    int32_t diff = *(int32_t*)p1 - *(int32_t*)p2;
                    memcpy(pout, &diff, elem_sz);
                    break;
                }
                case INT64: {
                    int64_t diff = *(int64_t*)p1 - *(int64_t*)p2;
                    memcpy(pout, &diff, elem_sz);
                    break;
                }
                case UINT8: {
                    uint8_t diff = *(uint8_t*)p1 - *(uint8_t*)p2;
                    memcpy(pout, &diff, elem_sz);
                    break;
                }
                case UINT16: {
                    uint16_t diff = *(uint16_t*)p1 - *(uint16_t*)p2;
                    memcpy(pout, &diff, elem_sz);
                    break;
                }
                case UINT32: {
                    uint32_t diff = *(uint32_t*)p1 - *(uint32_t*)p2;
                    memcpy(pout, &diff, elem_sz);
                    break;
                }
                case UINT64: {
                    uint64_t diff = *(uint64_t*)p1 - *(uint64_t*)p2;
                    memcpy(pout, &diff, elem_sz);
                    break;
                }
                case FLOAT32: {
                    float diff = *(float*)p1 - *(float*)p2;
                    memcpy(pout, &diff, elem_sz);
                    break;
                }
                case FLOAT64: {
                    double diff = *(double*)p1 - *(double*)p2;
                    memcpy(pout, &diff, elem_sz);
                    break;
                }
                case COMPLEX64: {
                    complex float diff = *(complex float*)p1 - *(complex float*)p2;
                    memcpy(pout, &diff, elem_sz);
                    break;
                }
                case COMPLEX128: {
                    complex double diff = *(complex double*)p1 - *(complex double*)p2;
                    memcpy(pout, &diff, elem_sz);
                    break;
                }
                default:
                    fprintf(stderr, "ediff1d: unsupported type\n");
                    free_array(flat); free_array(res);
                    return NULL;
            }
        }
        pos += diff_len;
    }
    
    // copy to_end
    if (post_len > 0) {
        Array *end_flat = flatten(to_end);
        if (end_flat == NULL) {
            free_array(flat); free_array(res);
            return NULL;
        }
        if (end_flat->dtype != out_dtype) {
            Array *tmp = astype(end_flat, out_dtype);
            free_array(end_flat);
            end_flat = tmp;
            if (end_flat == NULL) {
                free_array(flat); free_array(res);
                return NULL;
            }
        }
        memcpy(res_data + pos * elem_sz, end_flat->data, post_len * elem_sz);
        free_array(end_flat);
    }
    
    free_array(flat);
    return res;
}