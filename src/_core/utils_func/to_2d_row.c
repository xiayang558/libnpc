#include "shape.h"
Array* to_2d_row(Array *arr) {
    if (arr->ndim == 1) {
        int new_shape[2] = {1, arr->size};
        return reshape(arr, new_shape, 2);
    }
    return copy_array(arr); // Return copy to avoid modifying original shape
}


