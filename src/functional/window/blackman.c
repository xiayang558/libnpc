#include "array.h"


Array* blackman(int M) {
    if (M < 0) {
        fprintf(stderr, "blackman: M must be non-negative\n");
        return NULL;
    }

    // Create 1D array
    int shape[1] = {M};
    Array *arr = create_array(shape, 1, FLOAT64);
    if (arr == NULL) {
        return NULL;
    }

    double *data = (double*)arr->data;

    if (M == 0) {
        // Empty array, no data to fill
        return arr;
    }

    if (M == 1) {
        data[0] = 1.0;
        return arr;
    }

    double factor = 2.0 * M_PI / (M - 1);
    for (int i = 0; i < M; i++) {
        double a0 = 0.42;
        double a1 = 0.5 * cos(factor * i);
        double a2 = 0.08 * cos(2.0 * factor * i);
        data[i] = a0 - a1 + a2;  // Formula: 0.42 - 0.5*cos(2πi/(M-1)) + 0.08*cos(4πi/(M-1))
    }

    return arr;
}