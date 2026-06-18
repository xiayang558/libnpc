#include "array.h"


// Compute flat offset from multi-dimensional indices
int compute_offset(Array *arr, int *indices) {
    if (arr == NULL || indices == NULL) {
        return -1;
    }

    int offset = 0;
    int stride = 1;

    // Start computing from the last dimension
    for (int i = arr->ndim - 1; i >= 0; i--) {
        if (indices[i] < 0 || indices[i] >= arr->shape[i]) {
            fprintf(stderr, "Index out of bounds\n");
            return -1;
        }
        offset += indices[i] * stride;
        stride *= arr->shape[i];
    }

    return offset;
}