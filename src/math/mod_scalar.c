#include <math.h>
#include "array.h"

/**
 * mod_scalar — element-wise remainder with scalar divisor (like numpy.mod with scalar)
 *
 * result = arr % scalar, where % is modulo (result sign matches divisor sign).
 *
 * @param arr    Input array (real only, complex not supported)
 * @param scalar Pointer to scalar value (same type as arr->dtype)
 * @return FLOAT64 array, same shape as arr
 */
Array* mod_scalar(Array *arr, void *scalar) {
    if (arr == NULL || scalar == NULL) {
        fprintf(stderr, "mod_scalar: NULL argument\n");
        return NULL;
    }
    if (arr->dtype == COMPLEX64 || arr->dtype == COMPLEX128 ||
        arr->dtype == COMPLEX256) {
        fprintf(stderr, "mod_scalar: complex numbers not supported\n");
        return NULL;
    }

    Array *result = create_array(arr->shape, arr->ndim, FLOAT64);
    if (result == NULL) return NULL;

    double divisor;
    switch (arr->dtype) {
        case BOOL:    divisor = *(uint8_t*)scalar; break;
        case INT8:    divisor = *(int8_t*)scalar; break;
        case INT16:   divisor = *(int16_t*)scalar; break;
        case INT32:   divisor = *(int32_t*)scalar; break;
        case INT64:   divisor = *(int64_t*)scalar; break;
        case UINT8:   divisor = *(uint8_t*)scalar; break;
        case UINT16:  divisor = *(uint16_t*)scalar; break;
        case UINT32:  divisor = *(uint32_t*)scalar; break;
        case UINT64:  divisor = *(uint64_t*)scalar; break;
        case FLOAT32: divisor = *(float*)scalar; break;
        case FLOAT64: divisor = *(double*)scalar; break;
        default:
            fprintf(stderr, "mod_scalar: unsupported dtype %s\n", dtype_name(arr->dtype));
            free_array(result);
            return NULL;
    }

    if (divisor == 0.0) {
        /* divide-by-zero: fill with 0 (matching mod behavior) */
        double *rd = (double*)result->data;
        for (int i = 0; i < arr->size; i++) rd[i] = 0.0;
        return result;
    }

    size_t sz = dtype_size(arr->dtype);
    char *in = (char*)arr->data;
    double *rd = (double*)result->data;

    for (int i = 0; i < arr->size; i++) {
        void *pi = in + i * sz;
        double val;
        switch (arr->dtype) {
            case BOOL:   val = *(uint8_t*)pi; break;
            case INT8:   val = *(int8_t*)pi; break;
            case INT16:  val = *(int16_t*)pi; break;
            case INT32:  val = *(int32_t*)pi; break;
            case INT64:  val = *(int64_t*)pi; break;
            case UINT8:  val = *(uint8_t*)pi; break;
            case UINT16: val = *(uint16_t*)pi; break;
            case UINT32: val = *(uint32_t*)pi; break;
            case UINT64: val = *(uint64_t*)pi; break;
            case FLOAT32: val = *(float*)pi; break;
            case FLOAT64: val = *(double*)pi; break;
            default: val = 0.0; break;
        }

        /* floating-point modulo with sign match (same as numpy.mod) */
        double r = fmod(val, divisor);
        if (r != 0.0 && ((r > 0.0) != (divisor > 0.0))) r += divisor;
        rd[i] = r;
    }

    return result;
}
