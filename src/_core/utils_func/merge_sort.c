#include <stdint.h>
#include "array.h"
int less_than(void *base, size_t stride, DataType dtype,
              int64_t idx_a, int64_t idx_b) {
    void *pa = (char*)base + idx_a * stride;
    void *pb = (char*)base + idx_b * stride;
    switch (dtype) {
        case INT8:    return *(int8_t*)pa < *(int8_t*)pb;
        case INT16:   return *(int16_t*)pa < *(int16_t*)pb;
        case INT32:   return *(int32_t*)pa < *(int32_t*)pb;
        case INT64:   return *(int64_t*)pa < *(int64_t*)pb;
        case UINT8:   return *(uint8_t*)pa < *(uint8_t*)pb;
        case UINT16:  return *(uint16_t*)pa < *(uint16_t*)pb;
        case UINT32:  return *(uint32_t*)pa < *(uint32_t*)pb;
        case UINT64:  return *(uint64_t*)pa < *(uint64_t*)pb;
        case FLOAT32: return *(float*)pa < *(float*)pb;
        case FLOAT64: return *(double*)pa < *(double*)pb;
        case FLOAT128: return *(long double*)pa < *(long double*)pb;
        default:
            return 0;
    }
}

// merge sort merge step
    void merge(int64_t *indices, int left, int mid, int right,
            void *base, size_t stride, DataType dtype,
            int64_t *temp) {
    int i = left, j = mid + 1, k = 0;
    while (i <= mid && j <= right) {
        if (less_than(base, stride, dtype, indices[i], indices[j])) {
            temp[k++] = indices[i++];
        } else {
            temp[k++] = indices[j++];
        }
    }
    while (i <= mid) temp[k++] = indices[i++];
    while (j <= right) temp[k++] = indices[j++];
    for (i = 0; i < k; i++) {
        indices[left + i] = temp[i];
    }
}

// merge sort recursive implementation
void merge_sort_indices(int64_t *indices, int left, int right,
                        void *base, size_t stride, DataType dtype,
                        int64_t *temp) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        merge_sort_indices(indices, left, mid, base, stride, dtype, temp);
        merge_sort_indices(indices, mid + 1, right, base, stride, dtype, temp);
        merge(indices, left, mid, right, base, stride, dtype, temp);
    }
}


// binary search helper function
int64_t find_index(double val, double *bins, int len, int right) {
    // boundary cases
    if (val < bins[0]) return 0;
    if (val > bins[len-1]) return len;
    // binary search
    int left = 0, right_bound = len - 1;
    while (left <= right_bound) {
        int mid = (left + right_bound) / 2;
        if (right) {
            // bin definition: bins[i-1] < x <= bins[i]
            if (val <= bins[mid]) {
                if (mid == 0 || val > bins[mid-1]) return mid;
                right_bound = mid - 1;
            } else {
                left = mid + 1;
            }
        } else {
            // default: bins[i-1] <= x < bins[i]
            if (val < bins[mid]) {
                if (mid == 0 || val >= bins[mid-1]) return mid;
                right_bound = mid - 1;
            } else {
                left = mid + 1;
            }
        }
    }
    return len; // unreachable
}


