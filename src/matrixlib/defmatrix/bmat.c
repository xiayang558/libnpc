#include "def.h"
#include "array.h"


Array* bmat(Array **blocks, int rows, int cols) {
    if (blocks == NULL || rows <= 0 || cols <= 0) {
        fprintf(stderr, "bmat: Invalid arguments\n");
        return NULL;
    }

    int num_blocks = rows * cols;

    // check that blocks are not null and have the same data type
    DataType dtype = blocks[0]->dtype;
    int ndim = blocks[0]->ndim;
    if (ndim != 2) {
        fprintf(stderr, "bmat: All blocks must be 2D matrices\n");
        return NULL;
    }

    for (int i = 1; i < num_blocks; i++) {
        if (blocks[i] == NULL) {
            fprintf(stderr, "bmat: NULL block at index %d\n", i);
            return NULL;
        }
        if (blocks[i]->dtype != dtype) {
            fprintf(stderr, "bmat: Data type mismatch\n");
            return NULL;
        }
        if (blocks[i]->ndim != ndim) {
            fprintf(stderr, "bmat: All blocks must be 2D\n");
            return NULL;
        }
    }

    // check that within each row, block heights (rows) are consistent
    for (int r = 0; r < rows; r++) {
        int base_h = blocks[r * cols]->shape[0];
        for (int c = 1; c < cols; c++) {
            if (blocks[r * cols + c]->shape[0] != base_h) {
                fprintf(stderr, "bmat: Row %d: blocks have different heights\n", r);
                return NULL;
            }
        }
    }

    // compute total width per row, and check that all rows have the same total width
    int *row_widths = malloc(rows * sizeof(int));
    if (row_widths == NULL) {
        fprintf(stderr, "bmat: Memory allocation failed\n");
        return NULL;
    }

    for (int r = 0; r < rows; r++) {
        int total_width = 0;
        for (int c = 0; c < cols; c++) {
            total_width += blocks[r * cols + c]->shape[1];
        }
        row_widths[r] = total_width;
    }

    int common_width = row_widths[0];
    for (int r = 1; r < rows; r++) {
        if (row_widths[r] != common_width) {
            fprintf(stderr, "bmat: Rows have different total widths\n");
            free(row_widths);
            return NULL;
        }
    }
    free(row_widths);

    // first concatenate each row horizontally
    Array **row_arrays = malloc(rows * sizeof(Array*));
    if (row_arrays == NULL) {
        fprintf(stderr, "bmat: Memory allocation failed\n");
        return NULL;
    }

    for (int r = 0; r < rows; r++) {
        Array **row_blocks = &blocks[r * cols];
        // concatenate along axis=1
        Array *row_concat = concatenate(row_blocks[0], row_blocks[1], 1);
        for (int c = 2; c < cols; c++) {
            Array *tmp = concatenate(row_concat, row_blocks[c], 1);
            free_array(row_concat);
            row_concat = tmp;
        }
        row_arrays[r] = row_concat;
    }

    // column-concatenate all rows
    Array *result = concatenate(row_arrays[0], row_arrays[1], 0);
    for (int r = 2; r < rows; r++) {
        Array *tmp = concatenate(result, row_arrays[r], 0);
        free_array(result);
        result = tmp;
    }

    // free row arrays
    for (int r = 0; r < rows; r++) {
        free_array(row_arrays[r]);
    }
    free(row_arrays);

    return result;
}