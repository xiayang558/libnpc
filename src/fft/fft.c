#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <complex.h>
#include "array.h"
#include "utils.h"

/* Check if n is a power of 2 */
static int is_pow2(int n) { return n > 0 && (n & (n - 1)) == 0; }

/* Bit-reversal permutation */
static void bit_reverse(complex double *data, int n) {
    int j = 0;
    for (int i = 0; i < n; i++) {
        if (i < j) { complex double t = data[i]; data[i] = data[j]; data[j] = t; }
        int bit = n >> 1;
        for (; bit > 0 && (j & bit); bit >>= 1) j ^= bit;
        j ^= bit;
    }
}

/* Direct DFT: X[k] = sum(x[n] * exp(-2πi*k*n/N)), k = 0..N-1 */
static void direct_dft(complex double *data, int n) {
    complex double *copy = malloc(n * sizeof(complex double));
    if (!copy) return;
    memcpy(copy, data, n * sizeof(complex double));

    for (int k = 0; k < n; k++) {
        complex double sum = 0;
        for (int m = 0; m < n; m++) {
            double angle = -2.0 * M_PI * k * m / n;
            sum += copy[m] * (cos(angle) + sin(angle) * I);
        }
        data[k] = sum;
    }
    free(copy);
}

/**
 * fft — Fast Fourier Transform (like numpy.fft.fft)
 *
 * Power-of-2: Cooley-Tukey radix-2 DIT
 * Non-power-of-2: direct DFT (O(N^2), numerically accurate)
 *
 * @param arr 1D input array (any numeric type)
 * @return 1D COMPLEX128 frequency-domain array, same length as input
 */
Array* fft(Array *arr) {
    if (arr == NULL) {
        fprintf(stderr, "fft: NULL argument\n");
        return NULL;
    }
    if (arr->ndim != 1) {
        fprintf(stderr, "fft: input must be 1D\n");
        return NULL;
    }

    int n = arr->size;
    size_t in_sz = dtype_size(arr->dtype);
    char *src = (char*)arr->data;

    /* Copy input as complex double */
    complex double *data = malloc(n * sizeof(complex double));
    if (!data) return NULL;

    for (int i = 0; i < n; i++) {
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

    if (is_pow2(n)) {
        /* Radix-2 Cooley-Tukey DIT FFT */
        bit_reverse(data, n);
        for (int len = 2; len <= n; len <<= 1) {
            double angle = -2.0 * M_PI / len;
            complex double wlen = cos(angle) + sin(angle) * I;
            for (int i = 0; i < n; i += len) {
                complex double w = 1.0;
                for (int j = 0; j < len / 2; j++) {
                    complex double u = data[i + j];
                    complex double v = data[i + j + len / 2] * w;
                    data[i + j] = u + v;
                    data[i + j + len / 2] = u - v;
                    w *= wlen;
                }
            }
        }
    } else {
        /* Direct DFT */
        direct_dft(data, n);
    }

    int out_shape[1] = {n};
    Array *result = create_array(out_shape, 1, COMPLEX128);
    if (!result) { free(data); return NULL; }

    memcpy(result->data, data, n * sizeof(complex double));
    free(data);
    return result;
}
