#include "def.h"
#include "array.h"
#include "shape.h"
Array* hstack(Array **arrays, int num_arrays) {
    if (arrays == NULL || num_arrays <= 0) return NULL;
    Array **prepared = malloc(num_arrays * sizeof(Array*));
    if (!prepared) return NULL;
    for (int i = 0; i < num_arrays; i++) { prepared[i] = to_2d_row(arrays[i]); if (prepared[i] == NULL) { for (int j = 0; j < i; j++) free_array(prepared[j]); free(prepared); return NULL; } }
    int ndim = prepared[0]->ndim;
    for (int i = 1; i < num_arrays; i++) { if (prepared[i]->ndim != ndim) { fprintf(stderr, "hstack: arrays have different dimensions after conversion\n"); for (int j = 0; j < num_arrays; j++) free_array(prepared[j]); free(prepared); return NULL; } }
    Array *result = concatenate(prepared[0], prepared[1], 1);
    for (int i = 2; i < num_arrays; i++) { Array *tmp = concatenate(result, prepared[i], 1); free_array(result); result = tmp; }
    for (int i = 0; i < num_arrays; i++) free_array(prepared[i]);
    free(prepared);
    return result;
}
