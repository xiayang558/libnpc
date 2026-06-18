#include <stdlib.h>
#include <string.h>
#include "array.h"
Array* concatenate(Array *arr1, Array *arr2, int axis) {
    if (arr1 == NULL || arr2 == NULL) {
        NP_UTILS_ERROR_LOG("Error", "NULL array argument");
        return NULL;
    }

    if (arr1->dtype != arr2->dtype) {
        NP_UTILS_ERROR_LOG("Error", "Array data types do not match (%s vs %s)",
                          dtype_name(arr1->dtype), dtype_name(arr2->dtype));
        return NULL;
    }

    if (arr1->ndim != arr2->ndim) {
        NP_UTILS_ERROR_LOG("Error", "Array dimensions do not match (%d vs %d)",
                          arr1->ndim, arr2->ndim);
        return NULL;
    }

    if (axis < 0 || axis >= arr1->ndim) {
        NP_UTILS_ERROR_LOG("Error", "Invalid axis %d for arrays with %d dimensions", 
                          axis, arr1->ndim);
        return NULL;
    }

    // check that all dimensions except the concatenation axis match
    for (int i = 0; i < arr1->ndim; i++) {
        if (i != axis && arr1->shape[i] != arr2->shape[i]) {
            NP_UTILS_ERROR_LOG("Error", "Array shapes do not match for concatenation at axis %d", i);
            return NULL;
        }
    }

    // compute result array shape
    int *result_shape = (int*)malloc(arr1->ndim * sizeof(int));
    if (result_shape == NULL) {
        NP_UTILS_ERROR_LOG("Error", "Memory allocation failed");
        return NULL;
    }

    memcpy(result_shape, arr1->shape, arr1->ndim * sizeof(int));
    result_shape[axis] = arr1->shape[axis] + arr2->shape[axis];

    // create result array
    Array *result = create_array(result_shape, arr1->ndim, arr1->dtype);
    free(result_shape);
    
    if (result == NULL) {
        NP_UTILS_ERROR_LOG("Error", "Failed to create result array");
        return NULL;
    }

    size_t element_size = dtype_size(arr1->dtype);

    // for 1D arrays, direct concatenation
    if (arr1->ndim == 1) {
        memcpy(result->data, arr1->data, arr1->size * element_size);
        memcpy((char*)result->data + arr1->size * element_size,
               arr2->data, arr2->size * element_size);
        return result;
    }

    // for higher-dimensional arrays, copy block by block
    // compute block count (product of dimensions before the concatenation axis)
    int block_size = 1;
    for (int i = 0; i < axis; i++) {
        block_size *= arr1->shape[i];
    }
    
    // elements per block (concatenation axis and subsequent dimensions)
    int elements_per_block_arr1 = arr1->size / block_size;
    int elements_per_block_arr2 = arr2->size / block_size;
    
    char *dest = (char*)result->data;
    char *src1 = (char*)arr1->data;
    char *src2 = (char*)arr2->data;
    
    for (int block = 0; block < block_size; block++) {
        // copy first array block
        memcpy(dest, src1, elements_per_block_arr1 * element_size);
        dest += elements_per_block_arr1 * element_size;
        src1 += elements_per_block_arr1 * element_size;
        
        // copy second array block
        memcpy(dest, src2, elements_per_block_arr2 * element_size);
        dest += elements_per_block_arr2 * element_size;
        src2 += elements_per_block_arr2 * element_size;
    }
    
    return result;
}

// create a 0-dimensional array containing a single scalar
