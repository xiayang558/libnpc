#include "array.h"
#include "def.h"
#include "utils.h"

Array* msort(Array *a) {
    if (a == NULL) {
        fprintf(stderr, "msort: NULL array argument\n");
        return NULL;
    }
    int ndim = a->ndim;
    if (ndim == 0) { // scalar, return a copy directly
        return copy_array(a);
    }
    int axis_len = a->shape[0];
    if (axis_len <= 1) {
        return copy_array(a);
    }

    // same shape as output
    Array *res = create_array(a->shape, ndim, a->dtype);
    if (!res) return NULL;

    // compute strides (in elements)
    int *strides = malloc(ndim * sizeof(int));
    if (!strides) { free_array(res); return NULL; }
    compute_strides(a->shape, ndim, strides);
    size_t elem_sz = dtype_size(a->dtype);
    char *src = (char*)a->data;
    char *dst = (char*)res->data;

    // outer ndim combo (excluding the first axis)
    int outer_ndim = ndim - 1;
    int *outer_dims = malloc(outer_ndim * sizeof(int));
    int *outer_shape = malloc(outer_ndim * sizeof(int));
    int *outer_indices = calloc(outer_ndim, sizeof(int));
    if (!outer_dims || !outer_shape || !outer_indices) {
        free(strides); free(outer_dims); free(outer_shape); free(outer_indices);
        free_array(res);
        return NULL;
    }
    for (int i = 1; i < ndim; ++i) {
        outer_dims[i-1] = i;
        outer_shape[i-1] = a->shape[i];
    }
    int num_slices = 1;
    for (int i = 0; i < outer_ndim; ++i) num_slices *= outer_shape[i];

    // temporary storage for each column's data
    void *col_data = malloc(axis_len * elem_sz);
    if (!col_data) {
        free(strides); free(outer_dims); free(outer_shape); free(outer_indices);
        free_array(res);
        return NULL;
    }

    for (int slice = 0; slice < num_slices; ++slice) {
        // compute base offset for current slice in input (skip first axis)
        int base = 0;
        for (int j = 0; j < outer_ndim; ++j) {
            base += outer_indices[j] * strides[outer_dims[j]];
        }
        // extract this column's elements to temporary buffer
        for (int k = 0; k < axis_len; ++k) {
            void *src_ptr = src + (base + k * strides[0]) * elem_sz;
            memcpy((char*)col_data + k * elem_sz, src_ptr, elem_sz);
        }
        // sort
        switch (a->dtype) {
            case INT8: {
                int8_t *data = (int8_t*)col_data;
                qsort(data, axis_len, elem_sz, cmp_int64);
                break;
            }
            case INT16: {
                int16_t *data = (int16_t*)col_data;
                qsort(data, axis_len, elem_sz, cmp_int64);
                break;
            }
            case INT32: {
                int32_t *data = (int32_t*)col_data;
                qsort(data, axis_len, elem_sz, cmp_int64);
                break;
            }
            case INT64: {
                int64_t *data = (int64_t*)col_data;
                qsort(data, axis_len, elem_sz, cmp_int64);
                break;
            }
            case UINT8: {
                uint8_t *data = (uint8_t*)col_data;
                qsort(data, axis_len, elem_sz, cmp_uint64);
                break;
            }
            case UINT16: {
                uint16_t *data = (uint16_t*)col_data;
                qsort(data, axis_len, elem_sz, cmp_uint64);
                break;
            }
            case UINT32: {
                uint32_t *data = (uint32_t*)col_data;
                qsort(data, axis_len, elem_sz, cmp_uint64);
                break;
            }
            case UINT64: {
                uint64_t *data = (uint64_t*)col_data;
                qsort(data, axis_len, elem_sz, cmp_uint64);
                break;
            }
            case FLOAT32: {
                float *data = (float*)col_data;
                qsort(data, axis_len, elem_sz, cmp_float);
                break;
            }
            case FLOAT64: {
                double *data = (double*)col_data;
                qsort(data, axis_len, elem_sz, cmp_double);
                break;
            }
            case COMPLEX64: {
                complex float *data = (complex float*)col_data;
                qsort(data, axis_len, elem_sz, cmp_complex64);
                break;
            }
            case COMPLEX128: {
                complex double *data = (complex double*)col_data;
                qsort(data, axis_len, elem_sz, cmp_complex128);
                break;
            }
            default:
                fprintf(stderr, "msort: unsupported data type\n");
                free(col_data); free(strides); free(outer_dims); free(outer_shape); free(outer_indices);
                free_array(res);
                return NULL;
        }
        // write back to result
        for (int k = 0; k < axis_len; ++k) {
            void *dst_ptr = dst + (base + k * strides[0]) * elem_sz;
            memcpy(dst_ptr, (char*)col_data + k * elem_sz, elem_sz);
        }
        // update outer_indices
        int carry = 1;
        for (int j = outer_ndim - 1; j >= 0 && carry; --j) {
            outer_indices[j]++;
            if (outer_indices[j] >= outer_shape[j]) {
                outer_indices[j] = 0;
                carry = 1;
            } else {
                carry = 0;
            }
        }
    }

    free(strides);
    free(outer_dims);
    free(outer_shape);
    free(outer_indices);
    free(col_data);
    return res;
}