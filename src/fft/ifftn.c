#include <stdlib.h>
#include <string.h>
#include <complex.h>
#include "array.h"
#include "utils.h"

static int ifft_along_axis(complex double *data, const int *shape, int ndim, int axis) {
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
        Array *ifft_sl = ifft(sl_arr);
        free_array(sl_arr);
        if (!ifft_sl) { free(slice); free(strides); return -1; }

        complex double *fs = (complex double*)ifft_sl->data;
        for (int k = 0; k < axis_len; k++)
            data[base_off + k * strides[axis]] = fs[k];
        free_array(ifft_sl);
    }
    free(slice);
    free(strides);
    return 0;
}

Array* ifftn(Array *arr) {
    if (arr == NULL) { fprintf(stderr, "ifftn: NULL argument\n"); return NULL; }
    int ndim = arr->ndim;
    if (ndim == 0) { fprintf(stderr, "ifftn: input must have >= 1 dimension\n"); return NULL; }
    if (ndim == 1) return ifft(arr);
    if (ndim == 2) return ifft2(arr);

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
                im = cimagf(*(complex float*)(src + i * in_sz)); break;
            case COMPLEX128:
                r = creal(*(complex double*)(src + i * in_sz));
                im = cimag(*(complex double*)(src + i * in_sz)); break;
            default:
                r = get_value(src + i * in_sz, arr->dtype); break;
        }
        data[i] = r + im * I;
    }

    for (int axis = ndim - 1; axis >= 0; axis--) {
        if (ifft_along_axis(data, arr->shape, ndim, axis) != 0) { free(data); return NULL; }
    }

    Array *result = create_array(arr->shape, ndim, COMPLEX128);
    if (!result) { free(data); return NULL; }
    memcpy(result->data, data, size * sizeof(complex double));
    free(data);
    return result;
}
