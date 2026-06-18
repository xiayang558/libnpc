#include <math.h>
#include <complex.h>
#include <float.h>
#include <stdlib.h>
#include "array.h"
#include "utils.h"

static int dcmp(const void *a, const void *b) {
    double d = *(const double*)a - *(const double*)b;
    return (d > 0) - (d < 0);
}

Array* nanmedian(Array *a, int axis) {
    if (a == NULL) { fprintf(stderr, "nanmedian: NULL array argument\n"); return NULL; }
    int ndim = a->ndim;
    if (axis < -1 || axis >= ndim) { fprintf(stderr, "nanmedian: invalid axis %d for %dD array\n", axis, ndim); return NULL; }

    /* ─── axis = -1 ─── */
    if (axis == -1) {
        double *vals = malloc(a->size * sizeof(double));
        if (!vals) return NULL;
        size_t elem_sz = dtype_size(a->dtype);
        char *data = (char*)a->data;
        int count = 0;
        for (int i = 0; i < a->size; ++i) {
            void *ptr = data + i * elem_sz;
            if (is_nan(ptr, a->dtype)) continue;
            vals[count++] = get_value(ptr, a->dtype);
        }
        double result;
        if (count == 0) {
            result = NAN;
        } else {
            qsort(vals, count, sizeof(double), dcmp);
            if (count % 2 == 1) result = vals[count / 2];
            else result = (vals[count / 2 - 1] + vals[count / 2]) / 2.0;
        }
        free(vals);
        return create_scalar_array(&result, FLOAT64);
    }

    /* ─── along specified axis ─── */
    int axis_len = a->shape[axis];
    int out_ndim = ndim - 1;
    int *out_shape = malloc(out_ndim * sizeof(int));
    int od = 0;
    for (int i = 0; i < ndim; ++i) if (i != axis) out_shape[od++] = a->shape[i];
    Array *result = create_array(out_shape, out_ndim, FLOAT64);
    free(out_shape);
    if (!result) return NULL;

    if (axis_len == 0) return result;

    int *strides = malloc(ndim * sizeof(int));
    compute_strides(a->shape, ndim, strides);
    size_t elem_sz = dtype_size(a->dtype);
    char *data = (char*)a->data;
    double *res_data = (double*)result->data;

    int outer_ndim = ndim - 1;
    int *outer_dims = malloc(outer_ndim * sizeof(int));
    int *outer_shape_arr = malloc(outer_ndim * sizeof(int));
    int *outer_indices = calloc(outer_ndim, sizeof(int));
    if (!outer_dims || !outer_shape_arr || !outer_indices) {
        free(strides); free(outer_dims); free(outer_shape_arr); free(outer_indices);
        free_array(result); return NULL;
    }
    od = 0;
    for (int i = 0; i < ndim; ++i) if (i != axis) { outer_dims[od] = i; outer_shape_arr[od] = a->shape[i]; od++; }
    int num_slices = 1;
    for (int i = 0; i < outer_ndim; ++i) num_slices *= outer_shape_arr[i];

    int *res_strides = malloc(out_ndim * sizeof(int));
    if (out_ndim > 0) compute_strides(result->shape, out_ndim, res_strides);

    double *slice_vals = malloc(axis_len * sizeof(double));
    if (!slice_vals) {
        free(strides); free(outer_dims); free(outer_shape_arr); free(outer_indices);
        free(res_strides); free_array(result); return NULL;
    }

    for (int slice = 0; slice < num_slices; ++slice) {
        int base = 0;
        for (int j = 0; j < outer_ndim; ++j) base += outer_indices[j] * strides[outer_dims[j]];
        int count = 0;
        for (int k = 0; k < axis_len; ++k) {
            void *ptr = data + (base + k * strides[axis]) * elem_sz;
            if (is_nan(ptr, a->dtype)) continue;
            slice_vals[count++] = get_value(ptr, a->dtype);
        }
        double median_val;
        if (count == 0) {
            median_val = NAN;
        } else {
            qsort(slice_vals, count, sizeof(double), dcmp);
            if (count % 2 == 1) median_val = slice_vals[count / 2];
            else median_val = (slice_vals[count / 2 - 1] + slice_vals[count / 2]) / 2.0;
        }
        int out_off = 0;
        for (int j = 0; j < outer_ndim; ++j) out_off += outer_indices[j] * res_strides[j];
        res_data[out_off] = median_val;

        int carry = 1;
        for (int j = outer_ndim - 1; j >= 0 && carry; --j) {
            outer_indices[j]++;
            if (outer_indices[j] >= outer_shape_arr[j]) { outer_indices[j] = 0; carry = 1; }
            else carry = 0;
        }
    }

    free(strides); free(res_strides); free(slice_vals);
    free(outer_dims); free(outer_shape_arr); free(outer_indices);
    return result;
}
