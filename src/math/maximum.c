
#include "array.h"
#include "def.h"


// helper: compare two values, return the larger one (by value, not pointer)
static double max_double(double a, double b) {
    if (isnan(a) || isnan(b)) return NAN;
    return (a > b) ? a : b;
}

static float max_float(float a, float b) {
    if (isnan(a) || isnan(b)) return NAN;
    return (a > b) ? a : b;
}

static long double max_long_double(long double a, long double b) {
    if (isnan(a) || isnan(b)) return NAN;
    return (a > b) ? a : b;
}

static int64_t max_int64(int64_t a, int64_t b) { return (a > b) ? a : b; }
static uint64_t max_uint64(uint64_t a, uint64_t b) { return (a > b) ? a : b; }

// complex comparison by magnitude, return complex with larger magnitude (if equal, return first)
static complex double max_complex128(complex double a, complex double b) {
    double ma = cabs(a);
    double mb = cabs(b);
    return (ma >= mb) ? a : b;
}

static complex float max_complex64(complex float a, complex float b) {
    float ma = cabsf(a);
    float mb = cabsf(b);
    return (ma >= mb) ? a : b;
}

static complex long double max_complex256(complex long double a, complex long double b) {
    long double ma = cabsl(a);
    long double mb = cabsl(b);
    return (ma >= mb) ? a : b;
}

