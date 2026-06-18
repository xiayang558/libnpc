#include "def.h"
#include "array.h"
#include "shape.h"
Array* stack(Array **arrays, int num_arrays, int axis) {
    if (arrays == NULL || num_arrays <= 0) { fprintf(stderr, "stack: invalid input\n"); return NULL; }
    if (!all_shapes_equal(arrays, num_arrays)) { fprintf(stderr, "stack: all input arrays must have the same shape\n"); return NULL; }
    Array **expanded = malloc(num_arrays * sizeof(Array*));
    if (!expanded) return NULL;
    for (int i = 0; i < num_arrays; i++) { expanded[i] = expand_dims(arrays[i], axis); if (expanded[i] == NULL) { for (int j = 0; j < i; j++) free_array(expanded[j]); free(expanded); return NULL; } }
    Array *result = concatenate(expanded[0], expanded[1], axis);
    for (int i = 2; i < num_arrays; i++) { Array *tmp = concatenate(result, expanded[i], axis); free_array(result); result = tmp; }
    for (int i = 0; i < num_arrays; i++) { if (expanded[i]->shape) free(expanded[i]->shape); free(expanded[i]); }
    free(expanded);
    return result;
}
