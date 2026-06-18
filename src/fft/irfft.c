#include <stdlib.h>
#include <string.h>
#include <complex.h>
#include "array.h"
#include "utils.h"

/**
 * irfft — Inverse Real FFT, reconstructs real signal from half-spectrum (like numpy.fft.irfft)
 *
 * Build full conjugate-symmetric spectrum from half-spectrum X[0..m-1], then apply inverse FFT.
 *
 * Algorithm: X_padded = zero-pad/trim X → length n/2+1
 *       X_full = extend with conjugate symmetry
 *       result = real(ifft(X_full))
 *
 * @param arr 1D half-spectrum array (output of rfft)
 * @param n   Output real signal length; -1 defaults to 2*(arr->size - 1)
 * @return 1D FLOAT64 real signal
 */
Array* irfft(Array *arr, int n) {
    if (arr == NULL) {
        fprintf(stderr, "irfft: NULL argument\n");
        return NULL;
    }
    if (arr->ndim != 1) {
        fprintf(stderr, "irfft: input must be 1D\n");
        return NULL;
    }

    int m = arr->size;
    int N = (n <= 0) ? 2 * (m - 1) : n;
    if (N < 1) N = 1;

    /* Step 1: zero-pad or trim half-spectrum to length N/2+1 */
    int new_m = N / 2 + 1;

    /* Read input values, converting to complex double */
    complex double *X_padded = calloc(new_m, sizeof(complex double));
    if (!X_padded) return NULL;

    size_t in_sz = dtype_size(arr->dtype);
    char *src = (char*)arr->data;
    int copy_len = (m < new_m) ? m : new_m;
    for (int i = 0; i < copy_len; i++) {
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
        X_padded[i] = r + im * I;
    }

    /* Step 2: build full conjugate-symmetric spectrum of length N */
    complex double *X_full = calloc(N, sizeof(complex double));
    if (!X_full) { free(X_padded); return NULL; }

    /* DC is real */
    X_full[0] = creal(X_padded[0]) + 0.0 * I;

    if (N % 2 == 0) {
        /* Even: pairs + Nyquist bin */
        int nyq = N / 2;
        for (int k = 1; k < nyq && k < new_m; k++) {
            X_full[k] = X_padded[k];
            X_full[N - k] = conj(X_padded[k]);
        }
        /* Nyquist is real */
        if (nyq < new_m)
            X_full[nyq] = creal(X_padded[nyq]) + 0.0 * I;
    } else {
        /* Odd: pairs only, no Nyquist */
        for (int k = 1; k < new_m; k++) {
            X_full[k] = X_padded[k];
            X_full[N - k] = conj(X_padded[k]);
        }
    }
    free(X_padded);

    /* Step 3: compute inverse FFT of the full spectrum */
    Array *X_arr = create_array((int[]){N}, 1, COMPLEX128);
    if (!X_arr) { free(X_full); return NULL; }
    memcpy(X_arr->data, X_full, N * sizeof(complex double));
    free(X_full);

    Array *result = ifft(X_arr);
    free_array(X_arr);
    if (!result) return NULL;

    /* Result is complex but imag parts are ~0; convert to FLOAT64 */
    complex double *cd = (complex double*)result->data;
    int out_shape[1] = {N};
    Array *real_out = create_array(out_shape, 1, FLOAT64);
    if (!real_out) { free_array(result); return NULL; }

    double *rd = (double*)real_out->data;
    for (int i = 0; i < N; i++)
        rd[i] = creal(cd[i]);

    free_array(result);
    return real_out;
}
