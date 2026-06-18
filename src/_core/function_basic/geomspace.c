#include "array.h"


Array* geomspace(double start, double stop, int num, int endpoint) {
    if (num <= 0) {
        fprintf(stderr, "geomspace: num must be positive\n");
        return NULL;
    }
    if (start == 0.0 || stop == 0.0) {
        fprintf(stderr, "geomspace: start and stop must be non-zero\n");
        return NULL;
    }
    if ((start > 0 && stop < 0) || (start < 0 && stop > 0)) {
        fprintf(stderr, "geomspace: start and stop must have the same sign\n");
        return NULL;
    }

    // Actual number of points to generate internally (including stop or not)
    int total = endpoint ? num : num + 1;
    if (total < 2) total = 2; // Need at least two points to compute ratio

    // Create result array, output size is fixed at num
    int shape[1] = {num};
    Array *res = create_array(shape, 1, FLOAT64);
    if (!res) return NULL;
    double *data = (double*)res->data;

    if (num == 1) {
        data[0] = start;
        return res;
    }

    double ratio = stop / start;
    if (ratio <= 0) {
        fprintf(stderr, "geomspace: ratio must be positive\n");
        free_array(res);
        return NULL;
    }

    double step = 1.0 / (total - 1);
    for (int i = 0; i < total; i++) {
        double exponent = i * step;
        double val = start * pow(ratio, exponent);
        if (i < num) {
            data[i] = val;
        }
        // When endpoint=0, the last point will be discarded (not written to result)
    }
    return res;
}
