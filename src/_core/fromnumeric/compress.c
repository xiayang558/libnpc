#include "array.h"


Array* compress(Array *condition, Array *a, int axis) {
    if (condition == NULL || a == NULL) {
        fprintf(stderr, "compress: NULL argument\n");
        return NULL;
    }

    // condition array must be 1-dimensional
    if (condition->ndim != 1) {
        fprintf(stderr, "compress: condition must be 1-dimensional\n");
        return NULL;
    }

    int cond_len = condition->size;
    int use_flat = (axis == -1);

    if (!use_flat) {
        // check axis range
        if (axis < 0 || axis >= a->ndim) {
            fprintf(stderr, "compress: invalid axis %d for array with %d dimensions\n",
                    axis, a->ndim);
            return NULL;
        }
        // check if condition length matches axis length
        if (cond_len != a->shape[axis]) {
            fprintf(stderr, "compress: condition length (%d) must match axis length (%d)\n",
                    cond_len, a->shape[axis]);
            return NULL;
        }
    } else {
        // flattened mode, condition length must equal total array elements
        if (cond_len != a->size) {
            fprintf(stderr, "compress: condition length (%d) must match total array size (%d)\n",
                    cond_len, a->size);
            return NULL;
        }
    }

    // collect indices where condition is true
    size_t elem_size_cond = dtype_size(condition->dtype);
    char *cond_data = (char*)condition->data;
    int *true_indices = malloc(cond_len * sizeof(int));
    if (true_indices == NULL) {
        fprintf(stderr, "compress: memory allocation failed\n");
        return NULL;
    }
    int true_count = 0;
    for (int i = 0; i < cond_len; i++) {
        void *ptr = cond_data + i * elem_size_cond;
        if (is_nonzero(ptr, condition->dtype)) {
            true_indices[true_count++] = i;
        }
    }

    // determine output shape
    int out_ndim;
    int *out_shape = NULL;
    if (use_flat) {
        out_ndim = 1;
        out_shape = malloc(sizeof(int));
        if (out_shape == NULL) {
            free(true_indices);
            fprintf(stderr, "compress: memory allocation failed\n");
            return NULL;
        }
        out_shape[0] = true_count;
    } else {
        out_ndim = a->ndim;
        out_shape = malloc(out_ndim * sizeof(int));
        if (out_shape == NULL) {
            free(true_indices);
            fprintf(stderr, "compress: memory allocation failed\n");
            return NULL;
        }
        memcpy(out_shape, a->shape, out_ndim * sizeof(int));
        out_shape[axis] = true_count;
    }

    Array *result = create_array(out_shape, out_ndim, a->dtype);
    free(out_shape);
    if (result == NULL) {
        free(true_indices);
        fprintf(stderr, "compress: failed to create result array\n");
        return NULL;
    }

    // if true_count == 0, return empty array directly (but shape may be 0)
    if (true_count == 0) {
        free(true_indices);
        return result;
    }

    // compute strides (using existing function compute_strides)
    int *a_strides = malloc(a->ndim * sizeof(int));
    int *res_strides = malloc(result->ndim * sizeof(int));
    if (a_strides == NULL || res_strides == NULL) {
        free(a_strides); free(res_strides); free(true_indices);
        free_array(result);
        fprintf(stderr, "compress: memory allocation failed\n");
        return NULL;
    }
    compute_strides(a->shape, a->ndim, a_strides);
    compute_strides(result->shape, result->ndim, res_strides);

    char *a_data = (char*)a->data;
    char *res_data = (char*)result->data;
    size_t elem_size = dtype_size(a->dtype);

    if (use_flat) {
        // flattened mode: treat a as 1D, copy by true_indices
        for (int t = 0; t < true_count; t++) {
            int src_idx = true_indices[t];
            memcpy(res_data + t * elem_size, a_data + src_idx * elem_size, elem_size);
        }
    } else {
        // compress along specified axis
        int outer_ndim = a->ndim - 1;
        int *outer_dims = malloc(outer_ndim * sizeof(int));
        int *outer_shape = malloc(outer_ndim * sizeof(int));
        int *outer_indices = calloc(outer_ndim, sizeof(int));
        if (outer_dims == NULL || outer_shape == NULL || outer_indices == NULL) {
            free(a_strides); free(res_strides); free(true_indices);
            free(outer_dims); free(outer_shape); free(outer_indices);
            free_array(result);
            fprintf(stderr, "compress: memory allocation failed\n");
            return NULL;
        }

        int out_idx = 0;
        for (int i = 0; i < a->ndim; i++) {
            if (i != axis) {
                outer_dims[out_idx] = i;
                outer_shape[out_idx] = a->shape[i];
                out_idx++;
            }
        }

        int num_slices = 1;
        for (int i = 0; i < outer_ndim; i++) num_slices *= outer_shape[i];

        for (int s = 0; s < num_slices; s++) {
            // compute base offset for current input slice (excluding axis outer ndim)
            int base_offset = 0;
            for (int j = 0; j < outer_ndim; j++) {
                base_offset += outer_indices[j] * a_strides[outer_dims[j]];
            }
            // for each true index, copy the corresponding element to output
            for (int t = 0; t < true_count; t++) {
                int src_offset = base_offset + true_indices[t] * a_strides[axis];
                // compute output offset
                int res_offset = 0;
                for (int j = 0; j < outer_ndim; j++) {
                    res_offset += outer_indices[j] * res_strides[outer_dims[j]];
                }
                res_offset += t * res_strides[axis];
                memcpy(res_data + res_offset * elem_size, a_data + src_offset * elem_size, elem_size);
            }
            // update outer_indices to the next slice
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
        }

        free(outer_dims);
        free(outer_shape);
        free(outer_indices);
    }

    free(a_strides);
    free(res_strides);
    free(true_indices);
    return result;
}