#include "array.h"
DataType promote_type(DataType t1, DataType t2) {
    if (t1 == t2) return t1;
    
    // complex has highest priority
    if (t1 == COMPLEX256 || t2 == COMPLEX256) return COMPLEX256;
    if (t1 == COMPLEX128 || t2 == COMPLEX128) return COMPLEX128;
    if (t1 == COMPLEX64 || t2 == COMPLEX64) return COMPLEX64;
    
    // floating point
    if (t1 == FLOAT128 || t2 == FLOAT128) return FLOAT128;
    if (t1 == FLOAT64 || t2 == FLOAT64) return FLOAT64;
    if (t1 == FLOAT32 || t2 == FLOAT32) return FLOAT32;
    
    // integer: signed takes priority over unsigned
    int is_signed1 = (t1 == INT8 || t1 == INT16 || t1 == INT32 || t1 == INT64);
    int is_signed2 = (t2 == INT8 || t2 == INT16 || t2 == INT32 || t2 == INT64);
    if (is_signed1 || is_signed2) return INT64;
    
    // unsigned integer
    return UINT64;
}

// ====================== element copy and convert ======================
// copy source array elements to target array, performing type conversion when necessary
