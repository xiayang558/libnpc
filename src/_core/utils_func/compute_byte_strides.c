#include <stddef.h>
#include "array.h"
void compute_byte_strides(const int *shape, int ndim, size_t elem_size, size_t *strides) {
    if (ndim == 0) return;
    strides[ndim - 1] = elem_size;
    for (int i = ndim - 2; i >= 0; i--) {
        strides[i] = strides[i + 1] * shape[i + 1];
    }
}
