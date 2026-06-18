#include <stdlib.h>
#include "array.h"

/**
 * rfftfreq — Real FFT sample frequencies (like numpy.fft.rfftfreq)
 *
 * Return the non-negative frequency bins corresponding to rfft output.
 *
 * @param n  Window length
 * @param d sample spacing，default 1.0
 * @return 1D FLOAT64 frequency array of length n/2 + 1
 */
Array* rfftfreq(int n, double d) {
    if (n < 1) n = 1;
    if (d <= 0.0) d = 1.0;

    int len = n / 2 + 1;
    int shape[1] = {len};
    Array *result = create_array(shape, 1, FLOAT64);
    if (!result) return NULL;

    double *data = (double*)result->data;
    double denom = (double)n * d;

    for (int i = 0; i < len; i++)
        data[i] = (double)i / denom;

    return result;
}
