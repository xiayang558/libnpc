#include "npzfile.h"
#include "array.h"
#include <string.h>

DataType descr_to_dtype(const char *descr) {
    if (strcmp(descr, "|b1") == 0 || strcmp(descr, "|b") == 0) return BOOL;
    if (strcmp(descr, "|i1") == 0 || strcmp(descr, "<i1") == 0) return INT8;
    if (strcmp(descr, "<i2") == 0) return INT16;
    if (strcmp(descr, "<i4") == 0) return INT32;
    if (strcmp(descr, "<i8") == 0) return INT64;
    if (strcmp(descr, "|u1") == 0 || strcmp(descr, "<u1") == 0) return UINT8;
    if (strcmp(descr, "<u2") == 0) return UINT16;
    if (strcmp(descr, "<u4") == 0) return UINT32;
    if (strcmp(descr, "<u8") == 0) return UINT64;
    if (strcmp(descr, "<f4") == 0) return FLOAT32;
    if (strcmp(descr, "<f8") == 0) return FLOAT64;
    if (strcmp(descr, "<c8") == 0) return COMPLEX64;
    if (strcmp(descr, "<c16") == 0) return COMPLEX128;
    return UNKNOWN;
}
