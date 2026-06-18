#include <stdlib.h>
#include <string.h>
#include "array.h"

/**
 * squeeze -- remove length-1 dimensions (like numpy.squeeze)
 */
Array* squeeze(Array *arr, int axis) {
    if (arr == NULL) return NULL;

    /* Count non-1 dims, optionally restricted by axis */
    int new_ndim = 0;
    int *new_shape = malloc(arr->ndim * sizeof(int));
    if (!new_shape) return NULL;

    if (axis >= 0) {
        /* Remove specific axis only if it's size 1 */
        if (axis >= arr->ndim || arr->shape[axis] != 1) {
            /* Axis not squeezable: return copy */
            free(new_shape);
            return copy(arr);
        }
        for (int i = 0; i < arr->ndim; i++)
            if (i != axis) new_shape[new_ndim++] = arr->shape[i];
    } else {
        /* Remove all dims of size 1 */
        for (int i = 0; i < arr->ndim; i++)
            if (arr->shape[i] != 1) new_shape[new_ndim++] = arr->shape[i];
    }

    if (new_ndim == 0) new_shape[new_ndim++] = 1; /* scalar case */

    Array *result = create_array(new_shape, new_ndim, arr->dtype);
    free(new_shape);
    if (!result) return NULL;

    memcpy(result->data, arr->data, arr->size * dtype_size(arr->dtype));
    return result;
}
