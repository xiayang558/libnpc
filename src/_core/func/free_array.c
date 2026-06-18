#include "array.h"


// freearray
void free_array(Array *arr) {
    if (arr == NULL) {
        return;
    }

    if (arr->data != NULL && !arr->is_view) {
        free(arr->data);
    }

    if (arr->shape != NULL) {
        free(arr->shape);
    }

    free(arr);
}