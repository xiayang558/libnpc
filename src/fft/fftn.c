#include <stdlib.h>
#include <string.h>
#include <complex.h>
#include "array.h"
#include "utils.h"

/*
 * Extract a 1D slice along `axis` at fixed positions in all other dimensions,
 * apply FFT, and write back.
 */
static int fft_along_axis(complex double *data, const int *shape, int ndim, int axis) {
    int axis_len = shape[axis];

    /* Compute total number of slices = product of all dimensions except axis */
    int n_slices = 1;
    for (int d = 0; d < ndim; d++) {
        if (d != axis) n_slices *= shape[d];
    }

    /* Compute strides for each dimension */
    int *strides = malloc(ndim * sizeof(int));
    if (!strides) return -1;
    compute_strides(shape, ndim, strides);

    complex double *slice = malloc(axis_len * sizeof(complex double));
    if (!slice) { free(strides); return -1; }

    for (int s = 0; s < n_slices; s++) {
        /* Decode slice index: flat s → multi-index skipping `axis` */
        int remaining = s;
        int base_off = 0;
        for (int d = ndim - 1; d >= 0; d--) {
            if (d == axis) continue;
            int orig_len = shape[d];
            if (d == 0 && axis != 0) {
                /* First non-axis dimension: use remaining directly */
                /* Actually all non-axis dims contribute to strides */
            }
            int idx = remaining % orig_len;
            remaining /= orig_len;
            base_off += idx * strides[d];
        }

        /* Extract the slice along axis */
        for (int k = 0; k < axis_len; k++) {
            slice[k] = data[base_off + k * strides[axis]];
        }

        /* FFT the slice */
        Array *sl_arr = create_array((int[]){axis_len}, 1, COMPLEX128);
        if (!sl_arr) { free(slice); free(strides); return -1; }
        memcpy(sl_arr->data, slice, axis_len * sizeof(complex double));
        Array *fft_sl = fft(sl_arr);
        free_array(sl_arr);
        if (!fft_sl) { free(slice); free(strides); return -1; }

        /* Write back */
        complex double *fs = (complex double*)fft_sl->data;
        for (int k = 0; k < axis_len; k++) {
            data[base_off + k * strides[axis]] = fs[k];
        }
        free_array(fft_sl);
    }

    free(slice);
    free(strides);
    return 0;
}

/**
 * fftn — N-D Fast Fourier Transform (like numpy.fft.fftn)
 *
 * Apply 1D FFT along each axis, from last to first.
 *
 * @param arr Input array (any ndim, any numeric type)
 * @return COMPLEX128 spectrum, same shape as input
 */
Array* fftn(Array *arr) {
    if (arr == NULL) {
        fprintf(stderr, "fftn: NULL argument\n");
        return NULL;
    }

    int ndim = arr->ndim;
    if (ndim == 0) {
        fprintf(stderr, "fftn: input must have at least 1 dimension\n");
        return NULL;
    }

    /* Special case: 1D and 2D delegate to existing functions */
    if (ndim == 1) return fft(arr);
    if (ndim == 2) return fft2(arr);

    /* Copy input as COMPLEX128 */
    int size = arr->size;
    size_t in_sz = dtype_size(arr->dtype);
    char *src = (char*)arr->data;

    complex double *data = malloc(size * sizeof(complex double));
    if (!data) return NULL;

    for (int i = 0; i < size; i++) {
        double r = 0, im = 0;
        switch (arr->dtype) {
            case COMPLEX64:
                r = crealf(*(complex float*)(src + i * in_sz));
                im = cimagf(*(complex float*)(src + i * in_sz));
                break;
            case COMPLEX128:
                r = creal(*(complex double*)(src + i * in_sz));
                im = cimag(*(complex double*)(src + i * in_sz));
                break;
            default:
                r = get_value(src + i * in_sz, arr->dtype);
                break;
        }
        data[i] = r + im * I;
    }

    /* Apply FFT along each axis, last-to-first */
    for (int axis = ndim - 1; axis >= 0; axis--) {
        if (fft_along_axis(data, arr->shape, ndim, axis) != 0) {
            free(data);
            return NULL;
        }
    }

    Array *result = create_array(arr->shape, ndim, COMPLEX128);
    if (!result) { free(data); return NULL; }
    memcpy(result->data, data, size * sizeof(complex double));
    free(data);
    return result;
}
