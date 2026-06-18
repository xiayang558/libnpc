#include "def.h"
#include "array.h"
#include "shape.h"

/**
 * resize  --  Return a new array with the given shape (like numpy.resize)
 *
 * Difference from reshape:
 *   resize   --  can change total element count; repeats cyclically if not enough,
 *                truncates if too many (always copy)
 *   reshape  --  requires total element count unchanged
 *
 * @param arr       Input array
 * @param new_shape new shape
 * @param new_ndim  number of new dimensions
 * @return new array with the requested shape (copy)
 */
Array* resize(Array *arr, int *new_shape, int new_ndim) {
    if (arr == NULL) {
        fprintf(stderr, "resize: NULL array argument\n");
        return NULL;
    }
    if (new_shape == NULL || new_ndim <= 0) {
        fprintf(stderr, "resize: Invalid new_shape\n");
        return NULL;
    }

    /* Validate and compute new size */
    int new_size = 1;
    for (int i = 0; i < new_ndim; i++) {
        if (new_shape[i] <= 0) {
            fprintf(stderr, "resize: Shape must be >= 0, got shape[%d] = %d\n", i, new_shape[i]);
            return NULL;
        }
        new_size *= new_shape[i];
    }

    Array *new_arr = create_array(new_shape, new_ndim, arr->dtype);
    if (new_arr == NULL) return NULL;

    size_t elem_sz = dtype_size(arr->dtype);
    char *src = (char*)arr->data;
    char *dst = (char*)new_arr->data;

    if (new_size <= arr->size) {
        /* New size smaller or equal: just copy first elements */
        memcpy(dst, src, new_size * elem_sz);
    } else {
        /* New size larger: repeat src cyclically */
        for (int i = 0; i < new_size; i++) {
            memcpy(dst + i * elem_sz, src + (i % arr->size) * elem_sz, elem_sz);
        }
    }

    return new_arr;
}
