#include <stdio.h>
#include "array.h"
int check_axis(const Array *arr, int axis, const char* func_name) {
    if (arr == NULL) return FALSE;
    int ndim = arr->ndim;
    int ax = (axis < 0) ? axis + ndim : axis;
    if (ax < 0 || ax >= ndim) { fprintf(stderr, "[Error] %s: invalid axis %d for %d-D array\n", func_name, axis, ndim); return FALSE; }
    return TRUE;
}
