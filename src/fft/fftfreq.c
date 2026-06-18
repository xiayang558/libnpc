#include <stdlib.h>
#include "array.h"

/**
 * fftfreq — DFT sample frequencies (like numpy.fft.fftfreq)
 *
 * @param n  Window length
 * @param d sample spacing，default 1.0
 * @return 1D FLOAT64 frequency array of length n
 */
Array* fftfreq(int n, double d) {
    if (n < 1) n = 1;
    if (d <= 0.0) d = 1.0;

    int shape[1] = {n};
    Array *result = create_array(shape, 1, FLOAT64);
    if (!result) return NULL;

    double *data = (double*)result->data;
    double denom = (double)n * d;

    if (n % 2 == 0) {
        int half = n / 2;
        for (int i = 0; i < half; i++)
            data[i] = (double)i / denom;
        for (int i = half; i < n; i++)
            data[i] = (double)(i - n) / denom;
    } else {
        int half = n / 2;
        for (int i = 0; i <= half; i++)
            data[i] = (double)i / denom;
        for (int i = half + 1; i < n; i++)
            data[i] = (double)(i - n) / denom;
    }

    return result;
}
