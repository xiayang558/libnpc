#include "def.h"
#include "array.h"
#include "shape.h"
#include "function.h"
#include "utils.h"


Array* insert(Array *arr, Array *obj, Array *values, int axis) {
    if (arr == NULL || obj == NULL || values == NULL) {
        fprintf(stderr, "insert: NULL argument\n");
        return NULL;
    }
    int use_flat = (axis == -1);
    int ndim = arr->ndim;
    if (!use_flat && (axis < 0 || axis >= ndim)) {
        fprintf(stderr, "insert: axis %d out of bounds for %dD array\n", axis, ndim);
        return NULL;
    }
    
    // prepare insert indices
    int max_idx;
    if (use_flat) max_idx = arr->size;
    else max_idx = arr->shape[axis];
    int num_indices;
    int *indices = prepare_indices(obj, &num_indices, max_idx);
    if (indices == NULL) return NULL;
    
    // prepare values array
    Array *vals = prepare_values(values, num_indices);
    if (vals == NULL) {
        free(indices);
        return NULL;
    }
    
    // determine output shape
    int *out_shape = NULL;
    int out_ndim;
    if (use_flat) {
        out_ndim = 1;
        out_shape = malloc(sizeof(int));
        out_shape[0] = arr->size + num_indices;
    } else {
        out_ndim = ndim;
        out_shape = malloc(ndim * sizeof(int));
        memcpy(out_shape, arr->shape, ndim * sizeof(int));
        out_shape[axis] = arr->shape[axis] + num_indices;
    }
    if (!out_shape) {
        free(indices); free_array(vals);
        return NULL;
    }
    Array *res = create_array(out_shape, out_ndim, arr->dtype);
    free(out_shape);
    if (!res) {
        free(indices); free_array(vals);
        return NULL;
    }
    
    size_t elem_sz = dtype_size(arr->dtype);
    char *src = (char*)arr->data;
    char *dst = (char*)res->data;
    char *val_data = (char*)vals->data;
    size_t val_sz = dtype_size(vals->dtype);
    // type must match
    if (vals->dtype != arr->dtype) {
        fprintf(stderr, "insert: value type does not match array type\n");
        free(indices); free_array(vals); free_array(res);
        return NULL;
    }
    
    if (use_flat) {
        int src_pos = 0, dst_pos = 0;
        int next_idx = 0;
        for (int i = 0; i <= arr->size; ++i) {
            if (next_idx < num_indices && indices[next_idx] == i) {
                memcpy(dst + dst_pos * elem_sz, val_data + next_idx * val_sz, elem_sz);
                dst_pos++;
                next_idx++;
            }
            if (i < arr->size) {
                memcpy(dst + dst_pos * elem_sz, src + src_pos * elem_sz, elem_sz);
                src_pos++;
                dst_pos++;
            }
        }
    } else {
        int axis_len = arr->shape[axis];
        int *strides = malloc(ndim * sizeof(int));
        int *out_strides = malloc(ndim * sizeof(int));
        compute_strides(arr->shape, ndim, strides);
        compute_strides(res->shape, ndim, out_strides);
        if (!strides || !out_strides) {
            free(strides); free(out_strides);
            free(indices); free_array(vals); free_array(res);
            return NULL;
        }
        
        // combinations of outer dims (all except axis)
        int outer_ndim = ndim - 1;
        int *outer_dims = malloc(outer_ndim * sizeof(int));
        int *outer_shape = malloc(outer_ndim * sizeof(int));
        int *outer_indices = calloc(outer_ndim, sizeof(int));
        if (!outer_dims || !outer_shape || !outer_indices) {
            free(outer_dims); free(outer_shape); free(outer_indices);
            free(strides); free(out_strides);
            free(indices); free_array(vals); free_array(res);
            return NULL;
        }
        int idx = 0;
        for (int i = 0; i < ndim; ++i) {
            if (i != axis) {
                outer_dims[idx] = i;
                outer_shape[idx] = arr->shape[i];
                idx++;
            }
        }
        int num_slices = 1;
        for (int i = 0; i < outer_ndim; ++i) num_slices *= outer_shape[i];
        
        for (int slice = 0; slice < num_slices; ++slice) {
            int base_in = 0, base_out = 0;
            for (int j = 0; j < outer_ndim; ++j) {
                base_in  += outer_indices[j] * strides[outer_dims[j]];
                base_out += outer_indices[j] * out_strides[outer_dims[j]];
            }
            int src_k = 0, dst_k = 0;
            int next_idx = 0;
            for (int k = 0; k <= axis_len; ++k) {
                if (next_idx < num_indices && indices[next_idx] == k) {
                    void *val_ptr = val_data + next_idx * val_sz;
                    void *dst_ptr = dst + (base_out + dst_k * out_strides[axis]) * elem_sz;
                    memcpy(dst_ptr, val_ptr, elem_sz);
                    dst_k++;
                    next_idx++;
                }
                if (k < axis_len) {
                    void *src_ptr = src + (base_in + src_k * strides[axis]) * elem_sz;
                    void *dst_ptr = dst + (base_out + dst_k * out_strides[axis]) * elem_sz;
                    memcpy(dst_ptr, src_ptr, elem_sz);
                    src_k++;
                    dst_k++;
                }
            }
            // updateouterndimindex
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
            if (carry) break;
        }
        free(strides); free(out_strides);
        free(outer_dims); free(outer_shape); free(outer_indices);
    }
    
    free(indices);
    free_array(vals);
    return res;
}