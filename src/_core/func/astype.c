#include "array.h"


// typeconvertfunction
Array* astype(Array *arr, DataType new_dtype) {
    if (arr == NULL) {
        fprintf(stderr, "astype: NULL array argument\n");
        return NULL;
    }

    // Create new array, same shape, type = new_dtype
    Array *new_arr = create_array(arr->shape, arr->ndim, new_dtype);
    if (new_arr == NULL) {
        fprintf(stderr, "astype: failed to create new array\n");
        return NULL;
    }

    size_t src_elem_size = dtype_size(arr->dtype);
    size_t dst_elem_size = dtype_size(new_dtype);
    char *src_data = (char*)arr->data;
    char *dst_data = (char*)new_arr->data;

    // traversehaselementperformconvert
    for (int i = 0; i < arr->size; i++) {
        void *src_ptr = src_data + i * src_elem_size;
        void *dst_ptr = dst_data + i * dst_elem_size;

        // Perform conversion based on source and target types
        switch (arr->dtype) {
            case INT8: {
                int8_t val = *(int8_t*)src_ptr;
                switch (new_dtype) {
                    case INT8:   *(int8_t*)dst_ptr = val; break;
                    case INT16:  *(int16_t*)dst_ptr = (int16_t)val; break;
                    case INT32:  *(int32_t*)dst_ptr = (int32_t)val; break;
                    case INT64:  *(int64_t*)dst_ptr = (int64_t)val; break;
                    case UINT8:  *(uint8_t*)dst_ptr = (uint8_t)val; break;
                    case UINT16: *(uint16_t*)dst_ptr = (uint16_t)val; break;
                    case UINT32: *(uint32_t*)dst_ptr = (uint32_t)val; break;
                    case UINT64: *(uint64_t*)dst_ptr = (uint64_t)val; break;
                    case FLOAT32: *(float*)dst_ptr = (float)val; break;
                    case FLOAT64: *(double*)dst_ptr = (double)val; break;
                    case COMPLEX64: {
                        complex float *c = (complex float*)dst_ptr;
                        *c = val + 0.0f * I;
                        break;
                    }
                    case COMPLEX128: {
                        complex double *c = (complex double*)dst_ptr;
                        *c = val + 0.0 * I;
                        break;
                    }
                    default:
                        fprintf(stderr, "astype: unsupported target type for INT8\n");
                        free_array(new_arr);
                        return NULL;
                }
                break;
            }
            case INT16: {
                int16_t val = *(int16_t*)src_ptr;
                switch (new_dtype) {
                    case INT8:   *(int8_t*)dst_ptr = (int8_t)val; break;
                    case INT16:  *(int16_t*)dst_ptr = val; break;
                    case INT32:  *(int32_t*)dst_ptr = (int32_t)val; break;
                    case INT64:  *(int64_t*)dst_ptr = (int64_t)val; break;
                    case UINT8:  *(uint8_t*)dst_ptr = (uint8_t)val; break;
                    case UINT16: *(uint16_t*)dst_ptr = (uint16_t)val; break;
                    case UINT32: *(uint32_t*)dst_ptr = (uint32_t)val; break;
                    case UINT64: *(uint64_t*)dst_ptr = (uint64_t)val; break;
                    case FLOAT32: *(float*)dst_ptr = (float)val; break;
                    case FLOAT64: *(double*)dst_ptr = (double)val; break;
                    case COMPLEX64: {
                        complex float *c = (complex float*)dst_ptr;
                        *c = val + 0.0f * I;
                        break;
                    }
                    case COMPLEX128: {
                        complex double *c = (complex double*)dst_ptr;
                        *c = val + 0.0 * I;
                        break;
                    }
                    default:
                        fprintf(stderr, "astype: unsupported target type for INT16\n");
                        free_array(new_arr);
                        return NULL;
                }
                break;
            }
            case INT32: {
                int32_t val = *(int32_t*)src_ptr;
                switch (new_dtype) {
                    case INT8:   *(int8_t*)dst_ptr = (int8_t)val; break;
                    case INT16:  *(int16_t*)dst_ptr = (int16_t)val; break;
                    case INT32:  *(int32_t*)dst_ptr = val; break;
                    case INT64:  *(int64_t*)dst_ptr = (int64_t)val; break;
                    case UINT8:  *(uint8_t*)dst_ptr = (uint8_t)val; break;
                    case UINT16: *(uint16_t*)dst_ptr = (uint16_t)val; break;
                    case UINT32: *(uint32_t*)dst_ptr = (uint32_t)val; break;
                    case UINT64: *(uint64_t*)dst_ptr = (uint64_t)val; break;
                    case FLOAT32: *(float*)dst_ptr = (float)val; break;
                    case FLOAT64: *(double*)dst_ptr = (double)val; break;
                    case COMPLEX64: {
                        complex float *c = (complex float*)dst_ptr;
                        *c = val + 0.0f * I;
                        break;
                    }
                    case COMPLEX128: {
                        complex double *c = (complex double*)dst_ptr;
                        *c = val + 0.0 * I;
                        break;
                    }
                    default:
                        fprintf(stderr, "astype: unsupported target type for INT32\n");
                        free_array(new_arr);
                        return NULL;
                }
                break;
            }
            case INT64: {
                int64_t val = *(int64_t*)src_ptr;
                switch (new_dtype) {
                    case INT8:   *(int8_t*)dst_ptr = (int8_t)val; break;
                    case INT16:  *(int16_t*)dst_ptr = (int16_t)val; break;
                    case INT32:  *(int32_t*)dst_ptr = (int32_t)val; break;
                    case INT64:  *(int64_t*)dst_ptr = val; break;
                    case UINT8:  *(uint8_t*)dst_ptr = (uint8_t)val; break;
                    case UINT16: *(uint16_t*)dst_ptr = (uint16_t)val; break;
                    case UINT32: *(uint32_t*)dst_ptr = (uint32_t)val; break;
                    case UINT64: *(uint64_t*)dst_ptr = (uint64_t)val; break;
                    case FLOAT32: *(float*)dst_ptr = (float)val; break;
                    case FLOAT64: *(double*)dst_ptr = (double)val; break;
                    case COMPLEX64: {
                        complex float *c = (complex float*)dst_ptr;
                        *c = val + 0.0f * I;
                        break;
                    }
                    case COMPLEX128: {
                        complex double *c = (complex double*)dst_ptr;
                        *c = val + 0.0 * I;
                        break;
                    }
                    default:
                        fprintf(stderr, "astype: unsupported target type for INT64\n");
                        free_array(new_arr);
                        return NULL;
                }
                break;
            }
            case UINT8: {
                uint8_t val = *(uint8_t*)src_ptr;
                switch (new_dtype) {
                    case INT8:   *(int8_t*)dst_ptr = (int8_t)val; break;
                    case INT16:  *(int16_t*)dst_ptr = (int16_t)val; break;
                    case INT32:  *(int32_t*)dst_ptr = (int32_t)val; break;
                    case INT64:  *(int64_t*)dst_ptr = (int64_t)val; break;
                    case UINT8:  *(uint8_t*)dst_ptr = val; break;
                    case UINT16: *(uint16_t*)dst_ptr = (uint16_t)val; break;
                    case UINT32: *(uint32_t*)dst_ptr = (uint32_t)val; break;
                    case UINT64: *(uint64_t*)dst_ptr = (uint64_t)val; break;
                    case FLOAT32: *(float*)dst_ptr = (float)val; break;
                    case FLOAT64: *(double*)dst_ptr = (double)val; break;
                    case COMPLEX64: {
                        complex float *c = (complex float*)dst_ptr;
                        *c = val + 0.0f * I;
                        break;
                    }
                    case COMPLEX128: {
                        complex double *c = (complex double*)dst_ptr;
                        *c = val + 0.0 * I;
                        break;
                    }
                    default:
                        fprintf(stderr, "astype: unsupported target type for UINT8\n");
                        free_array(new_arr);
                        return NULL;
                }
                break;
            }
            case UINT16: {
                uint16_t val = *(uint16_t*)src_ptr;
                switch (new_dtype) {
                    case INT8:   *(int8_t*)dst_ptr = (int8_t)val; break;
                    case INT16:  *(int16_t*)dst_ptr = (int16_t)val; break;
                    case INT32:  *(int32_t*)dst_ptr = (int32_t)val; break;
                    case INT64:  *(int64_t*)dst_ptr = (int64_t)val; break;
                    case UINT8:  *(uint8_t*)dst_ptr = (uint8_t)val; break;
                    case UINT16: *(uint16_t*)dst_ptr = val; break;
                    case UINT32: *(uint32_t*)dst_ptr = (uint32_t)val; break;
                    case UINT64: *(uint64_t*)dst_ptr = (uint64_t)val; break;
                    case FLOAT32: *(float*)dst_ptr = (float)val; break;
                    case FLOAT64: *(double*)dst_ptr = (double)val; break;
                    case COMPLEX64: {
                        complex float *c = (complex float*)dst_ptr;
                        *c = val + 0.0f * I;
                        break;
                    }
                    case COMPLEX128: {
                        complex double *c = (complex double*)dst_ptr;
                        *c = val + 0.0 * I;
                        break;
                    }
                    default:
                        fprintf(stderr, "astype: unsupported target type for UINT16\n");
                        free_array(new_arr);
                        return NULL;
                }
                break;
            }
            case UINT32: {
                uint32_t val = *(uint32_t*)src_ptr;
                switch (new_dtype) {
                    case INT8:   *(int8_t*)dst_ptr = (int8_t)val; break;
                    case INT16:  *(int16_t*)dst_ptr = (int16_t)val; break;
                    case INT32:  *(int32_t*)dst_ptr = (int32_t)val; break;
                    case INT64:  *(int64_t*)dst_ptr = (int64_t)val; break;
                    case UINT8:  *(uint8_t*)dst_ptr = (uint8_t)val; break;
                    case UINT16: *(uint16_t*)dst_ptr = (uint16_t)val; break;
                    case UINT32: *(uint32_t*)dst_ptr = val; break;
                    case UINT64: *(uint64_t*)dst_ptr = (uint64_t)val; break;
                    case FLOAT32: *(float*)dst_ptr = (float)val; break;
                    case FLOAT64: *(double*)dst_ptr = (double)val; break;
                    case COMPLEX64: {
                        complex float *c = (complex float*)dst_ptr;
                        *c = val + 0.0f * I;
                        break;
                    }
                    case COMPLEX128: {
                        complex double *c = (complex double*)dst_ptr;
                        *c = val + 0.0 * I;
                        break;
                    }
                    default:
                        fprintf(stderr, "astype: unsupported target type for UINT32\n");
                        free_array(new_arr);
                        return NULL;
                }
                break;
            }
            case UINT64: {
                uint64_t val = *(uint64_t*)src_ptr;
                switch (new_dtype) {
                    case INT8:   *(int8_t*)dst_ptr = (int8_t)val; break;
                    case INT16:  *(int16_t*)dst_ptr = (int16_t)val; break;
                    case INT32:  *(int32_t*)dst_ptr = (int32_t)val; break;
                    case INT64:  *(int64_t*)dst_ptr = (int64_t)val; break;
                    case UINT8:  *(uint8_t*)dst_ptr = (uint8_t)val; break;
                    case UINT16: *(uint16_t*)dst_ptr = (uint16_t)val; break;
                    case UINT32: *(uint32_t*)dst_ptr = (uint32_t)val; break;
                    case UINT64: *(uint64_t*)dst_ptr = val; break;
                    case FLOAT32: *(float*)dst_ptr = (float)val; break;
                    case FLOAT64: *(double*)dst_ptr = (double)val; break;
                    case COMPLEX64: {
                        complex float *c = (complex float*)dst_ptr;
                        *c = val + 0.0f * I;
                        break;
                    }
                    case COMPLEX128: {
                        complex double *c = (complex double*)dst_ptr;
                        *c = val + 0.0 * I;
                        break;
                    }
                    default:
                        fprintf(stderr, "astype: unsupported target type for UINT64\n");
                        free_array(new_arr);
                        return NULL;
                }
                break;
            }
            case FLOAT32: {
                float val = *(float*)src_ptr;
                switch (new_dtype) {
                    case INT8:   *(int8_t*)dst_ptr = (int8_t)val; break;
                    case INT16:  *(int16_t*)dst_ptr = (int16_t)val; break;
                    case INT32:  *(int32_t*)dst_ptr = (int32_t)val; break;
                    case INT64:  *(int64_t*)dst_ptr = (int64_t)val; break;
                    case UINT8:  *(uint8_t*)dst_ptr = (uint8_t)val; break;
                    case UINT16: *(uint16_t*)dst_ptr = (uint16_t)val; break;
                    case UINT32: *(uint32_t*)dst_ptr = (uint32_t)val; break;
                    case UINT64: *(uint64_t*)dst_ptr = (uint64_t)val; break;
                    case FLOAT32: *(float*)dst_ptr = val; break;
                    case FLOAT64: *(double*)dst_ptr = (double)val; break;
                    case COMPLEX64: {
                        complex float *c = (complex float*)dst_ptr;
                        *c = val + 0.0f * I;
                        break;
                    }
                    case COMPLEX128: {
                        complex double *c = (complex double*)dst_ptr;
                        *c = val + 0.0 * I;
                        break;
                    }
                    default:
                        fprintf(stderr, "astype: unsupported target type for FLOAT32\n");
                        free_array(new_arr);
                        return NULL;
                }
                break;
            }
            case FLOAT64: {
                double val = *(double*)src_ptr;
                switch (new_dtype) {
                    case INT8:   *(int8_t*)dst_ptr = (int8_t)val; break;
                    case INT16:  *(int16_t*)dst_ptr = (int16_t)val; break;
                    case INT32:  *(int32_t*)dst_ptr = (int32_t)val; break;
                    case INT64:  *(int64_t*)dst_ptr = (int64_t)val; break;
                    case UINT8:  *(uint8_t*)dst_ptr = (uint8_t)val; break;
                    case UINT16: *(uint16_t*)dst_ptr = (uint16_t)val; break;
                    case UINT32: *(uint32_t*)dst_ptr = (uint32_t)val; break;
                    case UINT64: *(uint64_t*)dst_ptr = (uint64_t)val; break;
                    case FLOAT32: *(float*)dst_ptr = (float)val; break;
                    case FLOAT64: *(double*)dst_ptr = val; break;
                    case COMPLEX64: {
                        complex float *c = (complex float*)dst_ptr;
                        *c = val + 0.0f * I;
                        break;
                    }
                    case COMPLEX128: {
                        complex double *c = (complex double*)dst_ptr;
                        *c = val + 0.0 * I;
                        break;
                    }
                    default:
                        fprintf(stderr, "astype: unsupported target type for FLOAT64\n");
                        free_array(new_arr);
                        return NULL;
                }
                break;
            }
            case COMPLEX64: {
                complex float val = *(complex float*)src_ptr;
                switch (new_dtype) {
                    case INT8:   *(int8_t*)dst_ptr = (int8_t)crealf(val); break;
                    case INT16:  *(int16_t*)dst_ptr = (int16_t)crealf(val); break;
                    case INT32:  *(int32_t*)dst_ptr = (int32_t)crealf(val); break;
                    case INT64:  *(int64_t*)dst_ptr = (int64_t)crealf(val); break;
                    case UINT8:  *(uint8_t*)dst_ptr = (uint8_t)crealf(val); break;
                    case UINT16: *(uint16_t*)dst_ptr = (uint16_t)crealf(val); break;
                    case UINT32: *(uint32_t*)dst_ptr = (uint32_t)crealf(val); break;
                    case UINT64: *(uint64_t*)dst_ptr = (uint64_t)crealf(val); break;
                    case FLOAT32: *(float*)dst_ptr = crealf(val); break;
                    case FLOAT64: *(double*)dst_ptr = (double)crealf(val); break;
                    case COMPLEX64: *(complex float*)dst_ptr = val; break;
                    case COMPLEX128: {
                        complex double *c = (complex double*)dst_ptr;
                        *c = (complex double)val;
                        break;
                    }
                    default:
                        fprintf(stderr, "astype: unsupported target type for COMPLEX64\n");
                        free_array(new_arr);
                        return NULL;
                }
                break;
            }
            case COMPLEX128: {
                complex double val = *(complex double*)src_ptr;
                switch (new_dtype) {
                    case INT8:   *(int8_t*)dst_ptr = (int8_t)creal(val); break;
                    case INT16:  *(int16_t*)dst_ptr = (int16_t)creal(val); break;
                    case INT32:  *(int32_t*)dst_ptr = (int32_t)creal(val); break;
                    case INT64:  *(int64_t*)dst_ptr = (int64_t)creal(val); break;
                    case UINT8:  *(uint8_t*)dst_ptr = (uint8_t)creal(val); break;
                    case UINT16: *(uint16_t*)dst_ptr = (uint16_t)creal(val); break;
                    case UINT32: *(uint32_t*)dst_ptr = (uint32_t)creal(val); break;
                    case UINT64: *(uint64_t*)dst_ptr = (uint64_t)creal(val); break;
                    case FLOAT32: *(float*)dst_ptr = (float)creal(val); break;
                    case FLOAT64: *(double*)dst_ptr = creal(val); break;
                    case COMPLEX64: {
                        complex float *c = (complex float*)dst_ptr;
                        *c = (complex float)val;
                        break;
                    }
                    case COMPLEX128: *(complex double*)dst_ptr = val; break;
                    default:
                        fprintf(stderr, "astype: unsupported target type for COMPLEX128\n");
                        free_array(new_arr);
                        return NULL;
                }
                break;
            }
            default:
                fprintf(stderr, "astype: unsupported source data type\n");
                free_array(new_arr);
                return NULL;
        }
    }

    return new_arr;
}