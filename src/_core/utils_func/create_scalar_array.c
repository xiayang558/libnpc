#include <stdlib.h>
#include <string.h>
#include "array.h"
Array* create_scalar_array(void *value, DataType dtype) {
    if (value == NULL) {
        utils_error_log("Error", "create_scalar_array", "NULL value");
        return NULL;
    }

    Array *res = (Array*)malloc(sizeof(Array));
    
    if (res == NULL) {
        utils_error_log("Error", "create_scalar_array", "Memory allocation failed");
        return NULL;
    }

    // allocatenumberbased onmemory
    res->data = malloc(dtype_size(dtype));
    if (res->data == NULL) {
        free(res);
        utils_error_log("Error", "create_scalar_array", "Memory allocation failed");
        return NULL;
    }
    memcpy(res->data, value, dtype_size(dtype));
    res->dtype = dtype;
    res->shape = NULL;
    res->ndim = 0;
    res->size = 1;
    res->strides = NULL;
    res->is_view = 0;

    return res;
}

// compute contiguous layout strides
