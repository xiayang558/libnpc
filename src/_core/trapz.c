#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "array.h"
#include "utils.h"

/**
 * trapz  --  Numerical integration using the trapezoidal rule (like numpy.trapz)
 *
 * Formula: sum (y[i] + y[i+1]) * d[i] / 2
 *   d[i] = x[i+1] - x[i]  (x != NULL)
 *   d[i] = dx             (x == NULL)
 *
 * @param y     Input array
 * @param x     optional x coordinates (1D), NULL means use dx
 * @param dx    uniform spacing
 * @param axis  axis to integrate along, -1 means the last axis
 * @return integration result (one fewer dimension than y), output dtype is FLOAT64
 */
Array* trapz(Array *y, Array *x, double dx, int axis) {
    if (y == NULL) {
        fprintf(stderr, "trapz: NULL argument\n");
        return NULL;
    }

    int ndim = y->ndim;

    /* Handle axis */
    if (axis < 0) axis += ndim;
    if (axis < 0 || axis >= ndim) {
        fprintf(stderr, "trapz: axis %d out of bounds for %d-d array\n", axis, ndim);
        return NULL;
    }

    int n = y->shape[axis];  /* number of points along integration axis */

    /* Validate x if provided */
    if (x != NULL) {
        if (x->ndim != 1 || x->size != n) {
            fprintf(stderr, "trapz: x must be 1D with size %d, got size %d\n", n, x->size);
            return NULL;
        }
    }

    /* Build output shape (remove axis) */
    int out_ndim = ndim - 1;
    int *out_shape = NULL;
    int out_size = 1;
    if (out_ndim > 0) {
        out_shape = malloc(out_ndim * sizeof(int));
        int idx = 0;
        for (int i = 0; i < ndim; i++) {
            if (i != axis) {
                out_shape[idx] = y->shape[i];
                out_size *= out_shape[idx];
                idx++;
            }
        }
    }

    Array *res;
    if (out_ndim == 0) {
        int one = 1;
        res = create_array(&one, 1, FLOAT64);
    } else {
        res = create_array(out_shape, out_ndim, FLOAT64);
        free(out_shape);
    }
    if (!res) return NULL;

    /* If < 2 points, result is zero */
    if (n < 2) {
        memset(res->data, 0, res->size * sizeof(double));
        return res;
    }

    /* Precompute step sizes d[0..n-2] */
    double *d = malloc((n - 1) * sizeof(double));
    if (!d) { free_array(res); return NULL; }

    if (x != NULL) {
        char *x_data = (char*)x->data;
        size_t x_sz = dtype_size(x->dtype);
        for (int i = 0; i < n - 1; i++) {
            d[i] = get_value(x_data + (i+1) * x_sz, x->dtype) -
                   get_value(x_data + i * x_sz, x->dtype);
        }
    } else {
        for (int i = 0; i < n - 1; i++) d[i] = dx;
    }

    /* Compute element-level strides */
    int *strides = malloc(ndim * sizeof(int));
    if (!strides) { free(d); free_array(res); return NULL; }
    compute_strides(y->shape, ndim, strides);

    size_t elem_sz = dtype_size(y->dtype);
    char *y_data = (char*)y->data;
    double *res_data = (double*)res->data;

    /* For each output position, integrate along axis */
    int *indices = malloc(out_ndim > 0 ? out_ndim : 1 * sizeof(int));
    if (!indices) { free(strides); free(d); free_array(res); return NULL; }

    for (int out_flat = 0; out_flat < out_size; out_flat++) {
        /* Decode flat index to multi-index in output space */
        int tmp = out_flat;
        for (int i = out_ndim - 1; i >= 0; i--) {
            indices[i] = tmp % (out_ndim > 0 ? res->shape[i] : 1);
            tmp /= (out_ndim > 0 ? res->shape[i] : 1);
        }

        /* Build base element index in source:
         * position 0 along axis, rest from output indices */
        int base = 0;
        int out_i = 0;
        for (int i = 0; i < ndim; i++) {
            if (i == axis) continue;
            base += indices[out_i++] * strides[i];
        }

        /* Integrate along axis */
        double sum = 0.0;
        for (int k = 0; k < n - 1; k++) {
            int off0 = base + k * strides[axis];
            int off1 = base + (k + 1) * strides[axis];
            double v0 = get_value(y_data + off0 * elem_sz, y->dtype);
            double v1 = get_value(y_data + off1 * elem_sz, y->dtype);
            sum += (v0 + v1) * 0.5 * d[k];
        }
        res_data[out_flat] = sum;
    }

    free(indices);
    free(strides);
    free(d);
    return res;
}
