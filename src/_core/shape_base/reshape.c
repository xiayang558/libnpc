#include "def.h"
#include "array.h"
#include "shape.h"
Array* reshape(Array *arr, int *new_shape, int new_ndim) {
    if (arr == NULL || new_shape == NULL || new_ndim <= 0) { fprintf(stderr, "Invalid arguments\n"); return NULL; }
    int new_size = 1;
    for (int i = 0; i < new_ndim; i++) { if (new_shape[i] <= 0) { fprintf(stderr, "Number of elements must be positive\n"); return NULL; } new_size *= new_shape[i]; }
    if (new_size != arr->size) { fprintf(stderr, "Total number of elements must remain the same\n"); return NULL; }
    Array *new_arr = create_array(new_shape, new_ndim, arr->dtype);
    if (new_arr == NULL) return NULL;
    new_arr->shape = (int*)malloc(new_ndim * sizeof(int));
    if (new_arr->shape == NULL) { free(new_arr); return NULL; }
    memcpy(new_arr->shape, new_shape, new_ndim * sizeof(int));
    memcpy(new_arr->data, arr->data, new_size * dtype_size(arr->dtype));
    new_arr->dtype = arr->dtype;
    new_arr->ndim = new_ndim;
    new_arr->size = new_size;
    return new_arr;
}
