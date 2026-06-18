#include <stdlib.h>
#include <string.h>
#include <complex.h>
#include "array.h"
#include "utils.h"

/*
 * Extract a 1D slice along `axis` from `data` with shape `shape` (ndim dims).
 * Pad/truncate to `new_len`, apply IFFT, write to `dst` with `new_shape`.
 */
static int ifft_along_axis_pad(complex double *data, const int *shape,
                                complex double *dst, const int *new_shape,
                                int ndim, int axis, int new_len) {
    int axis_len = shape[axis];
    int n_slices = 1;
    for (int d = 0; d < ndim; d++)
        if (d != axis) n_slices *= shape[d];

    int *strides = malloc(ndim * sizeof(int));
    int *new_strides = malloc(ndim * sizeof(int));
    if (!strides || !new_strides) { free(strides); free(new_strides); return -1; }
    compute_strides(shape, ndim, strides);
    compute_strides(new_shape, ndim, new_strides);

    complex double *slice_padded = malloc(new_len * sizeof(complex double));
    if (!slice_padded) { free(strides); free(new_strides); return -1; }

    for (int s = 0; s < n_slices; s++) {
        /* Decode slice index to offset in source buffer */
        int remaining = s;
        int src_off = 0;
        for (int d = ndim - 1; d >= 0; d--) {
            if (d == axis) continue;
            int idx = remaining % shape[d];
            remaining /= shape[d];
            src_off += idx * strides[d];
        }

        /* Decode the same slice index to offset in destination buffer */
        remaining = s;
        int dst_off = 0;
        for (int d = ndim - 1; d >= 0; d--) {
            if (d == axis) continue;
            int idx = remaining % shape[d]; /* same idx, shape unchanged except axis */
            remaining /= shape[d];
            dst_off += idx * new_strides[d];
        }

        /* Pad/truncate along axis */
        int copy_len = (axis_len < new_len) ? axis_len : new_len;
        for (int k = 0; k < copy_len; k++)
            slice_padded[k] = data[src_off + k * strides[axis]];
        for (int k = copy_len; k < new_len; k++)
            slice_padded[k] = 0.0 + 0.0 * I;

        /* IFFT */
        Array *sl_arr = create_array((int[]){new_len}, 1, COMPLEX128);
        if (!sl_arr) { free(slice_padded); free(strides); free(new_strides); return -1; }
        memcpy(sl_arr->data, slice_padded, new_len * sizeof(complex double));
        Array *ifft_sl = ifft(sl_arr);
        free_array(sl_arr);
        if (!ifft_sl) { free(slice_padded); free(strides); free(new_strides); return -1; }

        /* Write back */
        complex double *fs = (complex double*)ifft_sl->data;
        for (int k = 0; k < new_len; k++)
            dst[dst_off + k * new_strides[axis]] = fs[k];
        free_array(ifft_sl);
    }

    free(slice_padded);
    free(strides);
    free(new_strides);
    return 0;
}

/**
 * irfftn — N-D Inverse Real FFT (like numpy.fft.irfftn)
 *
 * Apply 1D IFFT on non-last axes, 1D IRFFT on last (half-spectrum) axis.
 *
 * @param arr N-D half-spectrum array (output of rfftn)
 * @param s   Per-axis output length; -1 for default. Last axis defaults to 2*(size-1)
 * @return N-D FLOAT64 real array
 */
