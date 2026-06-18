#include "array.h"
#include "shape.h"
#include "utils.h"
#include "def.h"

// helper: convert obj to sorted unique index list
int* prepare_indices(Array *obj, int *out_len, int max_idx) {
    if (obj->ndim != 0 && obj->ndim != 1) {
        fprintf(stderr, "insert: obj must be scalar or 1D array\n");
        return NULL;
    }
    if (obj->dtype != INT8 && obj->dtype != INT16 && obj->dtype != INT32 && obj->dtype != INT64 &&
        obj->dtype != UINT8 && obj->dtype != UINT16 && obj->dtype != UINT32 && obj->dtype != UINT64) {
        fprintf(stderr, "insert: obj must be integer type\n");
        return NULL;
    }
    int n = obj->size;
    int *indices = malloc(n * sizeof(int));
    if (!indices) return NULL;
    size_t elem_sz = dtype_size(obj->dtype);
    char *data = (char*)obj->data;
    for (int i = 0; i < n; ++i) {
        void *ptr = data + i * elem_sz;
        int64_t idx;
        switch (obj->dtype) {
            case INT8:   idx = *(int8_t*)ptr; break;
            case INT16:  idx = *(int16_t*)ptr; break;
            case INT32:  idx = *(int32_t*)ptr; break;
            case INT64:  idx = *(int64_t*)ptr; break;
            case UINT8:  idx = *(uint8_t*)ptr; break;
            case UINT16: idx = *(uint16_t*)ptr; break;
            case UINT32: idx = *(uint32_t*)ptr; break;
            case UINT64: idx = *(uint64_t*)ptr; break;
            default: idx = -1; break;
        }
        if (idx < 0 || idx > max_idx) {
            fprintf(stderr, "insert: index %lld out of bounds for axis size %d\n", (long long)idx, max_idx);
            free(indices);
            return NULL;
        }
        indices[i] = (int)idx;
    }
    // sortandunique
    qsort(indices, n, sizeof(int), cmp_int);
    int uniq_len = 0;
    for (int i = 0; i < n; ++i) {
        if (i == 0 || indices[i] != indices[i-1]) {
            indices[uniq_len++] = indices[i];
        }
    }
    *out_len = uniq_len;
    return indices;
}

// helper: convert values to an array matching the number of insert positions
Array* prepare_values(Array *values, int num_insert) {
    if (values->ndim == 0) {
        // scalar: repeat num_insert times
        int shape[1] = {num_insert};
        Array *rep = create_array(shape, 1, values->dtype);
        if (!rep) return NULL;
        size_t elem_sz = dtype_size(values->dtype);
        for (int i = 0; i < num_insert; ++i) {
            memcpy((char*)rep->data + i * elem_sz, values->data, elem_sz);
        }
        return rep;
    } else if (values->ndim == 1 && values->size == num_insert) {
        return copy_array(values);
    } else {
        fprintf(stderr, "insert: values must be scalar or 1D array of length %d\n", num_insert);
        return NULL;
    }
}


