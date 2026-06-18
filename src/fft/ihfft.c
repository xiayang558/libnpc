#include <stdlib.h>
#include <string.h>
#include <complex.h>
#include "array.h"
#include "utils.h"

/**
 * ihfft — Inverse Hermitian FFT (like numpy.fft.ihfft)
 *
 * ihfft(x) = conj(rfft(x)) / N
 * Convert real signal to normalized half-spectrum (conjugate symmetric)
 *
 * @param arr 1D real array
 * @param n   Effective signal length (<=0 defaults to arr->size); >0 truncates or zero-pads
 * @return 1D COMPLEX128 normalized half-spectrum, length = n/2 + 1
 */
Array* ihfft(Array *arr, int n) {
    if (arr == NULL) { fprintf(stderr, "ihfft: NULL argument\n"); return NULL; }
    if (arr->ndim != 1) { fprintf(stderr, "ihfft: input must be 1D\n"); return NULL; }

    int N = (n <= 0) ? arr->size : n;
    if (N < 1) N = 1;

    /* Truncate or use full input */
    int use_len = (arr->size < N) ? arr->size : N;
    size_t in_sz = dtype_size(arr->dtype);
    char *src = (char*)arr->data;

    /* Build effective input array: copy use_len elements, zero-pad to N */
    complex double *eff = calloc(N, sizeof(complex double));
    if (!eff) return NULL;
    for (int i = 0; i < use_len; i++) {
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
        eff[i] = r + im * I;
    }

    /* Create array for the effective signal and compute FFT */
    Array *eff_arr = create_array((int[]){N}, 1, COMPLEX128);
    if (!eff_arr) { free(eff); return NULL; }
    memcpy(eff_arr->data, eff, N * sizeof(complex double));
    free(eff);

    Array *full = fft(eff_arr);
    free_array(eff_arr);
    if (!full) return NULL;

    /* Truncate to half-spectrum, conjugate, divide by N */
    int out_len = N / 2 + 1;
    int shape[1] = {out_len};
    Array *result = create_array(shape, 1, COMPLEX128);
    if (!result) { free_array(full); return NULL; }

    complex double *fs = (complex double*)full->data;
    complex double *ds = (complex double*)result->data;
    for (int i = 0; i < out_len; i++) {
        ds[i] = (creal(fs[i]) - cimag(fs[i]) * I) / N; /* conj / N */
    }

    free_array(full);
    return result;
}
