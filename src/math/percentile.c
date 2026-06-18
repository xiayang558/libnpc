#include <math.h>
#include <stdlib.h>
#include "array.h"
#include "shape.h"
#include "utils.h"

static int dcmp(const void *a, const void *b) {
    double d = *(const double*)a - *(const double*)b;
    return (d > 0) - (d < 0);
}

/** Linear interpolation: given sorted slice of length N and percentile p (0-100) */
static double pct_value(double *sorted, int n, double p) {
    double idx_f = (p / 100.0) * (n - 1);
    int lo = (int)idx_f;
    double frac = idx_f - lo;
    if (frac < 1e-15) return sorted[lo];
    return sorted[lo] * (1.0 - frac) + sorted[lo + 1] * frac;
}

Array* percentile(Array *a, Array *q, int axis) {
    if (a == NULL || q == NULL) {
        fprintf(stderr, "percentile: NULL argument\n");
        return NULL;
    }
    if (q->ndim != 1) {
        fprintf(stderr, "percentile: q must be 1D array\n");
        return NULL;
    }
    int ndim = a->ndim;
    if (axis < -1 || axis >= ndim) {
        fprintf(stderr, "percentile: invalid axis %d for %dD array\n", axis, ndim);
        return NULL;
    }

    int nq = q->size;
    double *qd = (double*)q->data;

    /* ─── axis = -1 : flatten ─── */
    if (axis == -1) {
        Array *flat = flatten(a);
        if (!flat) return NULL;
        Array *dbl = (flat->dtype == FLOAT64) ? flat : astype(flat, FLOAT64);
        if (flat != dbl) free_array(flat);
        if (!dbl) return NULL;

        int n = dbl->size;
        double *vals = (double*)dbl->data;
        qsort(vals, n, sizeof(double), dcmp);

        Array *result = create_array((int[]){nq}, 1, FLOAT64);
        if (!result) { free_array(dbl); return NULL; }
        double *rd = (double*)result->data;
        for (int i = 0; i < nq; i++) rd[i] = pct_value(vals, n, qd[i]);
        free_array(dbl);
        return result;
    }

    /* ─── along axis ─── */
    int axis_len = a->shape[axis];

    /* output shape: [nq] + all dims except axis */
    int out_ndim = ndim;
    int *out_shape = malloc(out_ndim * sizeof(int));
    out_shape[0] = nq;
    int od = 0;
    for (int i = 0; i < ndim; i++)
        if (i != axis) out_shape[++od] = a->shape[i];

    Array *result = create_array(out_shape, out_ndim, FLOAT64);
    free(out_shape);
    if (!result) return NULL;

    if (axis_len == 0) return result;

    size_t elem_sz = dtype_size(a->dtype);
    char *data = (char*)a->data;
    double *res_data = (double*)result->data;

    int *strides = malloc(ndim * sizeof(int));
    compute_strides(a->shape, ndim, strides);
    int *res_strides = malloc(out_ndim * sizeof(int));
    compute_strides(result->shape, out_ndim, res_strides);

    int outer_ndim = ndim - 1;
    int *outer_dims = malloc(outer_ndim * sizeof(int));
    int *outer_shape_arr = malloc(outer_ndim * sizeof(int));
    int *outer_indices = calloc(outer_ndim, sizeof(int));
    if (!outer_dims || !outer_shape_arr || !outer_indices) {
        free(strides); free(res_strides);
        free(outer_dims); free(outer_shape_arr); free(outer_indices);
        free_array(result); return NULL;
    }
    od = 0;
    for (int i = 0; i < ndim; i++)
        if (i != axis) { outer_dims[od] = i; outer_shape_arr[od] = a->shape[i]; od++; }

    int num_slices = 1;
    for (int i = 0; i < outer_ndim; i++) num_slices *= outer_shape_arr[i];

    double *slice_vals = malloc(axis_len * sizeof(double));
    if (!slice_vals) {
        free(strides); free(res_strides);
        free(outer_dims); free(outer_shape_arr); free(outer_indices);
        free_array(result); return NULL;
    }

    for (int s = 0; s < num_slices; s++) {
        int base = 0;
        for (int j = 0; j < outer_ndim; j++)
            base += outer_indices[j] * strides[outer_dims[j]];

        /* Collect values along axis */
        for (int k = 0; k < axis_len; k++) {
            void *ptr = data + (base + k * strides[axis]) * elem_sz;
            slice_vals[k] = get_value(ptr, a->dtype);
        }
        qsort(slice_vals, axis_len, sizeof(double), dcmp);

        /* Compute output base: outer_indices[j] maps to result dim j+1 */
        int out_base = 0;
        for (int j = 0; j < outer_ndim; j++)
            out_base += outer_indices[j] * res_strides[j + 1];

        /* Write qi values at position 0 */
        for (int qi = 0; qi < nq; qi++) {
            res_data[out_base + qi * res_strides[0]] =
                pct_value(slice_vals, axis_len, qd[qi]);
        }

        /* Advance outer_indices */
        int carry = 1;
        for (int j = outer_ndim - 1; j >= 0 && carry; j--) {
            outer_indices[j]++;
            if (outer_indices[j] >= outer_shape_arr[j]) {
                outer_indices[j] = 0; carry = 1;
            } else carry = 0;
        }
    }

    free(strides); free(res_strides); free(slice_vals);
    free(outer_dims); free(outer_shape_arr); free(outer_indices);
    return result;
}
