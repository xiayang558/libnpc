#include "def.h"
#include "array.h"
#include "shape.h"
Array* column_stack(Array **arrays, int num_arrays) {
    if (arrays == NULL || num_arrays <= 0) return NULL;
    Array **col_arrays = malloc(num_arrays * sizeof(Array*));
    if (!col_arrays) return NULL;
    for (int i = 0; i < num_arrays; i++) { if (arrays[i]->ndim == 1) { int s[2]={arrays[i]->size,1}; col_arrays[i]=reshape(arrays[i],s,2); } else col_arrays[i]=copy_array(arrays[i]); if(col_arrays[i]==NULL){for(int j=0;j<i;j++)free_array(col_arrays[j]);free(col_arrays);return NULL;} }
    Array *result = hstack(col_arrays, num_arrays);
    for (int i = 0; i < num_arrays; i++) free_array(col_arrays[i]);
    free(col_arrays);
    return result;
}
