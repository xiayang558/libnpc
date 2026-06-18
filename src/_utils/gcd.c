#include <stdlib.h>
#include <string.h>
#include "array.h"
#include "def.h"
#include "function.h"
#include "utils.h"

/** gcd for unsigned integers, Euclidean algorithm */
static uint64_t gcd_uint64(uint64_t a, uint64_t b) {
    while (b) { uint64_t t = b; b = a % b; a = t; }
    return a;
}

Array* gcd_array(Array *x1, Array *x2) {
    if (x1 == NULL || x2 == NULL) {
        fprintf(stderr, "gcd: NULL argument\n"); return NULL;
    }
    if (x1->dtype != x2->dtype) {
        fprintf(stderr, "gcd: data type mismatch\n"); return NULL;
    }
    /* Only integer types */
    if (!is_integer(x1->dtype)) {
        fprintf(stderr, "gcd: only integer types supported\n"); return NULL;
    }

    int out_ndim;
    int *out_shape = broadcast_shapes(2, (Array*[]){x1, x2}, &out_ndim);
    if (!out_shape) return NULL;

    int in_ndim1 = x1->ndim, in_ndim2 = x2->ndim;
    Array *result = create_array(out_shape, out_ndim, x1->dtype);
    if (!result) { free(out_shape); return NULL; }
    free(out_shape);

    size_t elem_sz = dtype_size(x1->dtype);
    char *data1 = (char*)x1->data, *data2 = (char*)x2->data;
    char *res_data = (char*)result->data;
    int *strides1 = malloc(in_ndim1 * sizeof(int));
    int *strides2 = malloc(in_ndim2 * sizeof(int));
    int *res_strides = malloc(result->ndim * sizeof(int));
    if (in_ndim1) compute_strides(x1->shape, in_ndim1, strides1);
    if (in_ndim2) compute_strides(x2->shape, in_ndim2, strides2);
    if (result->ndim) compute_strides(result->shape, result->ndim, res_strides);

    int *indices = result->ndim ? calloc(result->ndim, sizeof(int)) : NULL;
    for (int flat = 0; flat < result->size; ++flat) {
        if (result->ndim) {
            int tmp = flat;
            for (int i = result->ndim - 1; i >= 0; --i) {
                indices[i] = tmp % result->shape[i]; tmp /= result->shape[i];
            }
        }
        int off1 = 0, off2 = 0;
        if (result->ndim) {
            int doff = result->ndim - in_ndim1;
            for (int i = 0; i < in_ndim1; ++i) {
                int c = indices[doff + i]; if (x1->shape[i] == 1) c = 0;
                off1 += c * strides1[i];
            }
            doff = result->ndim - in_ndim2;
            for (int i = 0; i < in_ndim2; ++i) {
                int c = indices[doff + i]; if (x2->shape[i] == 1) c = 0;
                off2 += c * strides2[i];
            }
        }
        void *p1 = data1 + off1 * elem_sz, *p2 = data2 + off2 * elem_sz;
        void *pdst = res_data + flat * elem_sz;

        int64_t v1 = 0, v2 = 0;
        switch (x1->dtype) {
            case INT8:  v1 = *(int8_t*)p1; v2 = *(int8_t*)p2; break;
            case UINT8: v1 = *(uint8_t*)p1; v2 = *(uint8_t*)p2; break;
            case INT16: v1 = *(int16_t*)p1; v2 = *(int16_t*)p2; break;
            case UINT16: v1 = *(uint16_t*)p1; v2 = *(uint16_t*)p2; break;
            case INT32: v1 = *(int32_t*)p1; v2 = *(int32_t*)p2; break;
            case UINT32: v1 = *(uint32_t*)p1; v2 = *(uint32_t*)p2; break;
            case INT64: v1 = *(int64_t*)p1; v2 = *(int64_t*)p2; break;
            case UINT64: {
                uint64_t v = gcd_uint64(*(uint64_t*)p1, *(uint64_t*)p2);
                memcpy(pdst, &v, elem_sz); continue;
            }
            default: break;
        }
        /* Use gcd_int64 which handles signs */
        int64_t g = gcd_int64(v1, v2);
        switch (x1->dtype) {
            case INT8: { int8_t r = (int8_t)g; memcpy(pdst, &r, elem_sz); break; }
            case UINT8: { uint8_t r = (uint8_t)g; memcpy(pdst, &r, elem_sz); break; }
            case INT16: { int16_t r = (int16_t)g; memcpy(pdst, &r, elem_sz); break; }
            case UINT16: { uint16_t r = (uint16_t)g; memcpy(pdst, &r, elem_sz); break; }
            case INT32: { int32_t r = (int32_t)g; memcpy(pdst, &r, elem_sz); break; }
            case UINT32: { uint32_t r = (uint32_t)g; memcpy(pdst, &r, elem_sz); break; }
            case INT64: { int64_t r = g; memcpy(pdst, &r, elem_sz); break; }
            default: break;
        }
    }
    free(strides1); free(strides2); free(res_strides);
    if (indices) free(indices);
    return result;
}
