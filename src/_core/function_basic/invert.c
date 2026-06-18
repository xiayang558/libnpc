#include <stdlib.h>
#include <string.h>
#include "array.h"

/**
 * invert -- element-wise bitwise negation (like numpy.invert / ~x)
 */
Array* invert(Array *arr) {
    if (arr == NULL) {
        fprintf(stderr, "invert: NULL argument\n"); return NULL;
    }

    Array *result = create_array(arr->shape, arr->ndim, arr->dtype);
    if (!result) return NULL;

    size_t esz = dtype_size(arr->dtype);
    char *src = (char*)arr->data;
    char *dst = (char*)result->data;

    for (int i = 0; i < arr->size; i++) {
        switch (arr->dtype) {
            case BOOL: {
                uint8_t v = *(uint8_t*)(src + i * esz);
                uint8_t r = !v;
                memcpy(dst + i * esz, &r, esz);
                break;
            }
            case INT8: {
                int8_t v = *(int8_t*)(src + i * esz);
                int8_t r = ~v;
                memcpy(dst + i * esz, &r, esz);
                break;
            }
            case INT16: {
                int16_t v = *(int16_t*)(src + i * esz);
                int16_t r = ~v;
                memcpy(dst + i * esz, &r, esz);
                break;
            }
            case INT32: {
                int32_t v = *(int32_t*)(src + i * esz);
                int32_t r = ~v;
                memcpy(dst + i * esz, &r, esz);
                break;
            }
            case INT64: {
                int64_t v = *(int64_t*)(src + i * esz);
                int64_t r = ~v;
                memcpy(dst + i * esz, &r, esz);
                break;
            }
            case UINT8: {
                uint8_t v = *(uint8_t*)(src + i * esz);
                uint8_t r = ~v;
                memcpy(dst + i * esz, &r, esz);
                break;
            }
            case UINT16: {
                uint16_t v = *(uint16_t*)(src + i * esz);
                uint16_t r = ~v;
                memcpy(dst + i * esz, &r, esz);
                break;
            }
            case UINT32: {
                uint32_t v = *(uint32_t*)(src + i * esz);
                uint32_t r = ~v;
                memcpy(dst + i * esz, &r, esz);
                break;
            }
            case UINT64: {
                uint64_t v = *(uint64_t*)(src + i * esz);
                uint64_t r = ~v;
                memcpy(dst + i * esz, &r, esz);
                break;
            }
            default:
                fprintf(stderr, "invert: unsupported dtype\n");
                free_array(result); return NULL;
        }
    }

    return result;
}
