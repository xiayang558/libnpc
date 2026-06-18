#include <stdlib.h>
#include "array.h"

/**
 * digitize — return bin indices for each value in arr (like numpy.digitize)
 *
 * Given bins[0..M], returns indices 0..M for each value in arr:
 *   bin 0:  x <  bins[0]    (or x <= bins[0] if right)
 *   bin 1:  x in [bins[0], bins[1])  (or (bins[0], bins[1]] if right)
 *   ...
 *   bin M:  x >= bins[M-1]  (or x >  bins[M-1] if right)
 *
 * @param arr   Input array (any shape, any real dtype)
 * @param bins  1D monotonic increasing array of bin edges
 * @param right If 0, intervals are left-closed [); if 1, right-closed (]
 * @return INT64 array, same shape as arr, with bin indices 0..len(bins)
 */
Array* digitize(Array *arr, Array *bins, int right) {
    if (arr == NULL || bins == NULL) {
        fprintf(stderr, "digitize: NULL array argument\n");
        return NULL;
    }
    if (bins->ndim != 1) {
        fprintf(stderr, "digitize: bins must be 1D\n");
        return NULL;
    }

    int nbins = bins->size;
    if (nbins == 0) {
        fprintf(stderr, "digitize: bins must have at least one element\n");
        return NULL;
    }

    /* Extract bin values as doubles */
    double *bvals = malloc(nbins * sizeof(double));
    if (!bvals) return NULL;
    size_t bsz = dtype_size(bins->dtype);
    char *bdata = (char*)bins->data;
    for (int i = 0; i < nbins; i++) {
        void *pb = bdata + i * bsz;
        switch (bins->dtype) {
            case BOOL:   bvals[i] = *(uint8_t*)pb; break;
            case INT8:   bvals[i] = *(int8_t*)pb; break;
            case INT16:  bvals[i] = *(int16_t*)pb; break;
            case INT32:  bvals[i] = *(int32_t*)pb; break;
            case INT64:  bvals[i] = *(int64_t*)pb; break;
            case UINT8:  bvals[i] = *(uint8_t*)pb; break;
            case UINT16: bvals[i] = *(uint16_t*)pb; break;
            case UINT32: bvals[i] = *(uint32_t*)pb; break;
            case UINT64: bvals[i] = *(uint64_t*)pb; break;
            case FLOAT32: bvals[i] = *(float*)pb; break;
            case FLOAT64: bvals[i] = *(double*)pb; break;
            default: bvals[i] = 0.0; break;
        }
    }

    Array *result = create_array(arr->shape, arr->ndim, INT64);
    if (!result) { free(bvals); return NULL; }
    int64_t *rd = (int64_t*)result->data;

    size_t asz = dtype_size(arr->dtype);
    char *adata = (char*)arr->data;

    for (int i = 0; i < arr->size; i++) {
        void *pa = adata + i * asz;
        double val;
        switch (arr->dtype) {
            case BOOL:   val = *(uint8_t*)pa; break;
            case INT8:   val = *(int8_t*)pa; break;
            case INT16:  val = *(int16_t*)pa; break;
            case INT32:  val = *(int32_t*)pa; break;
            case INT64:  val = *(int64_t*)pa; break;
            case UINT8:  val = *(uint8_t*)pa; break;
            case UINT16: val = *(uint16_t*)pa; break;
            case UINT32: val = *(uint32_t*)pa; break;
            case UINT64: val = *(uint64_t*)pa; break;
            case FLOAT32: val = *(float*)pa; break;
            case FLOAT64: val = *(double*)pa; break;
            default: val = 0.0; break;
        }

        /* Binary search in monotonically increasing bins */
        int lo = 0, hi = nbins;
        if (right) {
            /* right=True: intervals are (bins[i-1], bins[i]] */
            while (lo < hi) {
                int mid = (lo + hi) / 2;
                if (bvals[mid] < val)
                    lo = mid + 1;
                else
                    hi = mid;
            }
        } else {
            /* right=False: intervals are [bins[i-1], bins[i]) */
            while (lo < hi) {
                int mid = (lo + hi) / 2;
                if (bvals[mid] <= val)
                    lo = mid + 1;
                else
                    hi = mid;
            }
        }
        rd[i] = lo;
    }

    free(bvals);
    return result;
}
