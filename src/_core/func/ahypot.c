#include "array.h"


Array* ahypot(Array *arr1, Array *arr2) {
    if (arr1 == NULL || arr2 == NULL) {
        fprintf(stderr, "hypot: NULL array argument\n");
        return NULL;
    }
    // real numbers only: complex not supported (NumPy also doesn't support it)
    if (arr1->dtype == COMPLEX64 || arr1->dtype == COMPLEX128 ||
        arr2->dtype == COMPLEX64 || arr2->dtype == COMPLEX128) {
        fprintf(stderr, "hypot: complex numbers not supported\n");
        return NULL;
    }

    /* Compute shape after broadcast */
    int out_ndim;
    int *out_shape = broadcast_shapes_2(arr1, arr2, &out_ndim);
    if (out_shape == NULL) {
        fprintf(stderr, "hypot: broadcast failed\n");
        return NULL;
    }

    // result type is always FLOAT64
    Array *result = create_array(out_shape, out_ndim, FLOAT64);
    free(out_shape);
    if (result == NULL) return NULL;

    // compute stride
    int *strides1 = NULL, *strides2 = NULL, *res_strides = NULL;
    if (arr1->ndim > 0) {
        strides1 = malloc(arr1->ndim * sizeof(int));
        compute_strides(arr1->shape, arr1->ndim, strides1);
    }
    if (arr2->ndim > 0) {
        strides2 = malloc(arr2->ndim * sizeof(int));
        compute_strides(arr2->shape, arr2->ndim, strides2);
    }
    if (result->ndim > 0) {
        res_strides = malloc(result->ndim * sizeof(int));
        compute_strides(result->shape, result->ndim, res_strides);
    }

    size_t sz1 = dtype_size(arr1->dtype);
    size_t sz2 = dtype_size(arr2->dtype);
    char *data1 = (char*)arr1->data;
    char *data2 = (char*)arr2->data;
    double *res_data = (double*)result->data;

    int *indices = calloc(result->ndim, sizeof(int));
    if (!indices) {
        free(strides1); free(strides2); free(res_strides);
        free_array(result);
        return NULL;
    }

    // traverseoutput arrayper element
    for (int flat = 0; flat < result->size; flat++) {
        // convert flat index to multi-dimensional indices
        int tmp = flat;
        for (int i = result->ndim - 1; i >= 0; i--) {
            indices[i] = tmp % result->shape[i];
            tmp /= result->shape[i];
        }
        // get both input values
        int off1 = get_broadcast_index(indices, result->ndim, arr1, strides1);
        int off2 = get_broadcast_index(indices, result->ndim, arr2, strides2);
        void *p1 = data1 + off1 * sz1;
        void *p2 = data2 + off2 * sz2;

        double v1 = 0.0, v2 = 0.0;
        // extract numeric values (uniformly converted to double)
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
        res_data[flat] = hypot(v1, v2);
    }

    free(indices);
    free(strides1);
    free(strides2);
    free(res_strides);
    return result;
}