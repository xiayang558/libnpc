#include "array.h"


// ---------- logspace function ----------
Array* logspace(double start, double stop, int num, double base, int endpoint) {
    if (num <= 0) {
        fprintf(stderr, "logspace: num must be positive\n");
        return NULL;
    }
    // Compute linearly spaced points (similar to linspace)
    int n = endpoint ? num : num + 1;  // number of interior points
    if (n < 2) n = 2;
    double step = (stop - start) / (n - 1);
    // Create 1D array
    int shape[1] = {num};
    Array *arr = create_array(shape, 1, FLOAT64);
    if (!arr) return NULL;
    double *data = (double*)arr->data;
    for (int i = 0; i < n; ++i) {
        double t = start + i * step;
        double val = pow(base, t);
        if (i < num) data[i] = val;
    }
    return arr;
}