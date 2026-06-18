#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>
#include <stdint.h>
#include "array.h"
Array* greater_equal(Array *arr1, Array *arr2) {
    if (arr1 == NULL || arr2 == NULL) {
        fprintf(stderr, "greater_equal: NULL array argument\n");
        return NULL;
    }

    // type check: require same type (could be expanded to type promotion)
    if (arr1->dtype != arr2->dtype) {
        fprintf(stderr, "greater_equal: data types must be the same\n");
        return NULL;
    }

    // Compute shape after broadcast
    int out_ndim;
    int *out_shape = broadcast_shapes_2(arr1, arr2, &out_ndim);
    if (out_shape == NULL) {
        fprintf(stderr, "greater_equal: broadcast failed\n");
        return NULL;
    }

    // create result arrays (BOOL type)
    Array *result = create_array(out_shape, out_ndim, BOOL);
    free(out_shape);
    if (result == NULL) {
        fprintf(stderr, "greater_equal: failed to create result array\n");
        return NULL;
    }

    // compute strides (in units of elements)
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

    size_t elem_sz = dtype_size(arr1->dtype);
    char *data1 = (char*)arr1->data;
    char *data2 = (char*)arr2->data;
    uint8_t *res_data = (uint8_t*)result->data;

    int *indices = calloc(result->ndim, sizeof(int));
    if (indices == NULL) {
        free(strides1); free(strides2); free(res_strides);
        free_array(result);
        return NULL;
    }

    // traverse the result array element by element
    for (int flat = 0; flat < result->size; flat++) {
        // convert flat index to multi-dimensional index
        int tmp = flat;
        for (int i = result->ndim - 1; i >= 0; i--) {
            indices[i] = tmp % result->shape[i];
            tmp /= result->shape[i];
        }
        // get the corresponding offset in the source arrays
        int off1 = get_broadcast_index(indices, result->ndim, arr1, strides1);
        int off2 = get_broadcast_index(indices, result->ndim, arr2, strides2);
        void *p1 = data1 + off1 * elem_sz;
        void *p2 = data2 + off2 * elem_sz;

        int cmp = 0;
        switch (arr1->dtype) {
            case INT8:   cmp = *(int8_t*)p1 >= *(int8_t*)p2; break;
            case INT16:  cmp = *(int16_t*)p1 >= *(int16_t*)p2; break;
            case INT32:  cmp = *(int32_t*)p1 >= *(int32_t*)p2; break;
            case INT64:  cmp = *(int64_t*)p1 >= *(int64_t*)p2; break;
            case UINT8:  cmp = *(uint8_t*)p1 >= *(uint8_t*)p2; break;
            case UINT16: cmp = *(uint16_t*)p1 >= *(uint16_t*)p2; break;
            case UINT32: cmp = *(uint32_t*)p1 >= *(uint32_t*)p2; break;
            case UINT64: cmp = *(uint64_t*)p1 >= *(uint64_t*)p2; break;
            case FLOAT32: cmp = *(float*)p1 >= *(float*)p2; break;
            case FLOAT64: cmp = *(double*)p1 >= *(double*)p2; break;
            case COMPLEX64:
                // NumPy would raise TypeError for complex comparison, here we simplify by comparing magnitudes
                cmp = cabsf(*(complex float*)p1) >= cabsf(*(complex float*)p2);
                break;
            case COMPLEX128:
                cmp = cabs(*(complex double*)p1) >= cabs(*(complex double*)p2);
                break;
            default:
                fprintf(stderr, "greater_equal: unsupported data type\n");
                free(indices); free(strides1); free(strides2); free(res_strides);
                free_array(result);
                return NULL;
        }
        res_data[flat] = cmp ? 1 : 0;
    }

    free(indices);
    free(strides1);
    free(strides2);
    free(res_strides);
    return result;
}
