#include <stdlib.h>
#include <string.h>
#include <complex.h>
#include "array.h"

/**
 * real  --  extractcomplexrealpart (like numpy.real)
 */
Array* real_array(Array *arr) {
    if (arr == NULL) { fprintf(stderr, "real: NULL argument\n"); return NULL; }

    DataType out_dtype;
    switch (arr->dtype) {
        case COMPLEX64:  out_dtype = FLOAT32; break;
        case COMPLEX128: out_dtype = FLOAT64; break;
        case COMPLEX256: out_dtype = FLOAT128; break;
        default:         return copy(arr); /* Non-complex: return copy */
    }

    Array *result = create_array(arr->shape, arr->ndim, out_dtype);
    if (!result) return NULL;

    size_t esz = dtype_size(arr->dtype);
    size_t osz = dtype_size(out_dtype);
    char *src = (char*)arr->data;
    char *dst = (char*)result->data;

    for (int i = 0; i < arr->size; i++) {
        switch (arr->dtype) {
            case COMPLEX64: {
                float r = crealf(*(complex float*)(src + i * esz));
                memcpy(dst + i * osz, &r, osz); break;
            }
            case COMPLEX128: {
                double r = creal(*(complex double*)(src + i * esz));
                memcpy(dst + i * osz, &r, osz); break;
            }
            case COMPLEX256: {
                long double r = creall(*(complex long double*)(src + i * esz));
                memcpy(dst + i * osz, &r, osz); break;
            }
            default: break;
        }
    }
    return result;
}

/**
 * imag  --  extractcompleximagpart (like numpy.imag)
 */
Array* imag_array(Array *arr) {
    if (arr == NULL) { fprintf(stderr, "imag: NULL argument\n"); return NULL; }

    DataType out_dtype;
    switch (arr->dtype) {
        case COMPLEX64:  out_dtype = FLOAT32; break;
        case COMPLEX128: out_dtype = FLOAT64; break;
        case COMPLEX256: out_dtype = FLOAT128; break;
        default:  /* Non-complex: return zeros */
            out_dtype = FLOAT64;
            break;
    }

    Array *result = create_array(arr->shape, arr->ndim, out_dtype);
    if (!result) return NULL;

    size_t esz = dtype_size(arr->dtype);
    size_t osz = dtype_size(out_dtype);
    char *src = (char*)arr->data;
    char *dst = (char*)result->data;

    if (arr->dtype != COMPLEX64 && arr->dtype != COMPLEX128 && arr->dtype != COMPLEX256) {
        /* Non-complex: fill with zeros */
        double zero = 0.0;
        for (int i = 0; i < arr->size; i++)
            memcpy(dst + i * osz, &zero, osz);
        return result;
    }

    for (int i = 0; i < arr->size; i++) {
        switch (arr->dtype) {
            case COMPLEX64: {
                float im = cimagf(*(complex float*)(src + i * esz));
                memcpy(dst + i * osz, &im, osz); break;
            }
            case COMPLEX128: {
                double im = cimag(*(complex double*)(src + i * esz));
                memcpy(dst + i * osz, &im, osz); break;
            }
            case COMPLEX256: {
                long double im = cimagl(*(complex long double*)(src + i * esz));
                memcpy(dst + i * osz, &im, osz); break;
            }
            default: break;
        }
    }
    return result;
}
