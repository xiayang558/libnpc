#include "array.h"

/**
 * ndim — Return number of array dimensions (like numpy.ndim)
 *
 * @param a Input array
 * @return Number of dimensions, or -1 if a is NULL
 */
int ndim(Array *a) {
    if (a == NULL) return -1;
    return a->ndim;
}
