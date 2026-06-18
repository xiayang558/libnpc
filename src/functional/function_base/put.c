#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "array.h"
#include "utils.h"

/**
 * put  --  in-place replace array elements based on flat indices (like numpy.put)
 *
 * @param arr     Input array (modified in-place)
 * @param indices 1D INT64 array, flat indices
 * @param values  Value array, cycled
 * @param mode    "raise" / "wrap" / "clip"
 *                raise: error on out-of-bounds
 *                wrap:  out-of-bounds modulo (ind[i] % n)
 *                clip:  out-of-bounds clamped to [0, n-1]
 * @return       number of elements successfully replaced
 */
int put(Array *arr, Array *indices, Array *values, const char *mode) {
    if (arr == NULL || indices == NULL || values == NULL || mode == NULL) {
        fprintf(stderr, "put: NULL argument\n");
        return -1;
    }
    if (indices->ndim != 1) {
        fprintf(stderr, "put: indices must be 1D array\n");
        return -1;
    }

    int ni = indices->size;
    int nv = values->size;
    int na = arr->size;
    size_t arr_sz = dtype_size(arr->dtype);
    size_t val_sz = dtype_size(values->dtype);
    char *arr_d = (char*)arr->data;
    char *val_d = (char*)values->data;

    for (int i = 0; i < ni; i++) {
        int64_t idx = *(int64_t*)((char*)indices->data + i * sizeof(int64_t));

        if (idx < 0 || idx >= na) {
            if (mode[0] == 'r') {
                fprintf(stderr, "put: index %lld out of bounds [0,%d]\n",
                        (long long)idx, na);
                return -1;
            } else if (mode[0] == 'c') {
                /* clip: clamp to [0, na-1] */
                if (idx < 0) idx = 0;
                if (idx >= na) idx = na - 1;
            } else if (mode[0] == 'w') {
                /* wrap: mod */
                idx = idx % na;
                if (idx < 0) idx += na;
            }
        }

        void *src = val_d + (i % nv) * val_sz;
        void *dst = arr_d + idx * arr_sz;

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
                default: break;
            }
        }
    }

    return ni;
}
