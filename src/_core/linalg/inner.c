#include "array.h"


Array* inner(Array *arr1, Array *arr2) {
    if (arr1 == NULL || arr2 == NULL) {
        fprintf(stderr, "inner: NULL array argument\n");
        return NULL;
    }
    // create axis=-1 scalar array
    int axis = -1;
    Array *axes_a = create_scalar_array(&axis, INT32);
    Array *axes_b = create_scalar_array(&axis, INT32);
    if (axes_a == NULL || axes_b == NULL) {
        free_array(axes_a);
        free_array(axes_b);
        return NULL;
    }
    Array *result = tensordot(arr1, arr2, axes_a, axes_b);
    free_array(axes_a);
    free_array(axes_b);
    return result;
}