#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "array.h"
#include "utils.h"

/**
 * true_divide — forced floating-point division (like numpy.true_divide)
 *
 * Differs from divide: integer input → FLOAT64 output, no truncation.
 * Supports broadcasting, division by zero → inf/-inf (IEEE 754)
 */
Array* true_divide(Array *arr1, Array *arr2) {
    if (arr1 == NULL || arr2 == NULL) {
        fprintf(stderr, "true_divide: NULL argument\n");
        return NULL;
    }

    /* Determine output dtype */
    DataType dt1 = arr1->dtype, dt2 = arr2->dtype;
    int cpx1 = (dt1 == COMPLEX64 || dt1 == COMPLEX128);
    int cpx2 = (dt2 == COMPLEX64 || dt2 == COMPLEX128);
    DataType out_dtype;
    if (cpx1 || cpx2) {
        out_dtype = COMPLEX128;
    } else {
        /* All integer/float → FLOAT64 (int→float coercion) */
        out_dtype = FLOAT64;
    }

    /* Broadcast shapes */
    int out_ndim;
    int *out_shape = broadcast_shapes_2(arr1, arr2, &out_ndim);
    if (out_shape == NULL) {
        fprintf(stderr, "true_divide: broadcast failed\n");
        return NULL;
    }

    Array *result;
    if (out_ndim == 0) {
        int one = 1;
        result = create_array(&one, 1, out_dtype);
    } else {
        result = create_array(out_shape, out_ndim, out_dtype);
    }
    free(out_shape);
    if (result == NULL) return NULL;

    /* Compute strides for all arrays */
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

    size_t sz1 = dtype_size(dt1), sz2 = dtype_size(dt2);
    char *d1 = (char*)arr1->data, *d2 = (char*)arr2->data;
    char *res = (char*)result->data;
    size_t out_sz = dtype_size(out_dtype);

    int *indices = NULL;
    if (result->ndim > 0)
        indices = calloc(result->ndim, sizeof(int));

    for (int flat = 0; flat < result->size; flat++) {
        /* Decode result index */
        if (indices) {
            int temp = flat;
            for (int i = result->ndim - 1; i >= 0; i--) {
                indices[i] = temp % result->shape[i];
                temp /= result->shape[i];
            }
        }

        /* Compute source offsets with broadcasting */
        int off1 = 0, off2 = 0;
        if (indices && arr1->ndim > 0) {
            for (int i = 0; i < arr1->ndim; i++) {
                int idx = indices[result->ndim - arr1->ndim + i];
                if (arr1->shape[i] == 1) idx = 0;
                off1 += idx * strides1[i];
            }
        }
        if (indices && arr2->ndim > 0) {
            for (int i = 0; i < arr2->ndim; i++) {
                int idx = indices[result->ndim - arr2->ndim + i];
                if (arr2->shape[i] == 1) idx = 0;
                off2 += idx * strides2[i];
            }
        }

        /* Get values as doubles */
        double v1 = get_value(d1 + off1 * sz1, dt1);
        double v2 = get_value(d2 + off2 * sz2, dt2);

        if (cpx1 || cpx2) {
            /* Complex: read values directly (get_value returns magnitude for complex) */
            complex double z1, z2;
            if (dt1 == COMPLEX64) z1 = *(complex float*)(d1 + off1 * sz1);
            else if (dt1 == COMPLEX128) z1 = *(complex double*)(d1 + off1 * sz1);
            else z1 = get_value(d1 + off1 * sz1, dt1) + 0*I;

            if (dt2 == COMPLEX64) z2 = *(complex float*)(d2 + off2 * sz2);
            else if (dt2 == COMPLEX128) z2 = *(complex double*)(d2 + off2 * sz2);
            else z2 = get_value(d2 + off2 * sz2, dt2) + 0*I;

            complex double q = z1 / z2;
            *(complex double*)(res + flat * out_sz) = q;
        } else {
            *(double*)(res + flat * out_sz) = v1 / v2;
        }
    }

    free(strides1); free(strides2); free(res_strides); free(indices);
    return result;
}
