#include "def.h"
#include "array.h"
#include "shape.h"
Array* transpose(Array *arr) {
    if (arr == NULL) { fprintf(stderr, "NULL array argument\n"); return NULL; }
    int *new_shape = (int*)malloc(arr->ndim * sizeof(int));
    if (new_shape == NULL) { fprintf(stderr, "Memory allocation failed\n"); return NULL; }
    for (int i = 0; i < arr->ndim; i++) new_shape[i] = arr->shape[arr->ndim - 1 - i];
    Array *new_arr = create_array(new_shape, arr->ndim, arr->dtype);
    free(new_shape);
    if (new_arr == NULL) return NULL;
    size_t element_size = dtype_size(arr->dtype);
    if (arr->ndim == 1) { memcpy(new_arr->data, arr->data, arr->size * element_size); return new_arr; }
    if (arr->ndim == 2) {
        int rows = arr->shape[0], cols = arr->shape[1];
        for (int i = 0; i < rows; i++) for (int j = 0; j < cols; j++) {
            int si[2] = {i,j}, di[2] = {j,i};
            memcpy(get_item(new_arr,di), get_item(arr,si), element_size);
        }
        return new_arr;
    }
    int *indices = (int*)malloc(arr->ndim * sizeof(int));
    if (indices == NULL) { free_array(new_arr); return NULL; }
    memset(indices, 0, arr->ndim * sizeof(int));
    for (int i = 0; i < arr->size; i++) {
        int src_offset = compute_offset(arr, indices);
        if (src_offset < 0) { free(indices); free_array(new_arr); return NULL; }
        int dest_indices[8]; for (int j = 0; j < arr->ndim; j++) dest_indices[j] = indices[arr->ndim-1-j];
        int dest_offset = compute_offset(new_arr, dest_indices);
        if (dest_offset < 0) { free(indices); free_array(new_arr); return NULL; }
        memcpy((char*)new_arr->data + dest_offset*element_size, (char*)arr->data + src_offset*element_size, element_size);
        int dim = 0;
        while (dim < arr->ndim) { indices[dim]++; if (indices[dim] < arr->shape[dim]) break; indices[dim] = 0; dim++; }
    }
    free(indices);
    return new_arr;
}
