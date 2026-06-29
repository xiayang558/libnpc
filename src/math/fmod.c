#include "array.h"
#include <math.h>

/**
 * fmod_arr — C-style remainder (toward zero), like numpy.fmod
 *
 * Both inputs are promoted to double, computed with fmod() from math.h.
 * Returns FLOAT64 result array with broadcast shape.
 *
 * @param x1 Dividend array
 * @param x2 Divisor array
 * @return FLOAT64 result array, or NULL on error
 */
Array* fmod_arr(Array *x1, Array *x2) {
    if (x1 == NULL || x2 == NULL) {
        fprintf(stderr, "fmod_arr: NULL array argument\n");
        return NULL;
    }

    /* complex not supported */
    if (x1->dtype == COMPLEX64 || x1->dtype == COMPLEX128 ||
        x1->dtype == COMPLEX256 ||
        x2->dtype == COMPLEX64 || x2->dtype == COMPLEX128 ||
        x2->dtype == COMPLEX256) {
        fprintf(stderr, "fmod_arr: complex numbers not supported\n");
        return NULL;
    }

    /* broadcast shapes */
    int out_ndim;
    Array *arrays[] = {x1, x2};
    int *out_shape = broadcast_shapes(2, arrays, &out_ndim);
    if (out_shape == NULL) {
        fprintf(stderr, "fmod_arr: broadcast failed\n");
        return NULL;
    }

    Array *result;
    if (out_ndim == 0) {
        result = (Array*)malloc(sizeof(Array));
        if (!result) { free(out_shape); return NULL; }
        result->data = malloc(sizeof(double));
        if (!result->data) { free(result); free(out_shape); return NULL; }
        result->dtype = FLOAT64;
        result->shape = NULL;
        result->ndim = 0;
        result->size = 1;
        result->strides = NULL;
        result->is_view = 0;
        *(double*)result->data = 0.0;
    } else {
        result = create_array(out_shape, out_ndim, FLOAT64);
        if (!result) { free(out_shape); return NULL; }
    }
    free(out_shape);

    /* strides */
    int *strides1 = NULL, *strides2 = NULL, *strides_res = NULL;
    if (x1->ndim > 0) { strides1 = malloc(x1->ndim * sizeof(int)); compute_strides(x1->shape, x1->ndim, strides1); }
    if (x2->ndim > 0) { strides2 = malloc(x2->ndim * sizeof(int)); compute_strides(x2->shape, x2->ndim, strides2); }
    if (result->ndim > 0) { strides_res = malloc(result->ndim * sizeof(int)); compute_strides(result->shape, result->ndim, strides_res); }

    size_t sz1 = dtype_size(x1->dtype);
    size_t sz2 = dtype_size(x2->dtype);
    char *data1 = (char*)x1->data;
    char *data2 = (char*)x2->data;
    double *res_data = (double*)result->data;

    int *indices = NULL;
    if (result->ndim > 0) {
        indices = calloc(result->ndim, sizeof(int));
        if (!indices) { free(strides1); free(strides2); free(strides_res); free_array(result); return NULL; }
    }

    for (int flat = 0; flat < result->size; ++flat) {
        if (result->ndim > 0) {
            int tmp = flat;
            for (int i = result->ndim - 1; i >= 0; --i) {
                indices[i] = tmp % result->shape[i];
                tmp /= result->shape[i];
            }
        }

        int off1 = 0, off2 = 0;
        if (x1->ndim > 0 && result->ndim > 0) {
            int dim_offset = result->ndim - x1->ndim;
            for (int i = 0; i < x1->ndim; ++i) {
                int coord = indices[dim_offset + i];
                if (x1->shape[i] == 1) coord = 0;
                off1 += coord * strides1[i];
            }
        }
        if (x2->ndim > 0 && result->ndim > 0) {
            int dim_offset = result->ndim - x2->ndim;
            for (int i = 0; i < x2->ndim; ++i) {
                int coord = indices[dim_offset + i];
                if (x2->shape[i] == 1) coord = 0;
                off2 += coord * strides2[i];
            }
        }

        void *p1 = data1 + off1 * sz1;
        void *p2 = data2 + off2 * sz2;

        double v1 = 0.0, v2 = 0.0;
        switch (x1->dtype) {
            case BOOL:   v1 = *(uint8_t*)p1;  break;
            case INT8:   v1 = *(int8_t*)p1;   break;
            case INT16:  v1 = *(int16_t*)p1;  break;
            case INT32:  v1 = *(int32_t*)p1;  break;
            case INT64:  v1 = *(int64_t*)p1;  break;
            case UINT8:  v1 = *(uint8_t*)p1;  break;
            case UINT16: v1 = *(uint16_t*)p1; break;
            case UINT32: v1 = *(uint32_t*)p1; break;
            case UINT64: v1 = *(uint64_t*)p1; break;
            case FLOAT32: v1 = *(float*)p1;  break;
            case FLOAT64: v1 = *(double*)p1; break;
            default: break;
        }
        switch (x2->dtype) {
            case BOOL:   v2 = *(uint8_t*)p2;  break;
            case INT8:   v2 = *(int8_t*)p2;   break;
            case INT16:  v2 = *(int16_t*)p2;  break;
            case INT32:  v2 = *(int32_t*)p2;  break;
            case INT64:  v2 = *(int64_t*)p2;  break;
            case UINT8:  v2 = *(uint8_t*)p2;  break;
            case UINT16: v2 = *(uint16_t*)p2; break;
            case UINT32: v2 = *(uint32_t*)p2; break;
            case UINT64: v2 = *(uint64_t*)p2; break;
            case FLOAT32: v2 = *(float*)p2;  break;
            case FLOAT64: v2 = *(double*)p2; break;
            default: break;
        }

        double res;
        if (v2 == 0.0) {
            res = NAN;
        } else {
            res = fmod(v1, v2);
        }

        if (result->ndim == 0) {
            res_data[0] = res;
        } else {
            int off_res = 0;
            for (int i = 0; i < result->ndim; ++i)
                off_res += indices[i] * strides_res[i];
            res_data[off_res] = res;
        }
    }

    free(strides1); free(strides2); free(strides_res);
    if (indices) free(indices);
    return result;
}
