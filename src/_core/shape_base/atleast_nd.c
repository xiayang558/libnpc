#include "def.h"
#include "array.h"
#include "shape.h"

Array* atleast_nd(Array *arr, int target_ndim) {
    if (arr == NULL) {
        fprintf(stderr, "atleast_%dd: NULL array argument\n", target_ndim);
        return NULL;
    }
    if (arr->ndim >= target_ndim) {
        Array *view = create_array(arr->shape, arr->ndim, arr->dtype);
        if (view == NULL) {
            fprintf(stderr, "atleast_%dd: Memory allocation failed\n", target_ndim);
            return NULL;
        }
        view->data = arr->data;
        view->dtype = arr->dtype;
        view->ndim = arr->ndim;
        view->size = arr->size;
        view->shape = (int*)malloc(arr->ndim * sizeof(int));
        if (view->shape == NULL) {
            free(view);
            fprintf(stderr, "atleast_%dd: Memory allocation failed\n", target_ndim);
            return NULL;
        }
        memcpy(view->shape, arr->shape, arr->ndim * sizeof(int));
        view->strides = NULL;
        view->is_view = TRUE;
        return view;
    }
    int new_ndim = target_ndim;
    int *new_shape = (int*)malloc(new_ndim * sizeof(int));
    if (new_shape == NULL) {
        fprintf(stderr, "atleast_%dd: Memory allocation failed\n", target_ndim);
        return NULL;
    }
    for (int i = arr->ndim; i < target_ndim; i++) new_shape[i] = 1;
    for (int i = 0; i < arr->ndim; i++) new_shape[i] = arr->shape[i];
    Array *view = create_array(new_shape, new_ndim, arr->dtype);
    if (view == NULL) { free(new_shape); return NULL; }
    view->data = arr->data;
    view->dtype = arr->dtype;
    view->ndim = new_ndim;
    view->size = arr->size;
    view->shape = new_shape;
    view->strides = NULL;
    view->is_view = TRUE;
    return view;
}
