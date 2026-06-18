#include "def.h"
#include "array.h"
#include "shape.h"
Array* vstack(Array **arrays, int num_arrays) {
    if (arrays == NULL || num_arrays <= 0) return NULL;
    Array **prepared = malloc(num_arrays * sizeof(Array*));
    if (!prepared) return NULL;
    for (int i = 0; i < num_arrays; i++) { if (arrays[i]->ndim == 1) { int s[2]={1,arrays[i]->size}; prepared[i]=reshape(arrays[i],s,2); } else prepared[i]=copy_array(arrays[i]); if(prepared[i]==NULL){for(int j=0;j<i;j++)free_array(prepared[j]);free(prepared);return NULL;} }
    Array *result = concatenate(prepared[0], prepared[1], 0);
    for (int i = 2; i < num_arrays; i++) { Array *tmp = concatenate(result, prepared[i], 0); free_array(result); result = tmp; }
    for (int i = 0; i < num_arrays; i++) free_array(prepared[i]);
    free(prepared);
    return result;
}
