#include "def.h"
#include "array.h"
#include "shape.h"
Array* diag(Array *arr, int k) {
    if (arr == NULL) { fprintf(stderr, "diag: NULL array argument\n"); return NULL; }
    if (arr->ndim < 1 || arr->ndim > 2) { fprintf(stderr, "diag: array must be 1-D or 2-D\n"); return NULL; }
    if (arr->ndim == 1) {
        int n = arr->size + abs(k);
        Array *result = create_array((int[]){n, n}, 2, arr->dtype);
        if (result == NULL) return NULL;
        size_t elem_sz = dtype_size(arr->dtype);
        memset(result->data, 0, n * n * elem_sz);
        char *src = (char*)arr->data, *dst = (char*)result->data;
        int start_row = (k >= 0) ? 0 : -k;
        int start_col = (k >= 0) ? k : 0;
        for (int i = 0; i < arr->size; i++) {
            int r = start_row + i, c = start_col + i;
            if (r >= 0 && r < n && c >= 0 && c < n) memcpy(dst + (r*n + c)*elem_sz, src + i*elem_sz, elem_sz);
        }
        return result;
    }
    int rows = arr->shape[0], cols = arr->shape[1];
    int len;
    if (k >= 0) len = cols - k; else len = rows + k;
    if (len > rows) len = rows; if (len > cols) len = cols;
    if (len < 0) len = 0;
    Array *result = create_array((int[]){len}, 1, arr->dtype);
    if (result == NULL) return NULL;
    size_t elem_sz = dtype_size(arr->dtype);
    char *src = (char*)arr->data, *dst = (char*)result->data;
    for (int i = 0; i < len; i++) memcpy(dst + i*elem_sz, src + (i*cols + i + k)*elem_sz, elem_sz);
    return result;
}
