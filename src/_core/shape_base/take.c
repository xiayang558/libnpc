#include "def.h"
#include "array.h"
#include "shape.h"
#include <string.h>

/**
 * take — extract elements along an axis (like numpy.take)
 *
 * @param arr     Input array
 * @param indices Integer index array
 * @param axis    Axis to take from, -1 flattens first
 * @return        New array with selected elements
 */
Array* take(Array *arr, Array *indices, int axis) {
    if (arr == NULL || indices == NULL) {
        fprintf(stderr, "take: NULL argument\n");
        return NULL;
    }

    /* Validate indices dtype */
    DataType idt = indices->dtype;
    if (!(idt == INT8 || idt == INT16 || idt == INT32 || idt == INT64 ||
          idt == UINT8 || idt == UINT16 || idt == UINT32 || idt == UINT64)) {
        fprintf(stderr, "take: indices must be integer type\n");
        return NULL;
    }

    if (axis == -1) {
        /* Flatten then index; output shape = indices shape */
        int out_ndim = indices->ndim;
        int *out_shape = malloc(out_ndim * sizeof(int));
        if (!out_shape) return NULL;
        memcpy(out_shape, indices->shape, out_ndim * sizeof(int));

        Array *res = create_array(out_shape, out_ndim, arr->dtype);
        free(out_shape);
        if (!res) return NULL;

        size_t elem_sz = dtype_size(arr->dtype);
        char *src = (char*)arr->data;
        char *dst = (char*)res->data;
        char *idx_data = (char*)indices->data;
        size_t idx_sz = dtype_size(idt);
        int na = arr->size;

        for (int i = 0; i < indices->size; i++) {
            int64_t idx_val;
            void *ip = idx_data + i * idx_sz;
            switch (idt) {
                case INT8:  idx_val = *(int8_t*)ip; break;
                case INT16: idx_val = *(int16_t*)ip; break;
                case INT32: idx_val = *(int32_t*)ip; break;
                case INT64: idx_val = *(int64_t*)ip; break;
                case UINT8: idx_val = *(uint8_t*)ip; break;
                case UINT16:idx_val = *(uint16_t*)ip; break;
                case UINT32:idx_val = *(uint32_t*)ip; break;
                case UINT64:idx_val = *(uint64_t*)ip; break;
                default:     idx_val = 0;
            }
            /* Wrap negative indices */
            if (idx_val < 0) idx_val = idx_val % na + na;
            if (idx_val < 0 || idx_val >= na) idx_val = idx_val % na;

            memcpy(dst + i * elem_sz, src + idx_val * elem_sz, elem_sz);
        }
        return res;
    }

    /* Axis-specific */
    int ndim = arr->ndim;
    if (axis < 0) axis += ndim;
    if (axis < 0 || axis >= ndim) {
        fprintf(stderr, "take: axis %d out of bounds for %d-d array\n", axis, ndim);
        return NULL;
    }

    int axis_len = arr->shape[axis];

    /* Build output shape: replace axis dim with indices shape */
    int out_ndim = (ndim - 1) + indices->ndim;
    int *out_shape = malloc(out_ndim * sizeof(int));
    if (!out_shape) return NULL;

    /* Copy axes before 'axis' */
    for (int i = 0; i < axis; i++) out_shape[i] = arr->shape[i];
    /* Insert indices shape */
    for (int i = 0; i < indices->ndim; i++) out_shape[axis + i] = indices->shape[i];
    /* Copy axes after 'axis' */
    for (int i = axis + 1; i < ndim; i++) out_shape[indices->ndim + i - 1] = arr->shape[i];

    Array *res = create_array(out_shape, out_ndim, arr->dtype);
    free(out_shape);
    if (!res) return NULL;

    size_t elem_sz = dtype_size(arr->dtype);
    char *src = (char*)arr->data;
    char *dst = (char*)res->data;
    char *idx_data = (char*)indices->data;
    size_t idx_sz = dtype_size(idt);

    /* Compute strides */
    int *src_strides = malloc(ndim * sizeof(int));
    int *res_strides = malloc(out_ndim * sizeof(int));
    int *idx_strides = malloc(indices->ndim * sizeof(int));
    int *out_indices = calloc(out_ndim, sizeof(int));
    if (!src_strides || !res_strides || !idx_strides || !out_indices) {
        free(src_strides); free(res_strides); free(idx_strides); free(out_indices);
        free_array(res); return NULL;
    }
    compute_strides(arr->shape, ndim, src_strides);
    compute_strides(res->shape, out_ndim, res_strides);
    compute_strides(indices->shape, indices->ndim, idx_strides);

    for (int flat = 0; flat < res->size; flat++) {
        /* Decode output index */
        int tmp = flat;
        for (int i = out_ndim - 1; i >= 0; i--) {
            out_indices[i] = tmp % res->shape[i];
            tmp /= res->shape[i];
        }

        /* Extract index value from the indices array */
        size_t idx_off = 0;
        for (int i = 0; i < indices->ndim; i++) {
            idx_off += out_indices[axis + i] * idx_strides[i];
        }
        int64_t idx_val;
        void *ip = idx_data + idx_off * idx_sz;
        switch (idt) {
            case INT8:  idx_val = *(int8_t*)ip; break;
            case INT16: idx_val = *(int16_t*)ip; break;
            case INT32: idx_val = *(int32_t*)ip; break;
            case INT64: idx_val = *(int64_t*)ip; break;
            case UINT8: idx_val = *(uint8_t*)ip; break;
            case UINT16:idx_val = *(uint16_t*)ip; break;
            case UINT32:idx_val = *(uint32_t*)ip; break;
            case UINT64:idx_val = *(uint64_t*)ip; break;
            default:     idx_val = 0;
        }
        /* Wrap */
        if (idx_val < 0) { idx_val = idx_val % axis_len; if (idx_val < 0) idx_val += axis_len; }
        if (idx_val >= axis_len) idx_val = idx_val % axis_len;

        /* Compute source offset:
         * axes before 'axis' → from out_indices
         * axis position → idx_val
         * axes after 'axis' → from out_indices after indices block */
        size_t src_off = 0;
        for (int i = 0; i < axis; i++) src_off += out_indices[i] * src_strides[i];
        src_off += idx_val * src_strides[axis];
        for (int i = axis + 1; i < ndim; i++)
            src_off += out_indices[indices->ndim + i - 1] * src_strides[i];

        /* Compute dest offset */
        size_t dst_off = 0;
        for (int i = 0; i < out_ndim; i++) dst_off += out_indices[i] * res_strides[i];

        memcpy(dst + dst_off * elem_sz, src + src_off * elem_sz, elem_sz);
    }

    free(src_strides); free(res_strides); free(idx_strides); free(out_indices);
    return res;
}
