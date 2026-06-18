#include "array.h"
int is_integer(DataType dtype) {
    switch (dtype) {
        case BOOL:
        case INT8:
        case INT16:
        case INT32:
        case INT64:
        case UINT8:
        case UINT16:
        case UINT32:
        case UINT64:
            return TRUE;
        default:
            return FALSE;
    }
}

// helper function: checkfloating pointnumberis notjoinnearinteger
