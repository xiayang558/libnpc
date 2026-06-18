#include <stdlib.h>
#include <string.h>
#include <complex.h>
#include "array.h"

/**
 * hfft — Hermitian FFT, reconstructs real signal from half-spectrum (like numpy.fft.hfft)
 *
 * Build full conjugate-symmetric spectrum from half-spectrum X[0..m-1], then apply forward FFT.
 *
 * Algorithm: X_padded = zero-pad/trim X → length n/2+1
 *       X_full = extend with conjugate symmetry
 *       result = fft(X_full)
 *
 * @param arr 1D COMPLEX128 half-spectrum array (output of rfft)
 * @param n   Output length; -1 defaults to 2*(arr->size - 1)
 * @return 1D FLOAT64 real signal
 */
Array* hfft(Array *arr, int n) {
    if (arr == NULL) {
        fprintf(stderr, "hfft: NULL argument\n");
        return NULL;
    }
    if (arr->ndim != 1) {
        fprintf(stderr, "hfft: input must be 1D\n");
        return NULL;
    }
    if (arr->dtype != COMPLEX128) {
        fprintf(stderr, "hfft: input must be COMPLEX128\n");
        return NULL;
    }

    int m = arr->size;
    int N = (n <= 0) ? 2 * (m - 1) : n;
    if (N < 1) N = 1;

    /* Step 1: zero-pad or trim half-spectrum to length N/2+1 */
    int new_m = N / 2 + 1;
    complex double *X_padded = calloc(new_m, sizeof(complex double));
    if (!X_padded) return NULL;

    complex double *src = (complex double*)arr->data;
    int copy_len = (m < new_m) ? m : new_m;
    memcpy(X_padded, src, copy_len * sizeof(complex double));

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

    /* Step 3: compute forward FFT of the full spectrum */
    Array *X_arr = create_array((int[]){N}, 1, COMPLEX128);
    if (!X_arr) { free(X_full); return NULL; }
    memcpy(X_arr->data, X_full, N * sizeof(complex double));
    free(X_full);

    Array *result = fft(X_arr);
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