// element-wise maximum function, supports broadcast
Array* maximum(Array *x1, Array *x2) {
    if (x1 == NULL || x2 == NULL) {
        fprintf(stderr, "maximum: NULL array argument\n");
        return NULL;
    }
    if (x1->dtype != x2->dtype) {
        // type promotion could be done, simplified here to require same type
        fprintf(stderr, "maximum: data type mismatch\n");
        return NULL;
    }

    // broadcast
    int out_ndim;
    int *out_shape = broadcast_shapes(2, (Array*[]){x1, x2}, &out_ndim);
    if (out_shape == NULL) {
        fprintf(stderr, "maximum: broadcast failed\n");
        return NULL;
    }

    // create result array, same type as input
    Array *result;
    if (out_ndim == 0) {
        result = (Array*)malloc(sizeof(Array));
        if (!result) { free(out_shape); return NULL; }
        result->data = malloc(dtype_size(x1->dtype));
        if (!result->data) { free(result); free(out_shape); return NULL; }
        result->dtype = x1->dtype;
        result->shape = NULL;
        result->ndim = 0;
        result->size = 1;
        result->strides = NULL;
        result->is_view = 0;
    } else {
        result = create_array(out_shape, out_ndim, x1->dtype);
        if (!result) {
            free(out_shape);
            return NULL;
        }
    }
    free(out_shape);

    // Stride computation
    int *strides1 = NULL, *strides2 = NULL, *res_strides = NULL;
    if (x1->ndim) strides1 = malloc(x1->ndim * sizeof(int));
    if (x2->ndim) strides2 = malloc(x2->ndim * sizeof(int));
    if (result->ndim) res_strides = malloc(result->ndim * sizeof(int));
    if (strides1 && x1->ndim) compute_strides(x1->shape, x1->ndim, strides1);
    if (strides2 && x2->ndim) compute_strides(x2->shape, x2->ndim, strides2);
    if (res_strides && result->ndim) compute_strides(result->shape, result->ndim, res_strides);

    size_t elem_sz = dtype_size(x1->dtype);
    char *data1 = (char*)x1->data;
    char *data2 = (char*)x2->data;
    char *res_data = (char*)result->data;

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
        if (x1->ndim && result->ndim) {
            int dim_offset = result->ndim - x1->ndim;
            for (int i = 0; i < x1->ndim; ++i) {
                int coord = indices[dim_offset + i];
                if (x1->shape[i] == 1) coord = 0;
                off1 += coord * strides1[i];
            }
        }
        if (x2->ndim && result->ndim) {
            int dim_offset = result->ndim - x2->ndim;
            for (int i = 0; i < x2->ndim; ++i) {
                int coord = indices[dim_offset + i];
                if (x2->shape[i] == 1) coord = 0;
                off2 += coord * strides2[i];
            }
        }
        void *p1 = data1 + off1 * elem_sz;
        void *p2 = data2 + off2 * elem_sz;
        void *pdst = res_data + flat * elem_sz; // for scalar, flat=0

        switch (x1->dtype) {
            case INT8: {
                int8_t v1 = *(int8_t*)p1, v2 = *(int8_t*)p2;
                int8_t res = (v1 > v2) ? v1 : v2;
                memcpy(pdst, &res, elem_sz);
                break;
            }
            case INT16: {
                int16_t v1 = *(int16_t*)p1, v2 = *(int16_t*)p2;
                int16_t res = (v1 > v2) ? v1 : v2;
                memcpy(pdst, &res, elem_sz);
                break;
            }
            case INT32: {
                int32_t v1 = *(int32_t*)p1, v2 = *(int32_t*)p2;
                int32_t res = (v1 > v2) ? v1 : v2;
                memcpy(pdst, &res, elem_sz);
                break;
            }
            case INT64: {
                int64_t v1 = *(int64_t*)p1, v2 = *(int64_t*)p2;
                int64_t res = (v1 > v2) ? v1 : v2;
                memcpy(pdst, &res, elem_sz);
                break;
            }
            case UINT8: {
                uint8_t v1 = *(uint8_t*)p1, v2 = *(uint8_t*)p2;
                uint8_t res = (v1 > v2) ? v1 : v2;
                memcpy(pdst, &res, elem_sz);
                break;
            }
            case UINT16: {
                uint16_t v1 = *(uint16_t*)p1, v2 = *(uint16_t*)p2;
                uint16_t res = (v1 > v2) ? v1 : v2;
                memcpy(pdst, &res, elem_sz);
                break;
            }
            case UINT32: {
                uint32_t v1 = *(uint32_t*)p1, v2 = *(uint32_t*)p2;
                uint32_t res = (v1 > v2) ? v1 : v2;
                memcpy(pdst, &res, elem_sz);
                break;
            }
            case UINT64: {
                uint64_t v1 = *(uint64_t*)p1, v2 = *(uint64_t*)p2;
                uint64_t res = (v1 > v2) ? v1 : v2;
                memcpy(pdst, &res, elem_sz);
                break;
            }
            case FLOAT32: {
                float v1 = *(float*)p1, v2 = *(float*)p2;
                float res = max_float(v1, v2);
                memcpy(pdst, &res, elem_sz);
                break;
            }
            case FLOAT64: {
                double v1 = *(double*)p1, v2 = *(double*)p2;
                double res = max_double(v1, v2);
                memcpy(pdst, &res, elem_sz);
                break;
            }
            case FLOAT128: {
                long double v1 = *(long double*)p1, v2 = *(long double*)p2;
                long double res = max_long_double(v1, v2);
                memcpy(pdst, &res, elem_sz);
                break;
            }
            case COMPLEX64: {
                complex float v1 = *(complex float*)p1, v2 = *(complex float*)p2;
                complex float res = max_complex64(v1, v2);
                memcpy(pdst, &res, elem_sz);
                break;
            }
            case COMPLEX128: {
                complex double v1 = *(complex double*)p1, v2 = *(complex double*)p2;
                complex double res = max_complex128(v1, v2);
                memcpy(pdst, &res, elem_sz);
                break;
            }
            case COMPLEX256: {
                complex long double v1 = *(complex long double*)p1, v2 = *(complex long double*)p2;
                complex long double res = max_complex256(v1, v2);
                memcpy(pdst, &res, elem_sz);
                break;
            }
            default:
                fprintf(stderr, "maximum: unsupported data type\n");
                free(strides1); free(strides2); free(res_strides);
                if (indices) free(indices);
                free_array(result);
                return NULL;
        }
    }

    free(strides1); free(strides2); free(res_strides);
    if (indices) free(indices);
    return result;
}