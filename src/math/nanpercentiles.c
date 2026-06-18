#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "array.h"
#include "shape.h"
#include "utils.h"

static int dcmp(const void *a, const void *b) {
    double d = *(const double*)a - *(const double*)b;
    return (d > 0) - (d < 0);
}

/* Collect non-NaN values from a slice, count them */
static int collect_nonnan(char *base, int step, DataType dtype, int len, double *out, int *out_n) {
    int cnt = 0;
    size_t esz = dtype_size(dtype);
    for (int k = 0; k < len; k++) {
        double v = get_value(base + k * step * esz, dtype);
        if (!isnan(v)) out[cnt++] = v;
    }
    if (out_n) *out_n = cnt;
    return cnt;
}

/** qtl: 0-1 scaled (quantile) or 0-100 scaled (percentile) */
static double nan_val(double *sorted, int n, double q, int pct_mode) {
    if (n == 0) return NAN;
    double idx_f = pct_mode ? (q / 100.0) * (n - 1) : q * (n - 1);
    int lo = (int)idx_f;
    double frac = idx_f - lo;
    if (frac < 1e-15) return sorted[lo];
    return sorted[lo] * (1.0 - frac) + sorted[lo + 1] * frac;
}

static Array* nan_percentile_core(Array *a, Array *q, int axis, int pct_mode) {
    if (a == NULL || q == NULL) return NULL;
    if (q->ndim != 1) return NULL;
    int ndim = a->ndim;
    int nq = q->size;
    double *qd = (double*)q->data;

    if (axis < -1 || axis >= ndim) return NULL;

    if (axis == -1) {
        double *vals = malloc(a->size * sizeof(double));
        if (!vals) return NULL;
        int cnt = 0;
        size_t esz = dtype_size(a->dtype);
        char *src = (char*)a->data;
        for (int i = 0; i < a->size; i++) {
            double v = get_value(src + i * esz, a->dtype);
            if (!isnan(v)) vals[cnt++] = v;
        }
        if (cnt > 0) qsort(vals, cnt, sizeof(double), dcmp);

        Array *result = create_array((int[]){nq}, 1, FLOAT64);
        if (!result) { free(vals); return NULL; }
        double *rd = (double*)result->data;
        for (int i = 0; i < nq; i++)
            rd[i] = nan_val(vals, cnt, qd[i], pct_mode);
        free(vals);
        return result;
    }

    /* along axis */
    int axis_len = a->shape[axis];
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
    od = 0;
    for (int i = 0; i < ndim; i++)
        if (i != axis) { outer_dims[od] = i; outer_shape_arr[od] = a->shape[i]; od++; }

    int num_slices = 1;
    for (int i = 0; i < outer_ndim; i++) num_slices *= outer_shape_arr[i];

    double *slice_vals = malloc(axis_len * sizeof(double));

    for (int s = 0; s < num_slices; s++) {
        int base = 0;
        for (int j = 0; j < outer_ndim; j++)
            base += outer_indices[j] * strides[outer_dims[j]];

        /* Collect non-NaN values */
        int step = strides[axis];
        int cnt = 0;
        for (int k = 0; k < axis_len; k++) {
            double v = get_value(data + (base + k * step) * elem_sz, a->dtype);
            if (!isnan(v)) slice_vals[cnt++] = v;
        }
        if (cnt > 0) qsort(slice_vals, cnt, sizeof(double), dcmp);

        int out_base = 0;
        for (int j = 0; j < outer_ndim; j++)
            out_base += outer_indices[j] * res_strides[j + 1];

        for (int qi = 0; qi < nq; qi++) {
            res_data[out_base + qi * res_strides[0]] =
                nan_val(slice_vals, cnt, qd[qi], pct_mode);
        }

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

Array* nanpercentile(Array *a, Array *q, int axis) {
    return nan_percentile_core(a, q, axis, 1);
}

Array* nanquantile(Array *a, Array *q, int axis) {
    return nan_percentile_core(a, q, axis, 0);
}
