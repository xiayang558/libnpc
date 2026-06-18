#include "def.h"
#include "array.h"
#include "shape.h"

/**
 * ravel — flatten array to 1D (like numpy.ravel)
 *
 * Difference from flatten:
 *   ravel   — returns a view (shares data when contiguous)
 *   flatten — always returns a copy
 *
 * @param arr   Input array
 * @param order 'C' = row-major, 'F' = column-major, 'A' = C if contiguous else F
 *              'K' = as stored in memory (currently same as 'A')
 * @return      1D array (view or copy)
 */
Array* ravel(Array *arr, const char *order) {
    if (arr == NULL) {
        fprintf(stderr, "ravel: NULL array argument\n");
        return NULL;
    }

    char ord = (order && order[0]) ? order[0] : 'C';

    if (ord == 'C' || ord == 'A' || ord == 'K') {
        /* View: share data, just change shape to 1D */
        int shape[1] = {arr->size};
        Array *view = create_array(shape, 1, arr->dtype);
        if (view == NULL) {
            fprintf(stderr, "ravel: Memory allocation failed\n");
            return NULL;
        }
        /* Overwrite data pointer to share the original */
        free(view->data);
        view->data = arr->data;
        view->is_view = 1;
        return view;
    }

    /* 'F' order: need a transposed copy → flatten */
    {
        int shape[1] = {arr->size};
        Array *flat = create_array(shape, 1, arr->dtype);
        if (flat == NULL) {
            fprintf(stderr, "ravel: Memory allocation failed\n");
            return NULL;
        }

        size_t elem_size = dtype_size(arr->dtype);
        if (arr->ndim == 1) {
            memcpy(flat->data, arr->data, arr->size * elem_size);
            return flat;
        }

        /* Column-major traversal: for each column (last dim), iterate rows */
        char *dst = (char*)flat->data;
        char *src = (char*)arr->data;

        /* strides for the last dimension = elem_size, for others accumulate */
        size_t *byte_strides = (size_t*)malloc(arr->ndim * sizeof(size_t));
        compute_byte_strides(arr->shape, arr->ndim, elem_size, byte_strides);

        if (arr->ndim == 2) {
            int rows = arr->shape[0], cols = arr->shape[1];
            size_t row_sz = byte_strides[0];
            size_t col_sz = byte_strides[1];
            for (int c = 0; c < cols; c++) {
                for (int r = 0; r < rows; r++) {
                    memcpy(dst, src + r * row_sz + c * col_sz, elem_size);
                    dst += elem_size;
                }
            }
        } else {
            /* General ndim: recursive column-major index enumeration */
            int *idx = (int*)calloc(arr->ndim, sizeof(int));
            for (int pos = 0; pos < arr->size; pos++) {
                /* Compute byte offset from current index */
                size_t off = 0;
                for (int d = 0; d < arr->ndim; d++)
                    off += idx[d] * byte_strides[d];
                memcpy(dst, src + off, elem_size);
                dst += elem_size;

                /* Increment in column-major (F) order: first axis varies fastest */
                for (int d = 0; d < arr->ndim; d++) {
                    idx[d]++;
                    if (idx[d] < arr->shape[d]) break;
                    idx[d] = 0;
                }
            }
            free(idx);
        }
        free(byte_strides);
        return flat;
    }
}
