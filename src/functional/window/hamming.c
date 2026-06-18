#include <stdlib.h>
#include <math.h>
#include "array.h"

/**
 * hamming  --  Hamming window (like numpy.hamming)
 *
 * w[n] = 0.54 - 0.46 * cos(2π*n / (M-1)), n = 0..M-1
 *
 * @param m Window length (degenerate when <= 1)
 * @return 1D FLOAT64 window array
 */
Array* hamming(int m) {
    if (m < 1) m = 1;

    int shape[1] = {m};
    Array *result = create_array(shape, 1, FLOAT64);
    if (!result) return NULL;

    double *data = (double*)result->data;

    if (m == 1) {
        data[0] = 1.0;
        return result;
    }

    double denom = (double)(m - 1);
    for (int i = 0; i < m; i++)
        data[i] = 0.54 - 0.46 * cos(2.0 * M_PI * i / denom);

    return result;
}
