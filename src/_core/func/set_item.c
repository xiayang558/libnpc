#include "array.h"


// setarrayelement
void set_item(Array *arr, int *indices, void *value) {
    if (arr == NULL || indices == NULL || value == NULL) {
        return;
    }

    int offset = compute_offset(arr, indices);
    if (offset < 0) {
        return;
    }

    size_t element_size = dtype_size(arr->dtype);
    memcpy((char*)arr->data + offset * element_size, value, element_size);
}