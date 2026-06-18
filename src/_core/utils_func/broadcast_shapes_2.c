#include "array.h"
/* Helper: compute shape after broadcast (see also broadcast_shapes function) */
int* broadcast_shapes_2(Array *arr1, Array *arr2, int *out_ndim) {
    Array *arrays[2] = {arr1, arr2};
    return broadcast_shapes(2, arrays, out_ndim); // falsesetalreadyimplement
}
