#include "def.h"
#include "array.h"
#include "shape.h"

Array* block(Array **blocks, int rows, int cols) {
    if (blocks == NULL || rows <= 0 || cols <= 0) {
        fprintf(stderr, "block: Invalid arguments\n");
        return NULL;
    }
    int num_blocks = rows * cols;
    DataType dtype = blocks[0]->dtype;
    int ndim = blocks[0]->ndim;
    if (ndim < 2) { fprintf(stderr, "block: All blocks must have at least 2 dimensions\n"); return NULL; }
    for (int i = 1; i < num_blocks; i++) {
        if (blocks[i] == NULL) { fprintf(stderr, "block: NULL block at index %d\n", i); return NULL; }
        if (blocks[i]->dtype != dtype) { fprintf(stderr, "block: Data type mismatch\n"); return NULL; }
        if (blocks[i]->ndim != ndim) { fprintf(stderr, "block: Dimension count mismatch\n"); return NULL; }
    }
    for (int r = 0; r < rows; r++) {
        int base_h = blocks[r * cols]->shape[0];
        for (int c = 1; c < cols; c++) {
            if (blocks[r * cols + c]->shape[0] != base_h) {
                fprintf(stderr, "block: Row %d: blocks have different heights\n", r);
                return NULL;
            }
        }
    }
    int *row_widths = malloc(rows * sizeof(int));
    if (!row_widths) { fprintf(stderr, "block: Memory allocation failed\n"); return NULL; }
    for (int r = 0; r < rows; r++) {
        int total_width = 0;
        for (int c = 0; c < cols; c++) total_width += blocks[r * cols + c]->shape[1];
        row_widths[r] = total_width;
    }
    int common_width = row_widths[0];
    for (int r = 1; r < rows; r++) {
        if (row_widths[r] != common_width) { fprintf(stderr, "block: Rows have different total widths\n"); free(row_widths); return NULL; }
    }
    free(row_widths);
    Array **row_arrays = malloc(rows * sizeof(Array*));
    if (!row_arrays) { fprintf(stderr, "block: Memory allocation failed\n"); return NULL; }
    for (int r = 0; r < rows; r++) {
        Array *row_concat = concatenate(blocks[r*cols], blocks[r*cols+1], 1);
        for (int c = 2; c < cols; c++) { Array *tmp = concatenate(row_concat, blocks[r*cols+c], 1); free_array(row_concat); row_concat = tmp; }
        row_arrays[r] = row_concat;
    }
    Array *result = concatenate(row_arrays[0], row_arrays[1], 0);
    for (int r = 2; r < rows; r++) { Array *tmp = concatenate(result, row_arrays[r], 0); free_array(result); result = tmp; }
    for (int r = 0; r < rows; r++) free_array(row_arrays[r]);
    free(row_arrays);
    return result;
}
