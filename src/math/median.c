#include "array.h"
#include "shape.h"
#include "utils.h"

Array* median(Array *a, int axis, int keepdims) {
    if (a == NULL) {
        fprintf(stderr, "median: NULL array argument\n");
        return NULL;
    }
    int ndim = a->ndim;
    if (axis < -1 || axis >= ndim) {
        fprintf(stderr, "median: invalid axis %d for %dD array\n", axis, ndim);
        return NULL;
    }

    // compute output shape
    int out_ndim;
    int *out_shape = NULL;
    if (axis == -1) {
        if (keepdims) {
            out_ndim = ndim;
            out_shape = malloc(ndim * sizeof(int));
            for (int i = 0; i < ndim; ++i) out_shape[i] = 1;
        } else {
            out_ndim = 0;
            out_shape = NULL;
        }
    } else {
        if (keepdims) {
            out_ndim = ndim;
            out_shape = malloc(ndim * sizeof(int));
            for (int i = 0; i < ndim; ++i) {
                if (i == axis) out_shape[i] = 1;
                else out_shape[i] = a->shape[i];
            }
        } else {
            out_ndim = ndim - 1;
            out_shape = malloc(out_ndim * sizeof(int));
            int idx = 0;
            for (int i = 0; i < ndim; ++i) {
                if (i != axis) out_shape[idx++] = a->shape[i];
            }
        }
    }

    // create result array
    Array *result;
    if (out_ndim == 0) {
        // manually create scalar array
        result = (Array*)malloc(sizeof(Array));
        if (!result) { free(out_shape); return NULL; }
        result->data = malloc(sizeof(double));
        if (!result->data) { free(result); free(out_shape); return NULL; }
        result->dtype = FLOAT64;
        result->shape = NULL;
        result->ndim = 0;
        result->size = 1;
        result->strides = NULL;
        result->is_view = 0;
        *(double*)result->data = 0.0;
    } else {
        result = create_array(out_shape, out_ndim, FLOAT64);
        free(out_shape);
        if (!result) return NULL;
    }

    if (a->size == 0) return result;

    // axis == -1 flatten handling
    if (axis == -1) {
        Array *flat = flatten(a);
        if (!flat) { free_array(result); return NULL; }
        Array *dbl = astype(flat, FLOAT64);
        free_array(flat);
        if (!dbl) { free_array(result); return NULL; }
        qsort(dbl->data, dbl->size, sizeof(double), cmp_double);
        int n = dbl->size;
        double median_val;
        if (n % 2 == 1) {
            median_val = ((double*)dbl->data)[n / 2];
        } else {
            median_val = (((double*)dbl->data)[n / 2 - 1] + ((double*)dbl->data)[n / 2]) / 2.0;
        }
        free_array(dbl);
        if (out_ndim == 0) {
            *(double*)result->data = median_val;
        } else {
            double *res_data = (double*)result->data;
            res_data[0] = median_val;
        }
        return result;
    }

    // compute median along specified axis
    int axis_len = a->shape[axis];
    int *strides = malloc(a->ndim * sizeof(int));
    if (!strides) { free_array(result); return NULL; }
    compute_strides(a->shape, ndim, strides);
    size_t elem_sz = dtype_size(a->dtype);
    char *data = (char*)a->data;
    double *out_data = (double*)result->data;

    int outer_ndim = ndim - 1;
    int *outer_dims = malloc(outer_ndim * sizeof(int));
    int *outer_shape = malloc(outer_ndim * sizeof(int));
    int *outer_indices = calloc(outer_ndim, sizeof(int));
    if (!outer_dims || !outer_shape || !outer_indices) {
        free(strides); free(outer_dims); free(outer_shape); free(outer_indices);
        free_array(result);
        return NULL;
    }
    int od_idx = 0;
    for (int i = 0; i < ndim; ++i) {
        if (i != axis) {
            outer_dims[od_idx] = i;
            outer_shape[od_idx] = a->shape[i];
            od_idx++;
        }
    }
    int num_slices = 1;
    for (int i = 0; i < outer_ndim; ++i) num_slices *= outer_shape[i];

    double *slice_vals = malloc(axis_len * sizeof(double));
    if (!slice_vals) {
        free(strides); free(outer_dims); free(outer_shape); free(outer_indices);
        free_array(result);
        return NULL;
    }

    int *res_strides = NULL;
    if (result->ndim > 0) {
        res_strides = malloc(result->ndim * sizeof(int));
        compute_strides(result->shape, result->ndim, res_strides);
    }

    for (int slice = 0; slice < num_slices; ++slice) {
        int base = 0;
        for (int j = 0; j < outer_ndim; ++j) {
            base += outer_indices[j] * strides[outer_dims[j]];
        }
        for (int k = 0; k < axis_len; ++k) {
            void *ptr = data + (base + k * strides[axis]) * elem_sz;
            double val = 0.0;
            switch (a->dtype) {
                case INT8:   val = *(int8_t*)ptr; break;
                case INT16:  val = *(int16_t*)ptr; break;
                case INT32:  val = *(int32_t*)ptr; break;
                case INT64:  val = *(int64_t*)ptr; break;
                case UINT8:  val = *(uint8_t*)ptr; break;
                case UINT16: val = *(uint16_t*)ptr; break;
                case UINT32: val = *(uint32_t*)ptr; break;
                case UINT64: val = *(uint64_t*)ptr; break;
                case FLOAT32: val = *(float*)ptr; break;
                case FLOAT64: val = *(double*)ptr; break;
                case COMPLEX64: val = cabsf(*(complex float*)ptr); break;
                case COMPLEX128: val = cabs(*(complex double*)ptr); break;
                default: break;
            }
            slice_vals[k] = val;
        }
        qsort(slice_vals, axis_len, sizeof(double), cmp_double);
        double median_val;
        if (axis_len % 2 == 1) {
            median_val = slice_vals[axis_len / 2];
        } else {
            median_val = (slice_vals[axis_len / 2 - 1] + slice_vals[axis_len / 2]) / 2.0;
        }
        int out_offset = 0;
        if (result->ndim > 0) {
            for (int j = 0; j < outer_ndim; ++j) {
                out_offset += outer_indices[j] * res_strides[j];
            }
        }
        out_data[out_offset] = median_val;
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
    free(slice_vals);
    if (res_strides) free(res_strides);
    return result;
}
