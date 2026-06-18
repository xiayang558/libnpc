#include "array.h"
#include "shape.h"


Array* fliplr(Array *arr) {
    if (arr == NULL) {
        fprintf(stderr, "fliplr: NULL array argument\n");
        return NULL;
    }
    if (arr->ndim < 2) {
        fprintf(stderr, "fliplr: requires at least 2 dimensions\n");
        return NULL;
    }
    // Directly call flip along axis 1
    return flip(arr, 1);
}