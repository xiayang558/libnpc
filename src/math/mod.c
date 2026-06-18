#include <math.h>
#include "array.h"

// helper: floating point modulo, result sign same as divisor (consistent with NumPy)
static double fmod_same_sign(double a, double b) {
    double r = fmod(a, b);
    if (r != 0.0 && ((r > 0.0) != (b > 0.0))) r += b;
    return r;
}

static float fmodf_same_sign(float a, float b) {
    float r = fmodf(a, b);
    if (r != 0.0f && ((r > 0.0f) != (b > 0.0f))) r += b;
    return r;
}

static long double fmodl_same_sign(long double a, long double b) {
    long double r = fmodl(a, b);
    if (r != 0.0L && ((r > 0.0L) != (b > 0.0L))) r += b;
    return r;
}

// helper: integer modulo, result sign same as divisor
static int64_t int_mod(int64_t a, int64_t b) {
    if (b == 0) return 0;          // divide-by-zero protection, return 0
    int64_t r = a % b;
    if (r != 0 && ((r > 0) != (b > 0))) r += b;
    return r;
}

Array* mod(Array *arr1, Array *arr2) {
    if (arr1 == NULL || arr2 == NULL) {
        fprintf(stderr, "mod: NULL array argument\n");
        return NULL;
    }
    // complex not supported
    if (arr1->dtype == COMPLEX64 || arr1->dtype == COMPLEX128 ||
        arr2->dtype == COMPLEX64 || arr2->dtype == COMPLEX128) {
        fprintf(stderr, "mod: complex numbers not supported\n");
        return NULL;
    }

    // broadcast
    int out_ndim;
    int *out_shape = broadcast_shapes(2, (Array*[]){arr1, arr2}, &out_ndim);
    if (out_shape == NULL) {
        fprintf(stderr, "mod: broadcast failed\n");
        return NULL;
    }

    // result type is uniformly FLOAT64
    DataType out_dtype = FLOAT64;
    Array *result;
    if (out_ndim == 0) {
        result = (Array*)malloc(sizeof(Array));
        if (!result) { free(out_shape); return NULL; }
        result->data = malloc(sizeof(double));
        if (!result->data) { free(result); free(out_shape); return NULL; }
        result->dtype = out_dtype;
        result->shape = NULL;
        result->ndim = 0;
        result->size = 1;
        result->strides = NULL;
        result->is_view = 0;
        *(double*)result->data = 0.0;
    } else {
        result = create_array(out_shape, out_ndim, out_dtype);
        if (!result) {
            free(out_shape);
            return NULL;
        }
    }
    free(out_shape);

    // Stride computation
    int *strides1 = NULL, *strides2 = NULL, *res_strides = NULL;
    if (arr1->ndim) strides1 = malloc(arr1->ndim * sizeof(int));
    if (arr2->ndim) strides2 = malloc(arr2->ndim * sizeof(int));
    if (result->ndim) res_strides = malloc(result->ndim * sizeof(int));
    if (strides1 && arr1->ndim) compute_strides(arr1->shape, arr1->ndim, strides1);
    if (strides2 && arr2->ndim) compute_strides(arr2->shape, arr2->ndim, strides2);
    if (res_strides && result->ndim) compute_strides(result->shape, result->ndim, res_strides);

    size_t sz1 = dtype_size(arr1->dtype);
    size_t sz2 = dtype_size(arr2->dtype);
    char *data1 = (char*)arr1->data;
    char *data2 = (char*)arr2->data;
    double *res_data = (double*)result->data;

    int *indices = result->ndim ? calloc(result->ndim, sizeof(int)) : NULL;
    if (result->ndim && !indices) {
        free(strides1); free(strides2); free(res_strides);
        free_array(result);
        return NULL;
    }

    int total = result->size;
    for (int flat = 0; flat < total; ++flat) {
        if (result->ndim) {
            int tmp = flat;
            for (int i = result->ndim - 1; i >= 0; --i) {
                indices[i] = tmp % result->shape[i];
                tmp /= result->shape[i];
            }
        }
        // offset
        int off1 = 0, off2 = 0;
        if (arr1->ndim && result->ndim) {
            int dim_offset = result->ndim - arr1->ndim;
            for (int i = 0; i < arr1->ndim; ++i) {
                int coord = indices[dim_offset + i];
                if (arr1->shape[i] == 1) coord = 0;
                off1 += coord * strides1[i];
            }
        }
        if (arr2->ndim && result->ndim) {
            int dim_offset = result->ndim - arr2->ndim;
            for (int i = 0; i < arr2->ndim; ++i) {
                int coord = indices[dim_offset + i];
                if (arr2->shape[i] == 1) coord = 0;
                off2 += coord * strides2[i];
            }
        }
        void *p1 = data1 + off1 * sz1;
        void *p2 = data2 + off2 * sz2;

        // extract dividend and divisor
        double v1 = 0.0, v2 = 0.0;
        switch (arr1->dtype) {
            case INT8:   v1 = *(int8_t*)p1; break;
            case INT16:  v1 = *(int16_t*)p1; break;
            case INT32:  v1 = *(int32_t*)p1; break;
            case INT64:  v1 = *(int64_t*)p1; break;
            case UINT8:  v1 = *(uint8_t*)p1; break;
            case UINT16: v1 = *(uint16_t*)p1; break;
            case UINT32: v1 = *(uint32_t*)p1; break;
            case UINT64: v1 = *(uint64_t*)p1; break;
            case FLOAT32: v1 = *(float*)p1; break;
            case FLOAT64: v1 = *(double*)p1; break;
            default: break;
        }
        switch (arr2->dtype) {
            case INT8:   v2 = *(int8_t*)p2; break;
            case INT16:  v2 = *(int16_t*)p2; break;
            case INT32:  v2 = *(int32_t*)p2; break;
            case INT64:  v2 = *(int64_t*)p2; break;
            case UINT8:  v2 = *(uint8_t*)p2; break;
            case UINT16: v2 = *(uint16_t*)p2; break;
            case UINT32: v2 = *(uint32_t*)p2; break;
            case UINT64: v2 = *(uint64_t*)p2; break;
            case FLOAT32: v2 = *(float*)p2; break;
            case FLOAT64: v2 = *(double*)p2; break;
            default: break;
        }

        double res;
        if (v2 == 0.0) {
            res = 0.0;   // divide-by-zero protection
        } else if (arr1->dtype == FLOAT32 || arr1->dtype == FLOAT64 ||
                   arr2->dtype == FLOAT32 || arr2->dtype == FLOAT64) {
            // floating point modulo
            if (arr1->dtype == FLOAT32 && arr2->dtype == FLOAT32) {
                res = fmodf_same_sign((float)v1, (float)v2);
            } else {
                res = fmod_same_sign(v1, v2);
            }
        } else {
            // integer modulo
            int64_t a = (int64_t)v1, b = (int64_t)v2;
            res = (double)int_mod(a, b);
        }

        if (result->ndim == 0) {
            res_data[0] = res;
        } else {
            int off_res = 0;
            for (int i = 0; i < result->ndim; ++i) off_res += indices[i] * res_strides[i];
            res_data[off_res] = res;
        }
    }

    free(strides1); free(strides2); free(res_strides);
    if (indices) free(indices);
    return result;
}