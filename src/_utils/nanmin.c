#include <math.h>
#include <complex.h>
#include <float.h>
#include "array.h"
#include "utils.h"

/**
 * nanmin  --  compute array minimum, ignore NaN (like numpy.nanmin)
 *
 * @param a    Input array
 * @param axis specified axis (-1 for flattened)
 * @return     FLOAT64 type result array; if all NaN, result element is NaN
 */
Array* nanmin(Array *a, int axis) {
    if (a == NULL) {
        fprintf(stderr, "nanmin: NULL array argument\n");
        return NULL;
    }
    int ndim = a->ndim;
    if (axis < -1 || axis >= ndim) {
        fprintf(stderr, "nanmin: invalid axis %d for %dD array\n", axis, ndim);
        return NULL;
    }

    /* ─── axis = -1: flattened ─── */
    if (axis == -1) {
        double min_val = INFINITY;
        int found = 0;
        size_t elem_sz = dtype_size(a->dtype);
        char *data = (char*)a->data;
        for (int i = 0; i < a->size; ++i) {
            void *ptr = data + i * elem_sz;
            if (is_nan(ptr, a->dtype)) continue;
            double val = get_value(ptr, a->dtype);
            if (!found || val < min_val) { min_val = val; found = 1; }
        }
        /* all NaN → return NaN */
        Array *res;
        if (!found) {
            double nan_val = NAN;
            res = create_scalar_array(&nan_val, FLOAT64);
        } else {
            res = create_scalar_array(&min_val, FLOAT64);
        }
        return res;
    }

    /* ─── along specified axis ─── */
    int axis_len = a->shape[axis];
    if (axis_len == 0) {
        int out_ndim = ndim - 1;
        int *out_shape = malloc(out_ndim * sizeof(int));
        int idx = 0;
        for (int i = 0; i < ndim; ++i) if (i != axis) out_shape[idx++] = a->shape[i];
        Array *res = create_array(out_shape, out_ndim, FLOAT64);
        free(out_shape);
        return res;
    }

    /* output shape */
    int out_ndim = ndim - 1;
    int *out_shape = malloc(out_ndim * sizeof(int));
    int od = 0;
    for (int i = 0; i < ndim; ++i) if (i != axis) out_shape[od++] = a->shape[i];
    Array *result = create_array(out_shape, out_ndim, FLOAT64);
    free(out_shape);
    if (!result) return NULL;

    /* stride */
    int *strides = malloc(ndim * sizeof(int));
    compute_strides(a->shape, ndim, strides);
    size_t elem_sz = dtype_size(a->dtype);
    char *data = (char*)a->data;
    double *res_data = (double*)result->data;

    /* outer ndim combo */
    int outer_ndim = ndim - 1;
    int *outer_dims = malloc(outer_ndim * sizeof(int));
    int *outer_shape = malloc(outer_ndim * sizeof(int));
    int *outer_indices = calloc(outer_ndim, sizeof(int));
    if (!outer_dims || !outer_shape || !outer_indices) {
        free(strides); free(outer_dims); free(outer_shape); free(outer_indices);
        free_array(result); return NULL;
    }
    od = 0;
    for (int i = 0; i < ndim; ++i) if (i != axis) outer_dims[od++] = i;
    for (int i = 0; i < outer_ndim; ++i) outer_shape[i] = a->shape[outer_dims[i]];
    int num_slices = 1;
    for (int i = 0; i < outer_ndim; ++i) num_slices *= outer_shape[i];

    /* result stride */
    int *res_strides = malloc(out_ndim * sizeof(int));
    if (out_ndim > 0) compute_strides(result->shape, out_ndim, res_strides);

    for (int slice = 0; slice < num_slices; ++slice) {
        int base = 0;
        for (int j = 0; j < outer_ndim; ++j)
            base += outer_indices[j] * strides[outer_dims[j]];

        double min_val = INFINITY;
        int found = 0;
        for (int k = 0; k < axis_len; ++k) {
            void *ptr = data + (base + k * strides[axis]) * elem_sz;
            if (is_nan(ptr, a->dtype)) continue;
            double val = get_value(ptr, a->dtype);
            if (!found || val < min_val) { min_val = val; found = 1; }
        }
        /* all NaN → NaN */
        int out_off = 0;
        for (int j = 0; j < outer_ndim; ++j)
            out_off += outer_indices[j] * res_strides[j];
        res_data[out_off] = found ? min_val : NAN;

        /* update outer_indices */
        int carry = 1;
        for (int j = outer_ndim - 1; j >= 0 && carry; --j) {
            outer_indices[j]++;
            if (outer_indices[j] >= outer_shape[j]) { outer_indices[j] = 0; carry = 1; }
            else carry = 0;
        }
    }

    free(strides); free(res_strides);
    free(outer_dims); free(outer_shape); free(outer_indices);
    return result;
}
