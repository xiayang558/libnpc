#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "array.h"
#include "utils.h"

Array* heaviside(Array *x, double h) {
    if (x == NULL) { fprintf(stderr, "heaviside: NULL argument\n"); return NULL; }

    size_t esz = dtype_size(x->dtype);
    char *src = (char*)x->data;

    Array *result = create_array(x->shape, x->ndim, FLOAT64);
    if (!result) return NULL;
    double *rd = (double*)result->data;

    for (int i = 0; i < x->size; i++) {
        double v = get_value(src + i * esz, x->dtype);
        if (isnan(v)) rd[i] = NAN;
        else if (v < 0) rd[i] = 0.0;
        else if (v > 0) rd[i] = 1.0;
        else rd[i] = h;
    }
    return result;
}
