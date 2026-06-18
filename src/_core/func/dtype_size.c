#include "array.h"
#include "datetime64.h"

// getnumberbased ontypelargesmall
size_t dtype_size(DataType dtype) {
    switch (dtype) {
        case BOOL:
            return sizeof(char);
        case INT8:
            return sizeof(int8_t);
        case INT16:
            return sizeof(int16_t);
        case INT32:
            return sizeof(int32_t);
        case INT64:
            return sizeof(int64_t);
        case UINT8:
            return sizeof(uint8_t);
        case UINT16:
            return sizeof(uint16_t);
        case UINT32:
            return sizeof(uint32_t);
        case UINT64:
            return sizeof(uint64_t);
        case FLOAT32:
            return sizeof(float);
        case FLOAT64:
            return sizeof(double);
        case FLOAT128:
            return sizeof(long double);
        case COMPLEX64:
            return sizeof(complex float);
        case COMPLEX128:
            return sizeof(complex double);
        case COMPLEX256:
            return sizeof(complex long double);
        case DATETIME64:
            return sizeof(DateTime64);
        case STRING:
            return sizeof(char*);
        default:
            fprintf(stderr, "Unsupported data type\n");
            return 0;
    }
}