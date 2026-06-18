#include "array.h"

/**
 * asize  --  Return total number of array elements or length of specified axis (like numpy.size)
 *
 * @param arr  Input array
 * @param axis Specified axis, -1 means return total element count
 * @return Total element count or axis length, returns -1 on error
 */
int asize(Array *arr, int axis) {
    if (arr == NULL) {
        fprintf(stderr, "asize: NULL array argument\n");
        return -1;
    }
    if (axis < 0) return arr->size;
    if (axis >= arr->ndim) {
        fprintf(stderr, "asize: axis %d out of bounds for %d-d array\n", axis, arr->ndim);
        return -1;
    }
    return arr->shape[axis];
}
