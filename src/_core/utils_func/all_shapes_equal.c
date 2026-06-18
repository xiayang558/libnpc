#include "array.h"
int all_shapes_equal(Array **arrays, int n) {
    if (n <= 0) return 0;
    int ndim = arrays[0]->ndim;
    for (int i = 0; i < n; i++) {
        if (arrays[i]->ndim != ndim) return 0;
        for (int d = 0; d < ndim; d++) {
            if (arrays[i]->shape[d] != arrays[0]->shape[d]) return 0;
        }
    }
    return 1;
}


