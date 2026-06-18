#include <stdlib.h>
#include "array.h"

void compute_elem_strides(const int *shape, int ndim, int *strides) {
    if (ndim == 0) return;
    strides[ndim - 1] = 1;
    for (int i = ndim - 2; i >= 0; i--) {
        strides[i] = strides[i + 1] * shape[i + 1];
    }
}
