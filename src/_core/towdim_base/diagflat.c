#include "def.h"
#include "array.h"
#include "shape.h"
Array* diagflat(Array *arr, int k) {
    if (arr == NULL) { fprintf(stderr, "diagflat: NULL array argument\n"); return NULL; }
    Array *flat = flatten(arr);
    if (!flat) return NULL;
    Array *result = diag(flat, k);
    free_array(flat);
    return result;
}
