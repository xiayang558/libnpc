#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "array.h"
#include "utils.h"

Array* unwrap(Array *arr, double discont, int axis) {
    if (arr == NULL) { fprintf(stderr, "unwrap: NULL argument\n"); return NULL; }
    if (discont <= 0.0) discont = M_PI;

    int ndim = arr->ndim;
    if (axis < -1 || axis >= ndim) { fprintf(stderr, "unwrap: invalid axis\n"); return NULL; }

    Array *work = (arr->dtype == FLOAT64) ? copy(arr) : astype(arr, FLOAT64);
    if (!work) return NULL;

    double period = 2.0 * M_PI;
    double *data = (double*)work->data;

    if (axis == -1) {
        /* Save copy to compute diffs against original values */
        double *orig = malloc(work->size * sizeof(double));
        memcpy(orig, data, work->size * sizeof(double));
        double cum = 0.0;
        for (int i = 1; i < work->size; i++) {
            double diff = orig[i] - orig[i-1];
            if (diff > discont) cum -= period;
            else if (diff < -discont) cum += period;
            data[i] = orig[i] + cum;
        }
        free(orig);
    } else {
        int axis_len = work->shape[axis];
        int *strides = malloc(ndim * sizeof(int));
        compute_strides(work->shape, ndim, strides);

        int n_slices = 1;
        for (int d = 0; d < ndim; d++)
            if (d != axis) n_slices *= work->shape[d];

        /* Full copy for original diffs */
        double *orig = malloc(work->size * sizeof(double));
        memcpy(orig, data, work->size * sizeof(double));

        for (int s = 0; s < n_slices; s++) {
            int base_off = 0, rem = s;
            for (int d = ndim - 1; d >= 0; d--) {
                if (d == axis) continue;
                base_off += (rem % work->shape[d]) * strides[d];
                rem /= work->shape[d];
            }

            double cum = 0.0;
            int step = strides[axis];
            for (int k = 1; k < axis_len; k++) {
                int idx_cur = base_off + k * step;
                int idx_prev = base_off + (k-1) * step;
                double diff = orig[idx_cur] - orig[idx_prev];
                if (diff > discont) cum -= period;
                else if (diff < -discont) cum += period;
                data[idx_cur] = orig[idx_cur] + cum;
            }
        }
        free(strides);
        free(orig);
    }

    return work;
}