// helper function: convert obj to sorted unique integer list (return int64_t array, caller responsible for freeing)
int64_t* prepare_delete_indices(Array *obj, int *out_len, int max_idx) {
    if (obj->ndim != 0 && obj->ndim != 1) {
        fprintf(stderr, "delete: obj must be scalar or 1D array\n");
        return NULL;
    }
    // check if numbers based on type are integers
    switch (obj->dtype) {
        case INT8: case INT16: case INT32: case INT64:
        case UINT8: case UINT16: case UINT32: case UINT64:
            break;
        default:
            fprintf(stderr, "delete: obj must be integer type\n");
            return NULL;
    }
    int n = obj->size;
    int64_t *indices = malloc(n * sizeof(int64_t));
    if (!indices) return NULL;
    size_t elem_sz = dtype_size(obj->dtype);
    char *data = (char*)obj->data;
    for (int i = 0; i < n; i++) {
        void *ptr = data + i * elem_sz;
        int64_t idx;
        switch (obj->dtype) {
            case INT8:   idx = *(int8_t*)ptr; break;
            case INT16:  idx = *(int16_t*)ptr; break;
            case INT32:  idx = *(int32_t*)ptr; break;
            case INT64:  idx = *(int64_t*)ptr; break;
            case UINT8:  idx = *(uint8_t*)ptr; break;
            case UINT16: idx = *(uint16_t*)ptr; break;
            case UINT32: idx = *(uint32_t*)ptr; break;
            case UINT64: idx = *(uint64_t*)ptr; break;
            default: idx = -1; break;
        }
        if (idx < 0 || idx >= max_idx) {
            fprintf(stderr, "delete: index %lld out of bounds for axis size %d\n", (long long)idx, max_idx);
            free(indices);
            return NULL;
        }
        indices[i] = idx;
    }
    
    qsort(indices, n, sizeof(int64_t), cmp_int64);
    // unique
    int uniq_len = 0;
    for (int i = 0; i < n; i++) {
        if (i == 0 || indices[i] != indices[i-1]) {
            indices[uniq_len++] = indices[i];
        }
    }
    *out_len = uniq_len;
    return indices;
}

// helper function: delete along the specified axis (internal use, axis must be valid)
Array* delete_along_axis(Array *arr, int axis, int64_t *del_indices, int del_count) {
    int n_dims = arr->ndim;
    int axis_len = arr->shape[axis];
    int new_len = axis_len - del_count;
    if (new_len < 0) {
        fprintf(stderr, "delete: more indices to delete than elements\n");
        return NULL;
    }
    // create new shape
    int *new_shape = malloc(n_dims * sizeof(int));
    if (!new_shape) return NULL;
    memcpy(new_shape, arr->shape, n_dims * sizeof(int));
    new_shape[axis] = new_len;
    Array *result = create_array(new_shape, n_dims, arr->dtype);
    free(new_shape);
    if (!result) return NULL;

    // Stride computation
    int *in_strides = malloc(n_dims * sizeof(int));
    int *out_strides = malloc(n_dims * sizeof(int));
    if (!in_strides || !out_strides) {
        free(in_strides); free(out_strides);
        free_array(result);
        return NULL;
    }
    compute_strides(arr->shape, n_dims, in_strides);
    compute_strides(result->shape, n_dims, out_strides);

    size_t elem_sz = dtype_size(arr->dtype);
    char *in_data = (char*)arr->data;
    char *out_data = (char*)result->data;

    // traverse combinations of all dims except axis
    int outer_ndim = n_dims - 1;
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
    for (int i = 0; i < n_dims; i++) {
        if (i != axis) {
            outer_dims[idx] = i;
            outer_shape[idx] = arr->shape[i];
            idx++;
        }
    }
    int num_slices = 1;
    for (int i = 0; i < outer_ndim; i++) num_slices *= outer_shape[i];

    for (int slice = 0; slice < num_slices; slice++) {
        int base_in = 0, base_out = 0;
        for (int j = 0; j < outer_ndim; j++) {
            base_in  += outer_indices[j] * in_strides[outer_dims[j]];
            base_out += outer_indices[j] * out_strides[outer_dims[j]];
        }
        // copy along axis, skipping indices to delete
        int out_k = 0;
        for (int k = 0; k < axis_len; k++) {
            int skip = 0;
            for (int d = 0; d < del_count; d++) {
                if (del_indices[d] == k) { skip = 1; break; }
            }
            if (skip) continue;
            int in_offset = base_in + k * in_strides[axis];
            int out_offset = base_out + out_k * out_strides[axis];
            memcpy(out_data + out_offset * elem_sz, in_data + in_offset * elem_sz, elem_sz);
            out_k++;
        }
        // update outer_indices
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

    free(in_strides); free(out_strides);
    free(outer_dims); free(outer_shape); free(outer_indices);
    return result;
}