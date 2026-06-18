#include "array.h"
int check_elementwise_op(Array *arr1, Array *arr2) {
    if (arr1 == NULL || arr2 == NULL) { fprintf(stderr, "[Error] check_elementwise_op: NULL array argument\n"); return FALSE; }
    if (arr1->dtype != arr2->dtype) { fprintf(stderr, "[Error] check_elementwise_op: data type mismatch\n"); return FALSE; }
    if (arr1->ndim != arr2->ndim) { fprintf(stderr, "[Error] check_elementwise_op: dimension mismatch\n"); return FALSE; }
    for (int i = 0; i < arr1->ndim; i++) { if (arr1->shape[i] != arr2->shape[i]) { fprintf(stderr, "[Error] check_elementwise_op: shape mismatch at dim %d\n", i); return FALSE; } }
    return TRUE;
}
