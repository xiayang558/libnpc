#include "array.h"


Array* around(Array *arr, int decimals) {
    if (arr == NULL) {
        fprintf(stderr, "[Error] around: NULL array argument\n");
        return NULL;
    }

    // Determine output data type based on input type
    DataType out_dtype = arr->dtype;
    int is_integer = 0;
    switch (arr->dtype) {
        case INT8: case INT16: case INT32: case INT64:
        case UINT8: case UINT16: case UINT32: case UINT64:
            is_integer = 1;
            if (decimals != 0) out_dtype = FLOAT64; // Promote to double precision
            break;
        case FLOAT32:
        case FLOAT64:
        case FLOAT128:
        case COMPLEX64:
        case COMPLEX128:
        case COMPLEX256:
            // Keep original type
            break;
        default:
            fprintf(stderr, "[Error] around: Unsupported data type %s\n", dtype_name(arr->dtype));
            return NULL;
    }

    // Determine output shape (broadcast)
    int out_ndim;
    int *out_shape = NULL;
    // Simplified: only handles scalar broadcast or same shape (can expand to full broadcast later)
    
    out_ndim = arr->ndim;
    out_shape = malloc(out_ndim * sizeof(int));
    memcpy(out_shape, arr->shape, out_ndim * sizeof(int));

    // Create output array (shape is NULL, ndim=0 for scalar)
    // Add scalar support
    Array *result;
    if (out_ndim == 0) {
        int value = 0; // default, not equal
        result = create_scalar_array(&value, out_dtype);
        if (result == NULL) {
            fprintf(stderr, "[Error] equal: Failed to create result scalar\n");
            return NULL;
        } 
    } else {
        result = create_array(out_shape, out_ndim, out_dtype);
        if (out_shape) free(out_shape);
        if (result == NULL) {
            fprintf(stderr, "[Error] equal: Failed to create result array\n");
            return NULL;
        }
    }
    // Array *result = create_array(a->shape, a->ndim, out_dtype);
    // if (result == NULL) {
    //     fprintf(stderr, "[Error] around: Failed to create result array\n");
    //     return NULL;
    // }

    size_t elem_size_in = dtype_size(arr->dtype);
    size_t elem_size_out = dtype_size(out_dtype);
    char *in_data = (char*)arr->data;
    char *out_data = (char*)result->data;

    double factor = pow(10.0, decimals);  // Rounding factor

    for (int i = 0; i < arr->size; i++) {
        void *in_ptr = in_data + i * elem_size_in;
        void *out_ptr = out_data + i * elem_size_out;

        switch (arr->dtype) {
            case INT8: {
                int8_t val = *(int8_t*)in_ptr;
                if (decimals == 0) {
                    *(int8_t*)out_ptr = val;
                } else {
                    double dval = (double)val;
                    double rounded = rint(dval * factor) / factor;
                    *(double*)out_ptr = rounded;
                }
                break;
            }
            case INT16: {
                int16_t val = *(int16_t*)in_ptr;
                if (decimals == 0) {
                    *(int16_t*)out_ptr = val;
                } else {
                    double dval = (double)val;
                    double rounded = rint(dval * factor) / factor;
                    *(double*)out_ptr = rounded;
                }
                break;
            }
            case INT32: {
                int32_t val = *(int32_t*)in_ptr;
                if (decimals == 0) {
                    *(int32_t*)out_ptr = val;
                } else {
                    double dval = (double)val;
                    double rounded = rint(dval * factor) / factor;
                    *(double*)out_ptr = rounded;
                }
                break;
            }
            case INT64: {
                int64_t val = *(int64_t*)in_ptr;
                if (decimals == 0) {
                    *(int64_t*)out_ptr = val;
                } else {
                    double dval = (double)val;
                    double rounded = rint(dval * factor) / factor;
                    *(double*)out_ptr = rounded;
                }
                break;
            }
            case UINT8: {
                uint8_t val = *(uint8_t*)in_ptr;
                if (decimals == 0) {
                    *(uint8_t*)out_ptr = val;
                } else {
                    double dval = (double)val;
                    double rounded = rint(dval * factor) / factor;
                    *(double*)out_ptr = rounded;
                }
                break;
            }
            case UINT16: {
                uint16_t val = *(uint16_t*)in_ptr;
                if (decimals == 0) {
                    *(uint16_t*)out_ptr = val;
                } else {
                    double dval = (double)val;
                    double rounded = rint(dval * factor) / factor;
                    *(double*)out_ptr = rounded;
                }
                break;
            }
            case UINT32: {
                uint32_t val = *(uint32_t*)in_ptr;
                if (decimals == 0) {
                    *(uint32_t*)out_ptr = val;
                } else {
                    double dval = (double)val;
                    double rounded = rint(dval * factor) / factor;
                    *(double*)out_ptr = rounded;
                }
                break;
            }
            case UINT64: {
                uint64_t val = *(uint64_t*)in_ptr;
                if (decimals == 0) {
                    *(uint64_t*)out_ptr = val;
                } else {
                    double dval = (double)val;
                    double rounded = rint(dval * factor) / factor;
                    *(double*)out_ptr = rounded;
                }
                break;
            }
            case FLOAT32: {
                float val = *(float*)in_ptr;
                float rounded = rintf(val * (float)factor) / (float)factor;
                *(float*)out_ptr = rounded;
                break;
            }
            case FLOAT64: {
                double val = *(double*)in_ptr;
                double rounded = rint(val * factor) / factor;
                *(double*)out_ptr = rounded;
                break;
            }
            case FLOAT128: {
                long double val = *(long double*)in_ptr;
                long double factor_ld = (long double)factor;
                long double rounded = rintl(val * factor_ld) / factor_ld;
                *(long double*)out_ptr = rounded;
                break;
            }
            case COMPLEX64: {
                complex float val = *(complex float*)in_ptr;
                float real = crealf(val);
                float imag = cimagf(val);
                float real_rounded = rintf(real * (float)factor) / (float)factor;
                float imag_rounded = rintf(imag * (float)factor) / (float)factor;
                *(complex float*)out_ptr = real_rounded + imag_rounded * I;
                break;
            }
            case COMPLEX128: {
                complex double val = *(complex double*)in_ptr;
                double real = creal(val);
                double imag = cimag(val);
                double real_rounded = rint(real * factor) / factor;
                double imag_rounded = rint(imag * factor) / factor;
                *(complex double*)out_ptr = real_rounded + imag_rounded * I;
                break;
            }
            case COMPLEX256: {
                complex long double val = *(complex long double*)in_ptr;
                long double real = creall(val);
                long double imag = cimagl(val);
                long double factor_ld = (long double)factor;
                long double real_rounded = rintl(real * factor_ld) / factor_ld;
                long double imag_rounded = rintl(imag * factor_ld) / factor_ld;
                *(complex long double*)out_ptr = real_rounded + imag_rounded * I;
                break;
            }
            default:
                // Should not reach here
                break;
        }
    }

    return result;
}