#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "array.h"
#include "utils.h"

/**
 * place -- replace array elements in-place based on mask (like numpy.place)
 *
 * @param arr  Input array (modified in-place)
 * @param mask BOOL array, same shape as arr
 * @param vals Replacement value array, used cyclically
 * @return     successreplaceelementnumber, failedReturn -1
 */
int place(Array *arr, Array *mask, Array *vals) {
    if (arr == NULL || mask == NULL || vals == NULL) {
        fprintf(stderr, "place: NULL argument\n");
        return -1;
    }
    if (mask->dtype != BOOL) {
        fprintf(stderr, "place: mask must be BOOL type\n");
        return -1;
    }
    if (arr->ndim != mask->ndim) {
        fprintf(stderr, "place: arr and mask must have same dimensions\n");
        return -1;
    }
    for (int i = 0; i < arr->ndim; i++) {
        if (arr->shape[i] != mask->shape[i]) {
            fprintf(stderr, "place: arr and mask must have same shape\n");
            return -1;
        }
    }

    int n = arr->size;
    int nv = vals->size;
    uint8_t *md = (uint8_t*)mask->data;
    int count = 0;

    /* Count matches first */
    for (int i = 0; i < n; i++) if (md[i]) count++;

    size_t arr_sz = dtype_size(arr->dtype);
    size_t val_sz = dtype_size(vals->dtype);
    char *arr_d = (char*)arr->data;
    char *val_d = (char*)vals->data;

    int vi = 0;
    for (int i = 0; i < n; i++) {
        if (!md[i]) continue;

        void *src = val_d + (vi % nv) * val_sz;
        void *dst = arr_d + i * arr_sz;

        /* Convert val to arr's dtype */
        if (arr->dtype == vals->dtype) {
            memcpy(dst, src, arr_sz);
        } else {
            double sv = get_value(src, vals->dtype);
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
        vi++;
    }

    return count;
}
