#include "array.h"


// Create array filled with zeros
Array* zeros(int *shape, int ndim, DataType dtype) {
    Array *arr = create_array(shape, ndim, dtype);
    if (arr == NULL) {
        return NULL;
    }

    size_t element_size = dtype_size(dtype);
    memset(arr->data, 0, arr->size * element_size);

    return arr;
}

// =========================== Create all-zero arrays ============================

// Create integer all-zero array (int32)
// Default data type is int32
Array* zeros_int(int *shape, int ndim) {
    return zeros(shape, ndim, INT32);
}

// Create unsigned integer all-zero array (uint32)
// Default data type is uint32
Array* zeros_uint(int *shape, int ndim) {
    return zeros(shape, ndim, UINT32);
}

// Create floating point all-zero array (float32)
// Default data type is float32
Array* zeros_float(int *shape, int ndim) {
    return zeros(shape, ndim, FLOAT32);
}

// Create complex all-zero array (complex64)
// Default data type is complex64
Array* zeros_complex(int *shape, int ndim) {
    return zeros(shape, ndim, COMPLEX64);
}

/*----------------------- Create integer all-zero array -----------------------*/

Array* zeros_int8(int *shape, int ndim) {
    return zeros(shape, ndim, INT8);
}

Array* zeros_int16(int *shape, int ndim) {
    return zeros(shape, ndim, INT16);
}

Array* zeros_int32(int *shape, int ndim) {
    return zeros(shape, ndim, INT32);
}

Array* zeros_int64(int *shape, int ndim) {
    return zeros(shape, ndim, INT64);
}

/*----------------------- Create unsigned integer all-zero array -----------------------*/
Array* zeros_uint8(int *shape, int ndim) {
    return zeros(shape, ndim, UINT8);
}

Array* zeros_uint16(int *shape, int ndim) {
    return zeros(shape, ndim, UINT16);
}

Array* zeros_uint32(int *shape, int ndim) {
    return zeros(shape, ndim, UINT32);
}

Array* zeros_uint64(int *shape, int ndim) {
    return zeros(shape, ndim, UINT64);
}

/*----------------------- Create floating point all-zero array -----------------------*/

Array* zeros_float32(int *shape, int ndim) {
    return zeros(shape, ndim, FLOAT32);
}

Array* zeros_float64(int *shape, int ndim) {
    return zeros(shape, ndim, FLOAT64);
}

/*----------------------- Create complex all-zero array -----------------------*/

Array* zeros_complex64(int *shape, int ndim) {
    return zeros(shape, ndim, COMPLEX64);
}

Array* zeros_complex128(int *shape, int ndim) {
    return zeros(shape, ndim, COMPLEX128);
}

Array* zeros_complex256(int *shape, int ndim) {
    return zeros(shape, ndim, COMPLEX256);
}