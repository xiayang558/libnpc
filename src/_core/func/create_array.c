#include "array.h"


Array* create_array(int *shape, int ndim, DataType dtype) {
    if (shape == NULL || ndim <= 0) {
        fprintf(stderr, "create_array: Invalid shape or dimensions\n");
        return NULL;
    }

    // Compute total number of elements, allow ndim of 0
    int size = 1;
    for (int i = 0; i < ndim; i++) {
        if (shape[i] < 0) {
            fprintf(stderr, "create_array: Shape dimension cannot be negative\n");
            return NULL;
        }
        size *= shape[i];
    }

    // Allocate array struct
    Array *arr = (Array*)malloc(sizeof(Array));
    if (arr == NULL) {
        fprintf(stderr, "create_array: Memory allocation failed (struct)\n");
        return NULL;
    }

    // Allocate data memory (when size == 0, set data to NULL)
    size_t element_size = dtype_size(dtype);
    if (size == 0) {
        arr->data = NULL;
    } else {
        arr->data = malloc(size * element_size);
        if (arr->data == NULL) {
            fprintf(stderr, "create_array: Memory allocation failed (data)\n");
            free(arr);
            return NULL;
        }
    }

    // Allocate shape array and copy
    arr->shape = (int*)malloc(ndim * sizeof(int));
    if (arr->shape == NULL) {
        fprintf(stderr, "create_array: Memory allocation failed (shape)\n");
        if (arr->data) free(arr->data);
        free(arr);
        return NULL;
    }
    memcpy(arr->shape, shape, ndim * sizeof(int));

    // Set other attributes
    arr->dtype = dtype;
    arr->ndim = ndim;
    arr->size = size;
    arr->strides = NULL;   // optional, keep as-is
    arr->is_view = 0;

    return arr;
}