Array* irfftn(Array *arr, int *s) {
    if (arr == NULL) {
        fprintf(stderr, "irfftn: NULL argument\n");
        return NULL;
    }
    int ndim = arr->ndim;
    if (ndim == 0) {
        fprintf(stderr, "irfftn: input must have at least 1 dimension\n");
        return NULL;
    }

    /* Delegate 1D and 2D */
    if (ndim == 1) {
        int n = (s && s[0] > 0) ? s[0] : -1;
        return irfft(arr, n);
    }
    if (ndim == 2) {
        int s2[2] = {
            (s && s[0] > 0) ? s[0] : -1,
            (s && s[1] > 0) ? s[1] : -1
        };
        return irfft2(arr, s2);
    }

    /* Compute output shape */
    int *out_shape = malloc(ndim * sizeof(int));
    if (!out_shape) return NULL;
    for (int d = 0; d < ndim; d++) {
        if (s && s[d] > 0)
            out_shape[d] = s[d];
        else if (d == ndim - 1)
            out_shape[d] = 2 * (arr->shape[d] - 1);
        else
            out_shape[d] = arr->shape[d];
        if (out_shape[d] < 1) out_shape[d] = 1;
    }

    /* Compute total sizes */
    int in_size = arr->size;
    int out_full_size = 1;
    for (int d = 0; d < ndim; d++)
        out_full_size *= out_shape[d];

    /* Copy input as COMPLEX128 */
    size_t in_sz = dtype_size(arr->dtype);
    char *src = (char*)arr->data;
    complex double *data = malloc(in_size * sizeof(complex double));
    if (!data) { free(out_shape); return NULL; }
    for (int i = 0; i < in_size; i++) {
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

    /* Process non-last axes (ndim-2 down to 0): pad + IFFT */
    complex double *cur_data = data;
    int *cur_shape = malloc(ndim * sizeof(int));
    if (!cur_shape) { free(data); free(out_shape); return NULL; }
    memcpy(cur_shape, arr->shape, ndim * sizeof(int));

    for (int axis = ndim - 2; axis >= 0; axis--) {
        int new_len = out_shape[axis];

        /* Compute new total size */
        int new_total = 1;
        for (int d = 0; d < ndim; d++)
            new_total *= (d == axis) ? new_len : cur_shape[d];

        int new_shape_tmp[ndim];
        memcpy(new_shape_tmp, cur_shape, ndim * sizeof(int));
        new_shape_tmp[axis] = new_len;

        complex double *new_data = malloc(new_total * sizeof(complex double));
        if (!new_data) { free(cur_shape); free(cur_data); free(out_shape); return NULL; }

        if (ifft_along_axis_pad(cur_data, cur_shape, new_data, new_shape_tmp,
                                 ndim, axis, new_len) != 0) {
            free(new_data); free(cur_shape); free(cur_data); free(out_shape); return NULL;
        }

        if (cur_data != data) free(cur_data);
        cur_data = new_data;
        cur_shape[axis] = new_len;
    }

    /* Process last axis (ndim-1): IRFFT → real output */
    int last_len = cur_shape[ndim - 1];
    int new_last_len = out_shape[ndim - 1];
    int n_slices = 1;
    for (int d = 0; d < ndim - 1; d++)
        n_slices *= cur_shape[d];

    int *strides = malloc(ndim * sizeof(int));
    int *out_strides = malloc(ndim * sizeof(int));
    if (!strides || !out_strides) {
        free(strides); free(out_strides);
        free(cur_data); free(cur_shape); free(out_shape); return NULL;
    }
    compute_strides(cur_shape, ndim, strides);
    compute_strides(out_shape, ndim, out_strides);

    double *real_data = malloc(out_full_size * sizeof(double));
    if (!real_data) {
        free(strides); free(out_strides);
        free(cur_data); free(cur_shape); free(out_shape); return NULL;
    }

    for (int s = 0; s < n_slices; s++) {
        /* Decode slice index to offset */
        int remaining = s;
        int src_off = 0;
        for (int d = ndim - 2; d >= 0; d--) {
            int idx = remaining % cur_shape[d];
            remaining /= cur_shape[d];
            src_off += idx * strides[d];
        }

        int dst_off = 0;
        remaining = s;
        for (int d = ndim - 2; d >= 0; d--) {
            int idx = remaining % cur_shape[d];
            remaining /= cur_shape[d];
            dst_off += idx * out_strides[d];
        }

        /* Extract slice along last axis */
        complex double *slice = malloc(last_len * sizeof(complex double));
        if (!slice) { free(real_data); free(strides); free(out_strides); free(cur_data); free(cur_shape); free(out_shape); return NULL; }
        for (int k = 0; k < last_len; k++)
            slice[k] = cur_data[src_off + k * strides[ndim - 1]];

        Array *sl_arr = create_array((int[]){last_len}, 1, COMPLEX128);
        if (!sl_arr) { free(slice); free(real_data); free(strides); free(out_strides); free(cur_data); free(cur_shape); free(out_shape); return NULL; }
        memcpy(sl_arr->data, slice, last_len * sizeof(complex double));
        free(slice);

        Array *irfft_sl = irfft(sl_arr, new_last_len);
        free_array(sl_arr);
        if (!irfft_sl) { free(real_data); free(strides); free(out_strides); free(cur_data); free(cur_shape); free(out_shape); return NULL; }

        double *rs = (double*)irfft_sl->data;
        for (int k = 0; k < new_last_len; k++)
            real_data[dst_off + k * out_strides[ndim - 1]] = rs[k];
        free_array(irfft_sl);
    }

    free(cur_data);
    if (cur_data != data) { /* already freed */ }
    free(cur_shape);
    free(strides);
    free(out_strides);

    Array *result = create_array(out_shape, ndim, FLOAT64);
    if (!result) { free(real_data); free(out_shape); return NULL; }
    memcpy(result->data, real_data, out_full_size * sizeof(double));
    free(real_data);
    free(out_shape);
    return result;
}
