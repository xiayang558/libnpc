#include "array.h"


// Create array filled with a specific value
Array* full(int *shape, int ndim, DataType dtype, void *value) {
    Array *arr = create_array(shape, ndim, dtype);
    if (arr == NULL || value == NULL) {
        return NULL;
    }

    size_t element_size = dtype_size(dtype);

    for (int i = 0; i < arr->size; i++) {
        memcpy((char*)arr->data + i * element_size, value, element_size);
    }

    return arr;
}

// =========================== Create array filled with specific value ============================

// Create integer fill array (int32)
Array* full_int(int *shape, int ndim, int value) {
    return full(shape, ndim, INT32, &value);
}

// Create unsigned integer fill array (uint32)
Array* full_uint(int *shape, int ndim, unsigned int value) {
    return full(shape, ndim, UINT32, &value);
}

// Create floating point fill array (float32)
Array* full_float(int *shape, int ndim, float value) {
    return full(shape, ndim, FLOAT32, &value);
}

// Create complex fill array (complex64)
Array* full_complex(int *shape, int ndim, complex float value) {
    return full(shape, ndim, COMPLEX64, &value);
}

/*--------------------------- Create integer fill array ---------------------------*/

Array* full_int8(int *shape, int ndim, int8_t value) {
    return full(shape, ndim, INT8, &value);
}

Array* full_int16(int *shape, int ndim, int16_t value) {
    return full(shape, ndim, INT16, &value);
}

Array* full_int32(int *shape, int ndim, int32_t value) {
    return full(shape, ndim, INT32, &value);
}

Array* full_int64(int *shape, int ndim, int64_t value) {
    return full(shape, ndim, INT64, &value);
}

/*--------------------------- Create unsigned integer fill array ---------------------------*/

Array* full_uint8(int *shape, int ndim, uint8_t value) {
    return full(shape, ndim, UINT8, &value);
}

Array* full_uint16(int *shape, int ndim, uint16_t value) {
    return full(shape, ndim, UINT16, &value);
}

Array* full_uint32(int *shape, int ndim, uint32_t value) {
    return full(shape, ndim, UINT32, &value);
}

Array* full_uint64(int *shape, int ndim, uint64_t value) {
    return full(shape, ndim, UINT64, &value);
}

/*--------------------------- Create floating point fill array ---------------------------*/

Array* full_float32(int *shape, int ndim, float value) {
    return full(shape, ndim, FLOAT32, &value);
}

Array* full_float64(int *shape, int ndim, double value) {
    return full(shape, ndim, FLOAT64, &value);
}

/*--------------------------- Create complex fill array ---------------------------*/

Array* full_complex64(int *shape, int ndim, complex float value) {
    return full(shape, ndim, COMPLEX64, &value);
}

Array* full_complex128(int *shape, int ndim, complex double value) {
    return full(shape, ndim, COMPLEX128, &value);
}

Array* full_complex256(int *shape, int ndim, complex long double value) {
    return full(shape, ndim, COMPLEX256, &value);
}


/*--------------------------- Create fill array with specified type -------------------------*/
Array* full_like(Array *arr, void *value, DataType dtype) {
    if (arr == NULL || value == NULL) {
        fprintf(stderr, "full_like: NULL argument\n");
        return NULL;
    }
    // Determine final data type
    DataType out_dtype = (dtype == UNKNOWN) ? arr->dtype : dtype;
    // Create new array, same shape as arr
    Array *res = create_array(arr->shape, arr->ndim, out_dtype);
    if (res == NULL) return NULL;

    // Fill data
    size_t elem_sz = dtype_size(out_dtype);
    char *data = (char*)res->data;
    for (int i = 0; i < res->size; i++) {
        memcpy(data + i * elem_sz, value, elem_sz);
    }
    return res;
}