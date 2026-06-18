#include "array.h"

// get type name from dtype
const char* dtype_name(DataType dtype) {
    switch (dtype) {
        case INT8:
            return "int8";
        case INT16:
            return "int16";
        case INT32:
            return "int32";
        case INT64:
            return "int64";
        case UINT8:
            return "uint8";
        case UINT16:
            return "uint16";
        case UINT32:
            return "uint32";
        case UINT64:
            return "uint64";
        case FLOAT32:
            return "float32";
        case FLOAT64:
            return "float64";
        case COMPLEX64:
            return "complex64";
        case COMPLEX128:
            return "complex128";
        case COMPLEX256:
            return "complex256";
        default:
            return "unknown";
    }
}