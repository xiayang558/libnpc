#include "def.h"
#include "array.h"
#include "shape.h"
Array* flatnonzero(Array *arr) {
    if (arr == NULL) { fprintf(stderr, "flatnonzero: NULL array argument\n"); return NULL; }
    size_t elem_sz = dtype_size(arr->dtype); char *data = (char*)arr->data;
    int cnt = 0;
    for (int i = 0; i < arr->size; i++) { if (is_nonzero(data + i * elem_sz, arr->dtype)) cnt++; }
    int shape[1] = {cnt};
    Array *res = create_array(shape, 1, INT64);
    if (res == NULL) return NULL;
    int64_t *out = (int64_t*)res->data; int idx = 0;
    for (int i = 0; i < arr->size; i++) { if (is_nonzero(data + i * elem_sz, arr->dtype)) out[idx++] = i; }
    return res;
}
