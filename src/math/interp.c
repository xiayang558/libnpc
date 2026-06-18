#include <stdlib.h>
#include "array.h"
#include "function.h"

/**
 * interp — 1D linear interpolation (like numpy.interp)
 *
 * Evaluates y = f(x) by linearly interpolating between (xp[k], fp[k]) points.
 * xp must be strictly monotonically increasing.
 *
 * @param arr   x-coordinates to evaluate at (any shape)
 * @param xp    1D array of known x-coordinates (must be increasing)
 * @param fp    1D array of known y-coordinates (same length as xp)
 * @param left  Optional fill value for x < xp[0] (NULL = use fp[0])
 * @param right Optional fill value for x > xp[-1] (NULL = use fp[-1])
 * @return FLOAT64 array, same shape as arr
 */
Array* interp(Array *arr, Array *xp, Array *fp, Array *left, Array *right) {
    if (arr == NULL || xp == NULL || fp == NULL) {
        fprintf(stderr, "interp: NULL array argument\n");
        return NULL;
    }
    if (xp->ndim != 1 || fp->ndim != 1) {
        fprintf(stderr, "interp: xp and fp must be 1D\n");
        return NULL;
    }
    if (xp->size != fp->size) {
        fprintf(stderr, "interp: xp and fp must have the same length\n");
        return NULL;
    }
    int n = xp->size;
    if (n == 0) {
        fprintf(stderr, "interp: xp and fp must have at least one element\n");
        return NULL;
    }

    /* Extract xp as double array */
    double *xv = malloc(n * sizeof(double));
    double *yv = malloc(n * sizeof(double));
    if (!xv || !yv) { free(xv); free(yv); return NULL; }

    size_t xsz = dtype_size(xp->dtype);
    size_t fsz = dtype_size(fp->dtype);
    char *xdata = (char*)xp->data;
    char *fdata = (char*)fp->data;

    for (int i = 0; i < n; i++) {
        void *px = xdata + i * xsz;
        void *pf = fdata + i * fsz;
        switch (xp->dtype) {
            case BOOL:   xv[i] = *(uint8_t*)px; break;
            case INT8:   xv[i] = *(int8_t*)px; break;
            case INT16:  xv[i] = *(int16_t*)px; break;
            case INT32:  xv[i] = *(int32_t*)px; break;
            case INT64:  xv[i] = *(int64_t*)px; break;
            case UINT8:  xv[i] = *(uint8_t*)px; break;
            case UINT16: xv[i] = *(uint16_t*)px; break;
            case UINT32: xv[i] = *(uint32_t*)px; break;
            case UINT64: xv[i] = *(uint64_t*)px; break;
            case FLOAT32: xv[i] = *(float*)px; break;
            case FLOAT64: xv[i] = *(double*)px; break;
            default: xv[i] = 0.0; break;
        }
        switch (fp->dtype) {
            case BOOL:   yv[i] = *(uint8_t*)pf; break;
            case INT8:   yv[i] = *(int8_t*)pf; break;
            case INT16:  yv[i] = *(int16_t*)pf; break;
            case INT32:  yv[i] = *(int32_t*)pf; break;
            case INT64:  yv[i] = *(int64_t*)pf; break;
            case UINT8:  yv[i] = *(uint8_t*)pf; break;
            case UINT16: yv[i] = *(uint16_t*)pf; break;
            case UINT32: yv[i] = *(uint32_t*)pf; break;
            case UINT64: yv[i] = *(uint64_t*)pf; break;
            case FLOAT32: yv[i] = *(float*)pf; break;
            case FLOAT64: yv[i] = *(double*)pf; break;
            default: yv[i] = 0.0; break;
        }
    }

    /* Determine left/right fill values */
    double left_val  = yv[0];
    double right_val = yv[n - 1];
    if (left != NULL && left->size > 0) {
        switch (left->dtype) {
            case FLOAT64: left_val = *(double*)left->data; break;
            case FLOAT32: left_val = *(float*)left->data; break;
            case INT64:   left_val = *(int64_t*)left->data; break;
            case INT32:   left_val = *(int32_t*)left->data; break;
            default: {
                /* generic extraction using pointer */
                size_t lsz = dtype_size(left->dtype);
                char *ld = (char*)left->data;
                switch (left->dtype) {
                    case INT8:   left_val = *(int8_t*)ld; break;
                    case INT16:  left_val = *(int16_t*)ld; break;
                    case UINT8:  left_val = *(uint8_t*)ld; break;
                    case UINT16: left_val = *(uint16_t*)ld; break;
                    case UINT32: left_val = *(uint32_t*)ld; break;
                    case UINT64: left_val = *(uint64_t*)ld; break;
                    default: left_val = yv[0]; break;
                }
                break;
            }
        }
    }
    if (right != NULL && right->size > 0) {
        switch (right->dtype) {
            case FLOAT64: right_val = *(double*)right->data; break;
            case FLOAT32: right_val = *(float*)right->data; break;
            case INT64:   right_val = *(int64_t*)right->data; break;
            case INT32:   right_val = *(int32_t*)right->data; break;
            default: {
                size_t rsz = dtype_size(right->dtype);
                char *rd = (char*)right->data;
                switch (right->dtype) {
                    case INT8:   right_val = *(int8_t*)rd; break;
                    case INT16:  right_val = *(int16_t*)rd; break;
                    case UINT8:  right_val = *(uint8_t*)rd; break;
                    case UINT16: right_val = *(uint16_t*)rd; break;
                    case UINT32: right_val = *(uint32_t*)rd; break;
                    case UINT64: right_val = *(uint64_t*)rd; break;
                    default: right_val = yv[n-1]; break;
                }
                break;
            }
        }
    }

    Array *result = create_array(arr->shape, arr->ndim, FLOAT64);
    if (!result) { free(xv); free(yv); return NULL; }
    double *rd = (double*)result->data;

    size_t asz = dtype_size(arr->dtype);
    char *adata = (char*)arr->data;

    for (int i = 0; i < arr->size; i++) {
        void *pa = adata + i * asz;
        double xval;
        switch (arr->dtype) {
            case BOOL:   xval = *(uint8_t*)pa; break;
            case INT8:   xval = *(int8_t*)pa; break;
            case INT16:  xval = *(int16_t*)pa; break;
            case INT32:  xval = *(int32_t*)pa; break;
            case INT64:  xval = *(int64_t*)pa; break;
            case UINT8:  xval = *(uint8_t*)pa; break;
            case UINT16: xval = *(uint16_t*)pa; break;
            case UINT32: xval = *(uint32_t*)pa; break;
            case UINT64: xval = *(uint64_t*)pa; break;
            case FLOAT32: xval = *(float*)pa; break;
            case FLOAT64: xval = *(double*)pa; break;
            default: xval = 0.0; break;
        }

        int idx = find_interval(xval, xv, n);
        double y;
        if (idx == -1) {
            y = left_val;
        } else if (idx == -2) {
            y = right_val;
        } else if (idx == n - 1 || xv[idx] == xval) {
            /* Exact match or last segment — use fp[idx] */
            y = yv[idx];
        } else {
            /* Linear interpolation between (xv[idx], yv[idx]) and (xv[idx+1], yv[idx+1]) */
            double t = (xval - xv[idx]) / (xv[idx + 1] - xv[idx]);
            y = yv[idx] + t * (yv[idx + 1] - yv[idx]);
        }
        rd[i] = y;
    }

    free(xv);
    free(yv);
    return result;
}
