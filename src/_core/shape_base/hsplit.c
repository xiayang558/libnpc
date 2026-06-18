#include "def.h"
#include "array.h"
#include "shape.h"
Array** hsplit(Array *ary, Array *indices_or_sections, int *num_splits) {
    if (ary == NULL) { fprintf(stderr, "hsplit: NULL array argument\n"); return NULL; }
    if (ary->ndim == 0) { fprintf(stderr, "hsplit: cannot split 0-d array\n"); return NULL; }
    if (ary->ndim == 1) { fprintf(stderr, "hsplit: split along axis 1 is not supported for 1-d arrays\n"); return NULL; }
    return split(ary, indices_or_sections, 1, num_splits);
}
