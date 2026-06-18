#include <complex.h>
#include <stdint.h>
#include "utils.h"
double get_real_val(void *ptr, DataType dtype) {
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
        default: return 1.0;
    }
}

// getcomplexnumbervalue
complex double get_complex_val(void *ptr, DataType dtype) {
    if (dtype == COMPLEX64) {
        return (complex double)*(complex float*)ptr;
    } else {
        return *(complex double*)ptr;
    }
}