#include <stdint.h>
#include <complex.h>
#include "array.h"
double to_double(void *ptr, DataType dtype) {
    switch (dtype) {
        case INT8:   return *(int8_t*)ptr;
        case INT16:  return *(int16_t*)ptr;
        case INT32:  return *(int32_t*)ptr;
        case INT64:  return *(int64_t*)ptr;
        case UINT8:  return *(uint8_t*)ptr;
        case UINT16: return *(uint16_t*)ptr;
        case UINT32: return *(uint32_t*)ptr;
        case UINT64: return *(uint64_t*)ptr;
        case FLOAT32: return *(float*)ptr;
        case FLOAT64: return *(double*)ptr;
        default: return 0.0;
    }
}

// helper: convert any numeric value to complex double
complex double to_complex(void *ptr, DataType dtype) {
    switch (dtype) {
        case BOOL:   return (complex double)*(uint8_t*)ptr;
        case INT8:   return (complex double)*(int8_t*)ptr;
        case INT16:  return (complex double)*(int16_t*)ptr;
        case INT32:  return (complex double)*(int32_t*)ptr;
        case INT64:  return (complex double)*(int64_t*)ptr;
        case UINT8:  return (complex double)*(uint8_t*)ptr;
        case UINT16: return (complex double)*(uint16_t*)ptr;
        case UINT32: return (complex double)*(uint32_t*)ptr;
        case UINT64: return (complex double)*(uint64_t*)ptr;
        case FLOAT32: return (complex double)*(float*)ptr;
        case FLOAT64: return (complex double)*(double*)ptr;
        case COMPLEX64: return (complex double)*(complex float*)ptr;
        case COMPLEX128: return *(complex double*)ptr;
        default: return 0.0;
    }
}

/* ----------------------- cmp function family ----------------------- */

