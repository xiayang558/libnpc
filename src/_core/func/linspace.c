#include "array.h"


// =========================== Create linearly spaced arrays ============================

Array* linspace(float start, float stop, int num) {
    if (num <= 0) {
        fprintf(stderr, "Number of elements must be positive\n");
        return NULL;
    }

    int shape[1] = {num};
    Array *arr = create_array(shape, 1, FLOAT32);
    if (arr == NULL) {
        return NULL;
    }

    float *data = (float*)arr->data;
    
    if (num == 1) {
        data[0] = start;
        return arr;
    }

    float step = (stop - start) / (num - 1);
    for (int i = 0; i < num; i++) {
        data[i] = start + i * step;
    }

    return arr;
}

// Create floating point linearly spaced array (float32)
Array* linspace_float(float start, float stop, int num) {
    return linspace(start, stop, num);
}

// Create floating point linearly spaced array (float64)
Array* linspace_double(double start, double stop, int num) {
    if (num <= 0) {
        fprintf(stderr, "Number of elements must be positive\n");
        return NULL;
    }
    
    int shape[1] = {num};
    Array *arr = create_array(shape, 1, FLOAT64);
    if (arr == NULL) {
        return NULL;
    }
    
    double *data = (double*)arr->data;
    
    if (num == 1) {
        data[0] = start;
        return arr;
    }
    
    double step = (stop - start) / (num - 1);
    for (int i = 0; i < num; i++) {
        data[i] = start + i * step;
    }
    
    return arr;
}

// Create floating point linearly spaced array (float32)
Array* linspace_float32(float start, float stop, int num) {
    return linspace(start, stop, num);
}

// Create floating point linearly spaced array (float64)
Array* linspace_float64(double start, double stop, int num) {
    return linspace_double(start, stop, num);
}
