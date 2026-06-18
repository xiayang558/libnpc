#include "window.h"

Array* kaiser(int M, double beta) {
    if (M < 0) {
        fprintf(stderr, "kaiser: M must be non-negative\n");
        return NULL;
    }
    int shape[1] = {M};
    Array *arr = create_array(shape, 1, FLOAT64);
    if (arr == NULL) return NULL;
    if (M == 0) return arr;
    if (M == 1) {
        *(double*)arr->data = 1.0;
        return arr;
    }
    double *data = (double*)arr->data;
    double denom = bessel_i0(beta);
    double half = (M - 1) / 2.0;
    for (int n = 0; n < M; ++n) {
        double x = (n - half) / half;
        double arg = beta * sqrt(1.0 - x * x);
        data[n] = bessel_i0(arg) / denom;
    }
    return arr;
}