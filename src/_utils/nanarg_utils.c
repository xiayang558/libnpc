#include <math.h>
#include <complex.h>
#include <float.h>
#include "array.h"


// helper: check if NaN (floating point)
int is_nan_double(double x) { return isnan(x); }
int is_nan_float(float x) { return isnan(x); }
int is_nan_long_double(long double x) { return isnan(x); }

// complex: if either real or imag part is NaN, treat as NaN
int is_nan_complex128(complex double z) {
    return isnan(creal(z)) || isnan(cimag(z));
}
int is_nan_complex64(complex float z) {
    return isnan(crealf(z)) || isnan(cimagf(z));
}

// helper: check real NaN
int is_nan_real(void *ptr, DataType dtype) {
    switch (dtype) {
        case FLOAT32: return isnan(*(float*)ptr);
        case FLOAT64: return isnan(*(double*)ptr);
        default: return 0;
    }
}

// helper: check complex NaN (if either real or imag part is NaN)
int is_nan_complex(void *ptr, DataType dtype) {
    if (dtype == COMPLEX64) {
        complex float z = *(complex float*)ptr;
        return isnan(crealf(z)) || isnan(cimagf(z));
    } else if (dtype == COMPLEX128) {
        complex double z = *(complex double*)ptr;
        return isnan(creal(z)) || isnan(cimag(z));
    }
    return 0;
}

// get value (real) or magnitude (complex)
double get_value(void *ptr, DataType dtype) {
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
        case COMPLEX64: return cabsf(*(complex float*)ptr);
        case COMPLEX128: return cabs(*(complex double*)ptr);
        default: return 0.0;
    }
}

// check if NaN
int is_nan(void *ptr, DataType dtype) {
    switch (dtype) {
        case FLOAT32: return isnan(*(float*)ptr);
        case FLOAT64: return isnan(*(double*)ptr);
        case FLOAT128: return isnan(*(long double*)ptr);
        case COMPLEX64: return is_nan_complex64(*(complex float*)ptr);
        case COMPLEX128: return is_nan_complex128(*(complex double*)ptr);
        default: return 0; // integers cannot be NaN
    }
}