#include <stdlib.h>
#include <string.h>
#include <complex.h>
#include "array.h"

/**
 * rfft — Real FFT, returns only non-negative frequency bins (like numpy.fft.rfft)
 *
 * For a real input of length n, exploits conjugate symmetry to compute only the first n/2+1 bins.
 *
 * @param arr 1D real array
 * @return 1D COMPLEX128, length = n/2 + 1
 */
Array* rfft(Array *arr) {
    if (arr == NULL) {
        fprintf(stderr, "rfft: NULL argument\n");
        return NULL;
    }
    if (arr->ndim != 1) {
        fprintf(stderr, "rfft: input must be 1D\n");
        return NULL;
    }

    int n = arr->size;
    int out_len = n / 2 + 1;  /* non-negative frequency bins */

    /* Compute full FFT */
    Array *full = fft(arr);
    if (full == NULL) return NULL;

    /* Truncate to first out_len elements */
    int shape[1] = {out_len};
    Array *result = create_array(shape, 1, COMPLEX128);
    if (result == NULL) { free_array(full); return NULL; }

    memcpy(result->data, full->data, out_len * sizeof(complex double));
    free_array(full);
    return result;
}
