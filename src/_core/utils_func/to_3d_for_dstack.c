#include <stdio.h>
#include "shape.h"
Array* to_3d_for_dstack(Array *arr) {
    if (arr->ndim == 1) {
        // (N,) -> (1,1,N)
        int new_shape[3] = {1, 1, arr->size};
        return reshape(arr, new_shape, 3);
    } else if (arr->ndim == 2) {
        // (M,N) -> (1,M,N)
        int new_shape[3] = {1, arr->shape[0], arr->shape[1]};
        return reshape(arr, new_shape, 3);
    } else if (arr->ndim == 3) {
        return copy_array(arr);
    } else {
        fprintf(stderr, "dstack: arrays with ndim > 3 are not supported (but NumPy supports)\n");
        return copy_array(arr);
    }
}

// Convert any integer type to int64_t
int64_t to_int64(void *ptr, DataType dtype) {
    switch (dtype) {
        case BOOL:   return *(uint8_t*)ptr;
        case INT8:   return *(int8_t*)ptr;
        case INT16:  return *(int16_t*)ptr;
        case INT32:  return *(int32_t*)ptr;
        case INT64:  return *(int64_t*)ptr;
        case UINT8:  return *(uint8_t*)ptr;
        case UINT16: return *(uint16_t*)ptr;
        case UINT32: return *(uint32_t*)ptr;
        case UINT64: return *(uint64_t*)ptr;
        default:     return 0;
    }
}

// Convert any integer type to uint64_t (for shift operations)
uint64_t to_uint64(void *ptr, DataType dtype) {
    switch (dtype) {
        case BOOL:   return *(uint8_t*)ptr;
        case INT8:   return (uint64_t)*(int8_t*)ptr;
        case INT16:  return (uint64_t)*(int16_t*)ptr;
        case INT32:  return (uint64_t)*(int32_t*)ptr;
        case INT64:  return (uint64_t)*(int64_t*)ptr;
        case UINT8:  return *(uint8_t*)ptr;
        case UINT16: return *(uint16_t*)ptr;
        case UINT32: return *(uint32_t*)ptr;
        case UINT64: return *(uint64_t*)ptr;
        default:     return 0;
    }
    
}
// Convert any numeric value to double (for exponential)
