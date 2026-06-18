#include "def.h"
#include "array.h"

Array* count_nonzero(Array *arr, int axis) {
    if (arr == NULL) { fprintf(stderr, "count_nonzero: NULL array argument\n"); return NULL; }
    if (axis < -1 || axis >= arr->ndim) { fprintf(stderr, "count_nonzero: Invalid axis %d for array with %d dimensions\n", axis, arr->ndim); return NULL; }
    if (axis == -1) {
        size_t elem_size = dtype_size(arr->dtype);
        char *data = (char*)arr->data;
        int64_t count = 0;
        for (int i = 0; i < arr->size; i++) { if (is_nonzero(data + i * elem_size, arr->dtype)) count++; }
        Array *result = create_scalar_array(&count, INT64);
        if (result == NULL) return NULL;
        return result;
    }
    int out_ndim = arr->ndim - 1;
    int *out_shape = malloc(out_ndim * sizeof(int));
    if (out_shape == NULL) { fprintf(stderr, "count_nonzero: memory allocation failed\n"); return NULL; }
    int out_idx = 0;
    for (int i = 0; i < arr->ndim; i++) { if (i != axis) out_shape[out_idx++] = arr->shape[i]; }
    Array *result = create_array(out_shape, out_ndim, INT64);
    free(out_shape);
    if (result == NULL) { fprintf(stderr, "count_nonzero: failed to create result array\n"); return NULL; }
    int *arr_strides = malloc(arr->ndim * sizeof(int)), *res_strides = malloc(result->ndim * sizeof(int));
    if (arr_strides == NULL || res_strides == NULL) { free(arr_strides); free(res_strides); free_array(result); return NULL; }
    compute_strides(arr->shape, arr->ndim, arr_strides);
    compute_strides(result->shape, result->ndim, res_strides);
    size_t elem_size = dtype_size(arr->dtype);
    char *arr_data = (char*)arr->data;
    int64_t *res_data = (int64_t*)result->data;
    int *indices = calloc(result->ndim, sizeof(int));
    if (indices == NULL) { free(arr_strides); free(res_strides); free_array(result); return NULL; }
    int total_out = result->size;
    for (int flat = 0; flat < total_out; flat++) {
        int temp = flat;
        for (int i = result->ndim - 1; i >= 0; i--) { indices[i] = temp % result->shape[i]; temp /= result->shape[i]; }
        int base_offset = 0;
        int out_dim_idx = 0;
        for (int i = 0; i < arr->ndim; i++) { if (i == axis) continue; base_offset += indices[out_dim_idx++] * arr_strides[i]; }
        int64_t cnt = 0;
        for (int k = 0; k < arr->shape[axis]; k++) { int offset = base_offset + k * arr_strides[axis]; if (is_nonzero(arr_data + offset * elem_size, arr->dtype)) cnt++; }
        int res_flat = 0;
        for (int i = 0; i < result->ndim; i++) res_flat += indices[i] * res_strides[i];
        res_data[res_flat] = cnt;
    }
    free(arr_strides); free(res_strides); free(indices);
    return result;
}
