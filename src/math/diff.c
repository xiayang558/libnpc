#include "array.h"


Array* diff(Array *arr, int n, int axis) {
    if (arr == NULL) {
        fprintf(stderr, "diff: NULL array argument\n");
        return NULL;
    }
    if (n < 0) {
        fprintf(stderr, "diff: n must be non-negative\n");
        return NULL;
    }
    // handle axis parameter
    int ax = axis;
    if (ax == -1) ax = arr->ndim - 1;
    if (ax < 0 || ax >= arr->ndim) {
        fprintf(stderr, "diff: axis %d out of bounds for %dD array\n", axis, arr->ndim);
        return NULL;
    }
    int axis_len = arr->shape[ax];
    if (n == 0) {
        // return a copy of this
        return copy_array(arr);
    }
    if (n >= axis_len) {
        // return array with length 0 along this axis
        int *out_shape = malloc(arr->ndim * sizeof(int));
        if (!out_shape) return NULL;
        memcpy(out_shape, arr->shape, arr->ndim * sizeof(int));
        out_shape[ax] = 0;
        Array *result = create_array(out_shape, arr->ndim, arr->dtype);
        free(out_shape);
        return result;
    }
    int new_len = axis_len - n;
    int *out_shape = malloc(arr->ndim * sizeof(int));
    if (!out_shape) return NULL;
    memcpy(out_shape, arr->shape, arr->ndim * sizeof(int));
    out_shape[ax] = new_len;
    Array *result = create_array(out_shape, arr->ndim, arr->dtype);
    free(out_shape);
    if (result == NULL) return NULL;

    // compute strides
    int *in_strides = malloc(arr->ndim * sizeof(int));
    int *out_strides = malloc(arr->ndim * sizeof(int));
    if (!in_strides || !out_strides) {
        free(in_strides); free(out_strides);
        free_array(result);
        return NULL;
    }
    compute_strides(arr->shape, arr->ndim, in_strides);
    compute_strides(result->shape, arr->ndim, out_strides);

    size_t elem_sz = dtype_size(arr->dtype);
    char *in_data = (char*)arr->data;
    char *out_data = (char*)result->data;

    // outer dimensions
    int outer_ndim = arr->ndim - 1;
    int *outer_dims = malloc(outer_ndim * sizeof(int));
    int *outer_shape = malloc(outer_ndim * sizeof(int));
    int *outer_indices = calloc(outer_ndim, sizeof(int));
    if (!outer_dims || !outer_shape || !outer_indices) {
        free(in_strides); free(out_strides);
        free(outer_dims); free(outer_shape); free(outer_indices);
        free_array(result);
        return NULL;
    }
    int idx = 0;
    for (int i = 0; i < arr->ndim; i++) {
        if (i != ax) {
            outer_dims[idx] = i;
            outer_shape[idx] = arr->shape[i];
            idx++;
        }
    }
    int num_slices = 1;
    for (int i = 0; i < outer_ndim; i++) num_slices *= outer_shape[i];

    // temporary buffers
    size_t temp_sz = axis_len * elem_sz;
    char *temp = malloc(temp_sz);
    char *temp2 = malloc(temp_sz);
    if (!temp || !temp2) {
        free(temp); free(temp2);
        free(in_strides); free(out_strides);
        free(outer_dims); free(outer_shape); free(outer_indices);
        free_array(result);
        return NULL;
    }

    for (int slice = 0; slice < num_slices; slice++) {
        // get base offset of current slice in input
        int base_in = 0;
        for (int j = 0; j < outer_ndim; j++) {
            base_in += outer_indices[j] * in_strides[outer_dims[j]];
        }
        // copy current slice data along axis to temp
        for (int k = 0; k < axis_len; k++) {
            int off = base_in + k * in_strides[ax];
            memcpy(temp + k * elem_sz, in_data + off * elem_sz, elem_sz);
        }
        // compute n-th order difference
        char *src = temp;
        char *dst = temp2;
        int cur_len = axis_len;
        for (int step = 0; step < n; step++) {
            for (int i = 0; i < cur_len - 1; i++) {
                void *p1 = src + (i+1) * elem_sz;
                void *p2 = src + i * elem_sz;
                void *pout = dst + i * elem_sz;
                switch (arr->dtype) {
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
                        break;
                }
            }
            cur_len--;
            // swap src and dst
            char *tmp = src; src = dst; dst = tmp;
        }
        // write result to output slice
        int base_out = 0;
        for (int j = 0; j < outer_ndim; j++) {
            base_out += outer_indices[j] * out_strides[outer_dims[j]];
        }
        for (int k = 0; k < new_len; k++) {
            int off = base_out + k * out_strides[ax];
            memcpy(out_data + off * elem_sz, src + k * elem_sz, elem_sz);
        }
        // update outer dimension index
        int carry = 1;
        for (int j = outer_ndim - 1; j >= 0 && carry; j--) {
            outer_indices[j]++;
            if (outer_indices[j] >= outer_shape[j]) {
                outer_indices[j] = 0;
                carry = 1;
            } else {
                carry = 0;
            }
        }
        if (carry) break;
    }

    free(temp);
    free(temp2);
    free(in_strides);
    free(out_strides);
    free(outer_dims);
    free(outer_shape);
    free(outer_indices);
    return result;
}