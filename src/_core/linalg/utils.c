#include "array.h"


// Linear algebra helper functions
void swap_rows(Array *arr, int row1, int row2) {
    if (arr == NULL || arr->ndim < 2) {
        return;
    }

    size_t element_size = dtype_size(arr->dtype);
    int cols = arr->shape[1];
    char *row1_ptr = (char*)arr->data + row1 * cols * element_size;
    char *row2_ptr = (char*)arr->data + row2 * cols * element_size;

    // swap two rows of data
    char *temp = (char*)malloc(cols * element_size);
    if (temp == NULL) {
        return;
    }

    memcpy(temp, row1_ptr, cols * element_size);
    memcpy(row1_ptr, row2_ptr, cols * element_size);
    memcpy(row2_ptr, temp, cols * element_size);

    free(temp);
}

void swap_cols(Array *arr, int col1, int col2) {
    if (arr == NULL || arr->ndim < 2) {
        return;
    }

    size_t element_size = dtype_size(arr->dtype);
    int rows = arr->shape[0];
    int cols = arr->shape[1];

    // swap two columns of data
    char *temp = (char*)malloc(element_size);
    if (temp == NULL) {
        return;
    }

    for (int i = 0; i < rows; i++) {
        char *col1_ptr = (char*)arr->data + (i * cols + col1) * element_size;
        char *col2_ptr = (char*)arr->data + (i * cols + col2) * element_size;

        memcpy(temp, col1_ptr, element_size);
        memcpy(col1_ptr, col2_ptr, element_size);
        memcpy(col2_ptr, temp, element_size);
    }

    free(temp);
}