#include "array.h"


// copyarray
Array* copy_array(Array *arr) {
    if (arr == NULL) {
        return NULL;
    }

    Array *new_arr = create_array(arr->shape, arr->ndim, arr->dtype);
    if (new_arr == NULL) {
        return NULL;
    }

    size_t element_size = dtype_size(arr->dtype);
    memcpy(new_arr->data, arr->data, arr->size * element_size);

    return new_arr;
}


Array* copy(Array *arr) {
    if (arr == NULL) {
        fprintf(stderr, "copy: NULL array argument\n");
        return NULL;
    }
    // straightforward: just use existing copy_array function
    return copy_array(arr);
}