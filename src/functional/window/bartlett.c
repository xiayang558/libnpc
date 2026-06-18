#include "array.h"

Array* bartlett(int M) {
    if (M < 0) {
        fprintf(stderr, "bartlett: M must be non-negative\n");
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

    double denominator = M - 1.0;
    double center = (M - 1.0) / 2.0;

    for (int i = 0; i < M; i++) {
        double val = 1.0 - fabs(i - center) / center;  // Equivalent to: 2/(M-1)*(center - fabs(i-center))
        data[i] = val;
    }

    return arr;
}
