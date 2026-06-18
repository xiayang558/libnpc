#include <math.h>
#include <complex.h>
#include <stdint.h>
#include "array.h"
int is_nonzero(void *ptr, DataType dtype) {
    switch (dtype) {
        case BOOL:
        case INT8:    return *(int8_t*)ptr != 0;
        case INT16:   return *(int16_t*)ptr != 0;
        case INT32:   return *(int32_t*)ptr != 0;
        case INT64:   return *(int64_t*)ptr != 0;
        case UINT8:   return *(uint8_t*)ptr != 0;
        case UINT16:  return *(uint16_t*)ptr != 0;
        case UINT32:  return *(uint32_t*)ptr != 0;
        case UINT64:  return *(uint64_t*)ptr != 0;
        case FLOAT32: return fabsf(*(float*)ptr) > 0.0f;
        case FLOAT64: return fabs(*(double*)ptr) > 0.0;
        case FLOAT128: return fabsl(*(long double*)ptr) > 0.0L;
        case COMPLEX64: {
            complex float *z = (complex float*)ptr;
            return crealf(*z) != 0.0f || cimagf(*z) != 0.0f;
        }
        case COMPLEX128: {
            complex double *z = (complex double*)ptr;
            return creal(*z) != 0.0 || cimag(*z) != 0.0;
        }
        case COMPLEX256: {
            complex long double *z = (complex long double*)ptr;
            return creall(*z) != 0.0L || cimagl(*z) != 0.0L;
        }
        default:
            return 0; // unknown type treated as 0
    }
}


// check if integer type
