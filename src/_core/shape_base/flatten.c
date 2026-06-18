#include "def.h"
#include "array.h"
#include "shape.h"
Array* flatten(Array *arr) {
    if (arr == NULL) { fprintf(stderr, "flatten: NULL array argument\n"); return NULL; }
    int shape[1] = {arr->size};
    Array *flat = create_array(shape, 1, arr->dtype);
    if (flat == NULL) return NULL;
    if (arr->size > 0)
        memcpy(flat->data, arr->data, arr->size * dtype_size(arr->dtype));
    return flat;
}
