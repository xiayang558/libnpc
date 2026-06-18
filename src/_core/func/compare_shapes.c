#include "array.h"
#include <stdbool.h>

// Check if two arrays have the same shape
bool compare_shapes(Array *arr1, Array *arr2) {
    if (arr1 == NULL || arr2 == NULL) {
        return false;
    }

    if (arr1->ndim != arr2->ndim) {
        return false;
    }

    for (int i = 0; i < arr1->ndim; i++) {
        if (arr1->shape[i] != arr2->shape[i]) {
            return false;
        }
    }

    return true;
}