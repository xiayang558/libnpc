#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <complex.h>
#include "array.h"
#include "utils.h"

static int is_pow2(int n) { return n > 0 && (n & (n - 1)) == 0; }

static void bit_reverse(complex double *data, int n) {
    int j = 0;
    for (int i = 0; i < n; i++) {
        if (i < j) { complex double t = data[i]; data[i] = data[j]; data[j] = t; }
        int bit = n >> 1;
        for (; bit > 0 && (j & bit); bit >>= 1) j ^= bit;
        j ^= bit;
    }
}

/* Direct inverse DFT: x[n] = (1/N) * sum(X[k] * exp(+2πi*k*n/N)) */
static void direct_idft(complex double *data, int n) {
    complex double *copy = malloc(n * sizeof(complex double));
    if (!copy) return;
    memcpy(copy, data, n * sizeof(complex double));

    for (int k = 0; k < n; k++) {
        complex double sum = 0;
        for (int m = 0; m < n; m++) {
            double angle = 2.0 * M_PI * k * m / n;
            sum += copy[m] * (cos(angle) + sin(angle) * I);
        }
        data[k] = sum / n;
    }
    free(copy);
}

/**
 * ifft — Inverse Fast Fourier Transform (like numpy.fft.ifft)
 *
 * Power-of-2: Cooley-Tukey radix-2 DIT (positive angle + divide by N)
 * Non-power-of-2: direct IDFT (O(N^2))
 *
 * @param arr 1D complex array (COMPLEX128)
 * @return 1D COMPLEX128 time-domain array, same length as input
 */
Array* ifft(Array *arr) {
    if (arr == NULL) {
        fprintf(stderr, "ifft: NULL argument\n");
        return NULL;
    }
    if (arr->ndim != 1) {
        fprintf(stderr, "ifft: input must be 1D\n");
        return NULL;
    }

    int n = arr->size;
    size_t in_sz = dtype_size(arr->dtype);
    char *src = (char*)arr->data;

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
        bit_reverse(data, n);
        for (int len = 2; len <= n; len <<= 1) {
            double angle = 2.0 * M_PI / len;  /* + for inverse */
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
        /* Divide by N */
        for (int i = 0; i < n; i++) data[i] /= n;
    } else {
        direct_idft(data, n);
    }

    int out_shape[1] = {n};
    Array *result = create_array(out_shape, 1, COMPLEX128);
    if (!result) { free(data); return NULL; }

    memcpy(result->data, data, n * sizeof(complex double));
    free(data);
    return result;
}
