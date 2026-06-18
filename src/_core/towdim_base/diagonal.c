#include "def.h"
#include "array.h"
#include "shape.h"
Array* diagonal(Array *arr, int offset, int axis1, int axis2) {
    if (arr == NULL) { fprintf(stderr, "diagonal: NULL array argument\n"); return NULL; }
    if (arr->ndim < 2) { fprintf(stderr, "diagonal: array must have at least 2 dimensions\n"); return NULL; }
    int ndim = arr->ndim;
    if (axis1 < 0) axis1 += ndim; if (axis2 < 0) axis2 += ndim;
    if (axis1 < 0 || axis1 >= ndim || axis2 < 0 || axis2 >= ndim) { fprintf(stderr, "diagonal: invalid axis\n"); return NULL; }
    if (axis1 == axis2) { fprintf(stderr, "diagonal: axis1 and axis2 must differ\n"); return NULL; }
    int d1 = arr->shape[axis1], d2 = arr->shape[axis2];
    int diag_len;
    if (offset >= 0) diag_len = d2 - offset; else diag_len = d1 + offset;
    if (diag_len > d1) diag_len = d1; if (diag_len > d2) diag_len = d2;
    if (diag_len < 0) diag_len = 0;
    int out_ndim = ndim - 1;
    int *out_shape = malloc(out_ndim * sizeof(int));
    if (!out_shape) return NULL;
    int idx = 0;
    for (int i = 0; i < ndim; i++) { if (i != axis1 && i != axis2) out_shape[idx++] = arr->shape[i]; }
    out_shape[idx] = diag_len;
    Array *result = create_array(out_shape, out_ndim, arr->dtype);
    free(out_shape);
    if (!result) return NULL;
    size_t elem_sz = dtype_size(arr->dtype);
    char *src = (char*)arr->data, *dst = (char*)result->data;
    int *src_strides = malloc(ndim * sizeof(int));
    int *dst_strides = malloc(out_ndim * sizeof(int));
    compute_strides(arr->shape, ndim, src_strides);
    compute_strides(result->shape, out_ndim, dst_strides);
    int *indices = calloc(out_ndim, sizeof(int));
    if (!indices) { free(src_strides); free(dst_strides); free_array(result); return NULL; }
    for (int flat = 0; flat < result->size; flat++) {
        int tmp = flat;
        for (int i = out_ndim-1; i >= 0; i--) { indices[i] = tmp % result->shape[i]; tmp /= result->shape[i]; }
        int src_off = 0, dst_off = 0;
        int out_dim = 0;
        for (int i = 0; i < ndim; i++) {
            if (i == axis1 || i == axis2) continue;
            src_off += indices[out_dim] * src_strides[i];
            dst_off += indices[out_dim] * dst_strides[out_dim];
            out_dim++;
        }
        int diag_i = indices[out_dim];
        int coord1 = diag_i + (offset >= 0 ? 0 : -offset);
        int coord2 = diag_i + (offset >= 0 ? offset : 0);
        src_off += coord1 * src_strides[axis1] + coord2 * src_strides[axis2];
        dst_off += diag_i * dst_strides[out_dim];
        memcpy(dst + dst_off*elem_sz, src + src_off*elem_sz, elem_sz);
    }
    free(src_strides); free(dst_strides); free(indices);
    return result;
}
