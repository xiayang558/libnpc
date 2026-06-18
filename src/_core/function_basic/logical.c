#include "array.h"


// helper function: convert any numeric value to boolean (0 or 1)
static uint8_t to_bool(void *ptr, DataType dtype) {
    switch (dtype) {
        case BOOL:   return *(uint8_t*)ptr;
        case INT8:   return *(int8_t*)ptr != 0;
        case INT16:  return *(int16_t*)ptr != 0;
        case INT32:  return *(int32_t*)ptr != 0;
        case INT64:  return *(int64_t*)ptr != 0;
        case UINT8:  return *(uint8_t*)ptr != 0;
        case UINT16: return *(uint16_t*)ptr != 0;
        case UINT32: return *(uint32_t*)ptr != 0;
        case UINT64: return *(uint64_t*)ptr != 0;
        case FLOAT32: return fabsf(*(float*)ptr) > 0.0f;
        case FLOAT64: return fabs(*(double*)ptr) > 0.0;
        case FLOAT128: return fabsl(*(long double*)ptr) > 0.0L;
        case COMPLEX64: {
            complex float z = *(complex float*)ptr;
            return crealf(z) != 0.0f || cimagf(z) != 0.0f;
        }
        case COMPLEX128: {
            complex double z = *(complex double*)ptr;
            return creal(z) != 0.0 || cimag(z) != 0.0;
        }
        default: return 0;
    }
}

// Generic binary logical operation
static Array* binary_logical_op(Array *arr1, Array *arr2, uint8_t (*op)(uint8_t, uint8_t), const char *name) {
    if (!arr1 || !arr2) {
        fprintf(stderr, "%s: NULL argument\n", name);
        return NULL;
    }
    // broadcast
    int out_ndim;
    int *out_shape = broadcast_shapes(2, (Array*[]){arr1, arr2}, &out_ndim);
    if (!out_shape) {
        fprintf(stderr, "%s: broadcast failed\n", name);
        return NULL;
    }
    // create resultboolean array
    Array *result;
    if (out_ndim == 0) {
        result = (Array*)malloc(sizeof(Array));
        if (!result) { free(out_shape); return NULL; }
        result->data = malloc(sizeof(uint8_t));
        if (!result->data) { free(result); free(out_shape); return NULL; }
        result->dtype = BOOL;
        result->shape = NULL;
        result->ndim = 0;
        result->size = 1;
        result->strides = NULL;
        result->is_view = 0;
    } else {
        result = create_array(out_shape, out_ndim, BOOL);
        if (!result) {
            free(out_shape);
            return NULL;
        }
    }
    free(out_shape);

    // stride
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
    uint8_t *res_data = (uint8_t*)result->data;

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
        uint8_t b1 = to_bool(p1, arr1->dtype);
        uint8_t b2 = to_bool(p2, arr2->dtype);
        uint8_t r = op(b1, b2);
        if (result->ndim == 0) {
            res_data[0] = r;
        } else {
            int off_res = 0;
            for (int i = 0; i < result->ndim; ++i) off_res += indices[i] * res_strides[i];
            res_data[off_res] = r;
        }
    }
    free(strides1); free(strides2); free(res_strides);
    if (indices) free(indices);
    return result;
}

static uint8_t and_op(uint8_t arr, uint8_t b) { return arr && b; }
static uint8_t or_op(uint8_t arr, uint8_t b)  { return arr || b; }
static uint8_t xor_op(uint8_t arr, uint8_t b) { return arr ^ b; }

Array* logical_and(Array *arr1, Array *arr2) { return binary_logical_op(arr1, arr2, and_op, "logical_and"); }
Array* logical_or(Array *arr1, Array *arr2)  { return binary_logical_op(arr1, arr2, or_op, "logical_or"); }
Array* logical_xor(Array *arr1, Array *arr2) { return binary_logical_op(arr1, arr2, xor_op, "logical_xor"); }

Array* logical_not(Array *arr) {
    if (!arr) {
        fprintf(stderr, "logical_not: NULL argument\n");
        return NULL;
    }
    Array *result = create_array(arr->shape, arr->ndim, BOOL);
    if (!result) return NULL;
    size_t elem_sz = dtype_size(arr->dtype);
    char *src = (char*)arr->data;
    uint8_t *dst = (uint8_t*)result->data;
    for (int i = 0; i < arr->size; ++i) {
        dst[i] = !to_bool(src + i * elem_sz, arr->dtype);
    }
    return result;
}