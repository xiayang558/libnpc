#include "def.h"
#include "array.h"
#include "shape.h"
Array** vsplit(Array *ary, Array *indices_or_sections, int *num_splits) {
    if (ary == NULL) { fprintf(stderr, "vsplit: NULL array argument\n"); return NULL; }
    if (ary->ndim < 1) { fprintf(stderr, "vsplit: array must have at least 1 dimension\n"); return NULL; }
    return split(ary, indices_or_sections, 0, num_splits);
}
