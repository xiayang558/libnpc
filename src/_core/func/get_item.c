#include "array.h"


// getarrayelement
void* get_item(Array *arr, int *indices) {
    if (arr == NULL || indices == NULL) {
        return NULL;
    }

    int offset = compute_offset(arr, indices);
    if (offset < 0) {
        return NULL;
    }

    size_t element_size = dtype_size(arr->dtype);
    return (char*)arr->data + offset * element_size;
}