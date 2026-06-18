#include <stdio.h>
#include <stdlib.h>
#include "array.h"
Array* expand_dims(Array *arr, int axis) {
    if (arr == NULL) return NULL;
    int ndim = arr->ndim;
    if (axis < 0) axis = ndim + 1; // NumPy behavior: axis can equal ndim (prepend at the end)
    if (axis < 0 || axis > ndim) {
        fprintf(stderr, "expand_dims: axis %d out of range for ndim %d\n", axis, ndim);
        return NULL;
    }
    int new_ndim = ndim + 1;
    int *new_shape = malloc(new_ndim * sizeof(int));
    if (new_shape == NULL) return NULL;
    for (int i = 0; i < axis; i++) new_shape[i] = arr->shape[i];
    new_shape[axis] = 1;
    for (int i = axis; i < ndim; i++) new_shape[i+1] = arr->shape[i];
    // create view (shares the underlying data)
    Array *view = malloc(sizeof(Array));
    if (view == NULL) { free(new_shape); return NULL; }
    view->data = arr->data;
    view->dtype = arr->dtype;
    view->ndim = new_ndim;
    view->shape = new_shape;
    view->size = arr->size;
    view->strides = NULL; // strides not yet supported, simplified
    view->is_view = 1;
    return view;
}

// helper: check if array shapes match (not considering cases where one array has fewer dimensions, need to broadcast? but stack requires exact match)
