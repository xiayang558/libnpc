#include "array.h"

// helper: compute 1-d array gradient (central difference, first-order difference at boundaries)
static Array* gradient_1d(Array *arr, double h) {
    int n = arr->size;
    Array *g = create_array(arr->shape, arr->ndim, arr->dtype);
    if (!g) return NULL;
    double *f_data = (double*)arr->data;
    double *g_data = (double*)g->data;
    if (n == 1) {
        g_data[0] = 0.0;
        return g;
    }
    // left boundary (forward difference)
    g_data[0] = (f_data[1] - f_data[0]) / h;
    // interior points (central difference)
    for (int i = 1; i < n - 1; i++) {
        g_data[i] = (f_data[i+1] - f_data[i-1]) / (2.0 * h);
    }
    // right boundary (backward difference)
    g_data[n-1] = (f_data[n-1] - f_data[n-2]) / h;
    return g;
}

// compute gradient along specified axis (input f must be FLOAT64)
static Array* gradient_along_axis(Array *arr, int axis, double h) {
    int ndim = arr->ndim;
    int shape[ndim];
    for (int i = 0; i < ndim; i++) shape[i] = arr->shape[i];
    Array *g = create_array(shape, ndim, arr->dtype);
    if (!g) return NULL;

    // compute strides (in units of elements)
    int *strides = malloc(ndim * sizeof(int));
    strides[ndim-1] = 1;
    for (int i = ndim-2; i >= 0; i--) strides[i] = strides[i+1] * arr->shape[i+1];
    int axis_len = arr->shape[axis];
    if (axis_len == 1) {
        // only one element, gradient is zero
        memset(g->data, 0, g->size * sizeof(double));
        free(strides);
        return g;
    }

    double *f_data = (double*)arr->data;
    double *g_data = (double*)g->data;

    // traverse outer ndim combinations excluding axis
    int outer_ndim = ndim - 1;
    int *outer_dims = malloc(outer_ndim * sizeof(int));
    int *outer_shape = malloc(outer_ndim * sizeof(int));
    int *outer_indices = calloc(outer_ndim, sizeof(int));
    int idx = 0;
    for (int i = 0; i < ndim; i++) {
        if (i != axis) {
            outer_dims[idx] = i;
            outer_shape[idx] = arr->shape[i];
            idx++;
        }
    }
    int num_slices = 1;
    for (int i = 0; i < outer_ndim; i++) num_slices *= outer_shape[i];

    for (int slice = 0; slice < num_slices; slice++) {
        int base = 0;
        for (int j = 0; j < outer_ndim; j++) {
            base += outer_indices[j] * strides[outer_dims[j]];
        }
        // compute current slice gradient along axis
        // left boundary
        int offset0 = base + 0 * strides[axis];
        int offset1 = base + 1 * strides[axis];
        g_data[offset0] = (f_data[offset1] - f_data[offset0]) / h;
        // interior
        for (int k = 1; k < axis_len - 1; k++) {
            int off_left = base + (k-1) * strides[axis];
            int off_right = base + (k+1) * strides[axis];
            int off_center = base + k * strides[axis];
            g_data[off_center] = (f_data[off_right] - f_data[off_left]) / (2.0 * h);
        }
        // right boundary
        int offset_last = base + (axis_len - 1) * strides[axis];
        int offset_prev = base + (axis_len - 2) * strides[axis];
        g_data[offset_last] = (f_data[offset_last] - f_data[offset_prev]) / h;

        // update index
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

    free(strides);
    free(outer_dims);
    free(outer_shape);
    free(outer_indices);
    return g;
}


/**
 * @brief compute array gradient.
 * @param arr      Input array (at least 1-d).
 * @param spacing  Spacing parameter: if NULL then spacing is 1; if scalar (0-d array) then all axes use same spacing;
 *                 if 1-d array and length is arr->ndim, then each axis uses corresponding spacing.
 * @param out      output: gradient array list (each element is Array*), caller must free out and its contents.
 * @param num_out  output: gradient array count (equals arr->ndim).
 * @return 0 success, -1 failed.
 */
int gradient(Array *arr, Array *spacing, Array ***out, int *num_out) {
    if (arr == NULL || out == NULL || num_out == NULL) {
        fprintf(stderr, "gradient: NULL argument\n");
        return -1;
    }
    if (arr->dtype != FLOAT64) {
        fprintf(stderr, "gradient: only FLOAT64 supported\n");
        return -1;
    }
    int ndim = arr->ndim;
    *num_out = ndim;
    *out = malloc(ndim * sizeof(Array*));
    if (!*out) return -1;

    // parse spacing
    double *h = malloc(ndim * sizeof(double));
    if (!h) {
        free(*out);
        return -1;
    }
    if (spacing == NULL) {
        for (int i = 0; i < ndim; i++) h[i] = 1.0;
    } else if (spacing->ndim == 0) {
        double scalar = *(double*)spacing->data;
        for (int i = 0; i < ndim; i++) h[i] = scalar;
    } else if (spacing->ndim == 1 && spacing->size == ndim) {
        double *sp = (double*)spacing->data;
        for (int i = 0; i < ndim; i++) h[i] = sp[i];
    } else {
        fprintf(stderr, "gradient: invalid spacing parameter\n");
        free(h);
        free(*out);
        return -1;
    }

    // compute gradient for each axis
    for (int ax = 0; ax < ndim; ax++) {
        (*out)[ax] = gradient_along_axis(arr, ax, h[ax]);
        if (!(*out)[ax]) {
            for (int j = 0; j < ax; j++) free_array((*out)[j]);
            free(*out);
            free(h);
            return -1;
        }
    }
    free(h);
    return 0;
}