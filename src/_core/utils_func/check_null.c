#include <stdio.h>
#include "array.h"
int check_null(const Array *arr, const char* func_name) {
    if (arr == NULL) { fprintf(stderr, "[Error] %s: NULL array argument\n", func_name); return FALSE; }
    return TRUE;
}
