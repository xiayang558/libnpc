#include <stdint.h>
#include "array.h"
int partition_indices(int64_t *indices, int left, int right,
                      void *base, size_t stride, DataType dtype) {
    // select the rightmost element as pivot
    int pivot_idx = right;
    int64_t pivot_val_idx = indices[pivot_idx];
    double pivot_val = get_value_as_double((char*)base + pivot_val_idx * stride, dtype);
    int i = left;
    for (int j = left; j < right; j++) {
        double cur_val = get_value_as_double((char*)base + indices[j] * stride, dtype);
        if (cur_val < pivot_val) {
            // swap indices[i] and indices[j]
            int64_t tmp = indices[i];
            indices[i] = indices[j];
            indices[j] = tmp;
            i++;
        }
    }
    // place pivot at its correct position
    int64_t tmp = indices[i];
    indices[i] = indices[pivot_idx];
    indices[pivot_idx] = tmp;
    return i;
}

// quickselect recursive implementation
void quickselect(int64_t *indices, int left, int right, int k,
                 void *base, size_t stride, DataType dtype) {
    if (left >= right) return;
    int pivot_pos = partition_indices(indices, left, right, base, stride, dtype);
    if (k == pivot_pos) return;
    else if (k < pivot_pos)
        quickselect(indices, left, pivot_pos - 1, k, base, stride, dtype);
    else
        quickselect(indices, pivot_pos + 1, right, k, base, stride, dtype);
}

// convert any numeric value to double for comparison (simplified version)
double get_value_as_double(void *ptr, DataType dtype) {
    switch (dtype) {
        case INT8:    return (double)*(int8_t*)ptr;
        case INT16:   return (double)*(int16_t*)ptr;
        case INT32:   return (double)*(int32_t*)ptr;
        case INT64:   return (double)*(int64_t*)ptr;
        case UINT8:   return (double)*(uint8_t*)ptr;
        case UINT16:  return (double)*(uint16_t*)ptr;
        case UINT32:  return (double)*(uint32_t*)ptr;
        case UINT64:  return (double)*(uint64_t*)ptr;
        case FLOAT32: return (double)*(float*)ptr;
        case FLOAT64: return *(double*)ptr;
        case FLOAT128: return (double)*(long double*)ptr;
        default: return 0.0;
    }
}

