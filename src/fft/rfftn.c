#include <stdlib.h>
#include <string.h>
#include <complex.h>
#include "array.h"
#include "utils.h"

/*
 * Apply 1D FFT to each 1D slice along `axis` in `data` with the given `shape`.
 */
static int fft_along_axis(complex double *data, const int *shape, int ndim, int axis) {
    int axis_len = shape[axis];
    int n_slices = 1;
    for (int d = 0; d < ndim; d++)
        if (d != axis) n_slices *= shape[d];

    int *strides = malloc(ndim * sizeof(int));
    if (!strides) return -1;
    compute_strides(shape, ndim, strides);

    complex double *slice = malloc(axis_len * sizeof(complex double));
    if (!slice) { free(strides); return -1; }

    for (int s = 0; s < n_slices; s++) {
        int remaining = s;
        int base_off = 0;
        for (int d = ndim - 1; d >= 0; d--) {
            if (d == axis) continue;
            int idx = remaining % shape[d];
            remaining /= shape[d];
            base_off += idx * strides[d];
        }
        for (int k = 0; k < axis_len; k++)
            slice[k] = data[base_off + k * strides[axis]];

        Array *sl_arr = create_array((int[]){axis_len}, 1, COMPLEX128);
        if (!sl_arr) { free(slice); free(strides); return -1; }
        memcpy(sl_arr->data, slice, axis_len * sizeof(complex double));
        Array *fft_sl = fft(sl_arr);
        free_array(sl_arr);
        if (!fft_sl) { free(slice); free(strides); return -1; }

        complex double *fs = (complex double*)fft_sl->data;
        for (int k = 0; k < axis_len; k++)
            data[base_off + k * strides[axis]] = fs[k];
        free_array(fft_sl);
    }
    free(slice);
    free(strides);
    return 0;
}

/**
 * rfftn — N-D Real FFT (like numpy.fft.rfftn)
 *
 * Apply full FFT on non-last axes, RFFT on last axis (keep non-negative frequencies).
 *
 * @param arr N-D real array
 * @return N-D COMPLEX128 half-spectrum (last axis = n/2 + 1)
 */
Array* rfftn(Array *arr) {
    if (arr == NULL) {
        fprintf(stderr, "rfftn: NULL argument\n");
        return NULL;
    }
    int ndim = arr->ndim;
    if (ndim == 0) {
        fprintf(stderr, "rfftn: input must have at least 1 dimension\n");
        return NULL;
    }

    /* Delegate 1D and 2D */
    if (ndim == 1) return rfft(arr);
    if (ndim == 2) return rfft2(arr);

    /* Compute output shape */
    int *out_shape = malloc(ndim * sizeof(int));
    if (!out_shape) return NULL;
    for (int d = 0; d < ndim - 1; d++)
        out_shape[d] = arr->shape[d];
    out_shape[ndim - 1] = arr->shape[ndim - 1] / 2 + 1;

    int out_size = 1;
    for (int d = 0; d < ndim; d++)
        out_size *= out_shape[d];

    /* Copy input as COMPLEX128 */
    int size = arr->size;
    size_t in_sz = dtype_size(arr->dtype);
    char *src = (char*)arr->data;

    complex double *data = malloc(size * sizeof(complex double));
    if (!data) { free(out_shape); return NULL; }
    for (int i = 0; i < size; i++) {
        double r = 0, im = 0;
        switch (arr->dtype) {
            case COMPLEX64:
                r = crealf(*(complex float*)(src + i * in_sz));
                im = cimagf(*(complex float*)(src + i * in_sz)); break;
            case COMPLEX128:
                r = creal(*(complex double*)(src + i * in_sz));
                im = cimag(*(complex double*)(src + i * in_sz)); break;
            default:
                r = get_value(src + i * in_sz, arr->dtype); break;
        }
        data[i] = r + im * I;
    }

    /* Step 1: FFT on non-last axes (ndim-2 down to 0) */
    for (int axis = ndim - 2; axis >= 0; axis--) {
        if (fft_along_axis(data, arr->shape, ndim, axis) != 0) {
            free(data); free(out_shape); return NULL;
        }
    }

    /* Step 2: RFFT on last axis (ndim-1) → output shrinks */
    int last_len = arr->shape[ndim - 1];
    int new_last_len = out_shape[ndim - 1];
    int n_slices = 1;
    for (int d = 0; d < ndim - 1; d++)
        n_slices *= arr->shape[d];

    int *strides = malloc(ndim * sizeof(int));
    int *out_strides = malloc(ndim * sizeof(int));
    if (!strides || !out_strides) {
        free(strides); free(out_strides);
        free(data); free(out_shape); return NULL;
    }
    compute_strides(arr->shape, ndim, strides);
    compute_strides(out_shape, ndim, out_strides);

    complex double *out_data = malloc(out_size * sizeof(complex double));
    if (!out_data) {
        free(strides); free(out_strides);
        free(data); free(out_shape); return NULL;
    }

    for (int s = 0; s < n_slices; s++) {
        int remaining = s;
        int src_off = 0;
        for (int d = ndim - 2; d >= 0; d--) {
            int idx = remaining % arr->shape[d];
            remaining /= arr->shape[d];
            src_off += idx * strides[d];
        }

        int dst_off = 0;
        remaining = s;
        for (int d = ndim - 2; d >= 0; d--) {
            int idx = remaining % arr->shape[d];
            remaining /= arr->shape[d];
            dst_off += idx * out_strides[d];
        }

        /* Extract slice along last axis */
        complex double *slice = malloc(last_len * sizeof(complex double));
        if (!slice) { free(out_data); free(strides); free(out_strides); free(data); free(out_shape); return NULL; }
        for (int k = 0; k < last_len; k++)
            slice[k] = data[src_off + k * strides[ndim - 1]];

        Array *sl_arr = create_array((int[]){last_len}, 1, COMPLEX128);
        if (!sl_arr) { free(slice); free(out_data); free(strides); free(out_strides); free(data); free(out_shape); return NULL; }
        memcpy(sl_arr->data, slice, last_len * sizeof(complex double));
        free(slice);

        Array *rfft_sl = rfft(sl_arr);
        free_array(sl_arr);
        if (!rfft_sl) { free(out_data); free(strides); free(out_strides); free(data); free(out_shape); return NULL; }

        complex double *rs = (complex double*)rfft_sl->data;
        for (int k = 0; k < new_last_len; k++)
            out_data[dst_off + k * out_strides[ndim - 1]] = rs[k];
        free_array(rfft_sl);
    }

    free(data);
    free(strides);
    free(out_strides);

    Array *result = create_array(out_shape, ndim, COMPLEX128);
    if (!result) { free(out_data); free(out_shape); return NULL; }
    memcpy(result->data, out_data, out_size * sizeof(complex double));
    free(out_data);
    free(out_shape);
    return result;
}
