#include "array.h"
#include "shape.h"


Array* flipud(Array *arr) {
    if (arr == NULL) {
        fprintf(stderr, "flipud: NULL array argument\n");
        return NULL;
    }
    if (arr->ndim < 2) {
        fprintf(stderr, "flipud: requires at least 2 dimensions\n");
        return NULL;
    }
    // Directly call flip along axis 0
    return flip(arr, 0);
}