#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "array.h"
#include "utils.h"

/**
 * putmask  --  in-place replace array elements based on mask (like numpy.putmask)
 *
 * for every n where mask.flat[n] == True, execute arr.flat[n] = values[n % nv]
 * values are cycled by flat position (not by match count)
 *
 * @param arr    Input array (modified in-place)
 * @param mask   BOOL array, same size as arr
 * @param values Replacement value array, cycled by flat position
 */
void putmask(Array *arr, Array *mask, Array *values) {
    if (arr == NULL || mask == NULL || values == NULL) {
        fprintf(stderr, "putmask: NULL argument\n");
        return;
    }
    if (mask->dtype != BOOL) {
        fprintf(stderr, "putmask: mask must be BOOL type\n");
        return;
    }
    if (arr->size != mask->size) {
        fprintf(stderr, "putmask: arr and mask must have same size\n");
        return;
    }

    int n = arr->size;
    int nv = values->size;
    size_t arr_sz = dtype_size(arr->dtype);
    size_t val_sz = dtype_size(values->dtype);
    char *arr_d = (char*)arr->data;
    char *val_d = (char*)values->data;
    uint8_t *md = (uint8_t*)mask->data;

    for (int i = 0; i < n; i++) {
        if (!md[i]) continue;

        void *src = val_d + (i % nv) * val_sz;
        void *dst = arr_d + i * arr_sz;

        if (arr->dtype == values->dtype) {
            memcpy(dst, src, arr_sz);
        } else {
            double sv = get_value(src, values->dtype);
            switch (arr->dtype) {
                case INT8:   *(int8_t*)dst = (int8_t)sv; break;
                case INT16:  *(int16_t*)dst = (int16_t)sv; break;
                case INT32:  *(int32_t*)dst = (int32_t)sv; break;
                case INT64:  *(int64_t*)dst = (int64_t)sv; break;
                case UINT8:  *(uint8_t*)dst = (uint8_t)sv; break;
                case UINT16: *(uint16_t*)dst = (uint16_t)sv; break;
                case UINT32: *(uint32_t*)dst = (uint32_t)sv; break;
                case UINT64: *(uint64_t*)dst = (uint64_t)sv; break;
                case FLOAT32: *(float*)dst = (float)sv; break;
                case FLOAT64: *(double*)dst = sv; break;
                case COMPLEX64: *(complex float*)dst = (float)sv + 0.0f*I; break;
                case COMPLEX128: *(complex double*)dst = sv + 0.0*I; break;
                default: break;
            }
        }
    }
}
