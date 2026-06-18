#include "def.h"
#include "array.h"
#include "shape.h"
Array** dsplit(Array *ary, Array *indices_or_sections, int *num_splits) {
    if (ary == NULL) { fprintf(stderr, "dsplit: NULL array argument\n"); return NULL; }
    if (ary->ndim < 3) { fprintf(stderr, "dsplit: array must have at least 3 dimensions\n"); return NULL; }
    return split(ary, indices_or_sections, 2, num_splits);
}
