#include "array.h"


// Printarrayinfo
void array_info(Array *arr) {
    if (arr == NULL) {
        fprintf(stderr, "NULL array argument\n");
        return;
    }

    printf("Array Info:\n");
    printf("  Type: %s\n", dtype_name(arr->dtype));
    printf("  Shape: (");
    for (int i = 0; i < arr->ndim; i++) {
        printf("%d", arr->shape[i]);
        if (i < arr->ndim - 1) {
            printf(", ");
        }
    }
    printf(")\n");
    printf("  Size: %d\n", arr->size);
}