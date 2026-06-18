#include "array.h"


// createfull1array
Array* ones(int *shape, int ndim, DataType dtype) {
    Array *arr = create_array(shape, ndim, dtype);
    if (arr == NULL) {
        return NULL;
    }

    size_t element_size = dtype_size(dtype);

    switch (dtype) {
        case INT8: {
            int8_t one = 1;
            for (int i = 0; i < arr->size; i++) {
                memcpy((char*)arr->data + i * element_size, &one, element_size);
            }
            break;
        }
        case INT16: {
            int16_t one = 1;
            for (int i = 0; i < arr->size; i++) {
                memcpy((char*)arr->data + i * element_size, &one, element_size);
            }
            break;
        }
        case INT32: {
            int32_t one = 1;
            for (int i = 0; i < arr->size; i++) {
                memcpy((char*)arr->data + i * element_size, &one, element_size);
            }
            break;
        }
        case INT64: {
            int64_t one = 1;
            for (int i = 0; i < arr->size; i++) {
                memcpy((char*)arr->data + i * element_size, &one, element_size);
            }
            break;
        }
        case UINT8: {
            uint8_t one = 1;
            for (int i = 0; i < arr->size; i++) {
                memcpy((char*)arr->data + i * element_size, &one, element_size);
            }
            break;
        }
        case UINT16: {
            uint16_t one = 1;
            for (int i = 0; i < arr->size; i++) {
                memcpy((char*)arr->data + i * element_size, &one, element_size);
            }
            break;
        }
        case UINT32: {
            uint32_t one = 1;
            for (int i = 0; i < arr->size; i++) {
                memcpy((char*)arr->data + i * element_size, &one, element_size);
            }
            break;
        }
        case UINT64: {
            uint64_t one = 1;
            for (int i = 0; i < arr->size; i++) {
                memcpy((char*)arr->data + i * element_size, &one, element_size);
            }
            break;
        }
        case FLOAT32: {
            float one = 1.0f;
            for (int i = 0; i < arr->size; i++) {
                memcpy((char*)arr->data + i * element_size, &one, element_size);
            }
            break;
        }
        case FLOAT64: {
            double one = 1.0;
            for (int i = 0; i < arr->size; i++) {
                memcpy((char*)arr->data + i * element_size, &one, element_size);
            }
            break;
        }
        case COMPLEX64: {
            complex float one = 1.0f + 0.0f * I;
            for (int i = 0; i < arr->size; i++) {
                memcpy((char*)arr->data + i * element_size, &one, element_size);
            }
            break;
        }
        case COMPLEX128: {
            complex double one = 1.0 + 0.0 * I;
            for (int i = 0; i < arr->size; i++) {
                memcpy((char*)arr->data + i * element_size, &one, element_size);
            }
            break;
        }
        case COMPLEX256: {
            complex long double one = 1.0L + 0.0L * I;
            for (int i = 0; i < arr->size; i++) {
                memcpy((char*)arr->data + i * element_size, &one, element_size);
            }
            break;
        }
        default:
            fprintf(stderr, "Unsupported data type\n");
            free_array(arr);
            return NULL;
    }

    return arr;
}

// =========================== createfull1array ============================

// Create integer all-ones array (int32)
Array* ones_int(int *shape, int ndim) {
    return ones(shape, ndim, INT32);
}

// Create unsigned integer all-ones array (uint32)
Array* ones_uint(int *shape, int ndim) {
    return ones(shape, ndim, UINT32);
}

// Create float all-ones array (float32)
Array* ones_float(int *shape, int ndim) {
    return ones(shape, ndim, FLOAT32);
}

// Create complex all-ones array (complex64)
Array* ones_complex(int *shape, int ndim) {
    return ones(shape, ndim, COMPLEX64);
}

/*--------------------------- create all-ones integer array ---------------------------*/

Array* ones_int8(int *shape, int ndim) {
    return ones(shape, ndim, INT8);
}

Array* ones_int16(int *shape, int ndim) {
    return ones(shape, ndim, INT16);
}

Array* ones_int32(int *shape, int ndim) {
    return ones(shape, ndim, INT32);
}

Array* ones_int64(int *shape, int ndim) {
    return ones(shape, ndim, INT64);
}

/*--------------------------- create all-ones unsigned integer array ---------------------------*/

Array* ones_uint8(int *shape, int ndim) {
    return ones(shape, ndim, UINT8);
}

Array* ones_uint16(int *shape, int ndim) {
    return ones(shape, ndim, UINT16);
}

Array* ones_uint32(int *shape, int ndim) {
    return ones(shape, ndim, UINT32);
}

Array* ones_uint64(int *shape, int ndim) {
    return ones(shape, ndim, UINT64);
}

/*--------------------------- create all-ones float array ---------------------------*/

Array* ones_float32(int *shape, int ndim) {
    return ones(shape, ndim, FLOAT32);
}

Array* ones_float64(int *shape, int ndim) {
    return ones(shape, ndim, FLOAT64);
}

/*--------------------------- create all-ones complex array ---------------------------*/

Array* ones_complex64(int *shape, int ndim) {
    return ones(shape, ndim, COMPLEX64);
}

Array* ones_complex128(int *shape, int ndim) {
    return ones(shape, ndim, COMPLEX128);
}

Array* ones_complex256(int *shape, int ndim) {
    return ones(shape, ndim, COMPLEX256);
}
