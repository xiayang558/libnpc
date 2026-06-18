#include "array.h"
#include "utils.h"

Array* nancumprod(Array *a, int axis) {
    if (a == NULL) {
        fprintf(stderr, "nancumprod: NULL array argument\n");
        return NULL;
    }
    int ndim = a->ndim;
    if (axis < -1 || axis >= ndim) {
        fprintf(stderr, "nancumprod: invalid axis %d for %dD array\n", axis, ndim);
        return NULL;
    }

    // determine output type: integers promoted to FLOAT64, complex kept as-is
    int is_complex = (a->dtype == COMPLEX64 || a->dtype == COMPLEX128);
    DataType out_dtype = is_complex ? a->dtype : FLOAT64;

    // create result array (same shape as input)
    Array *result = create_array(a->shape, ndim, out_dtype);
    if (!result) return NULL;

    size_t in_elem = dtype_size(a->dtype);
    size_t out_elem = dtype_size(out_dtype);
    char *src = (char*)a->data;
    char *dst = (char*)result->data;

    // handle axis = -1 (flatten)
    if (axis == -1) {
        if (is_complex) {
            complex double prod = 1.0 + 0.0*I;
            for (int i = 0; i < a->size; ++i) {
                void *ptr = src + i * in_elem;
                if (!is_nan_complex(ptr, a->dtype)) {
                    prod *= get_complex_val(ptr, a->dtype);
                }
                void *out_ptr = dst + i * out_elem;
                if (out_dtype == COMPLEX64) {
                    *(complex float*)out_ptr = (complex float)prod;
                } else {
                    *(complex double*)out_ptr = prod;
                }
            }
        } else {
            double prod = 1.0;
            for (int i = 0; i < a->size; ++i) {
                void *ptr = src + i * in_elem;
                if (!is_nan_real(ptr, a->dtype)) {
                    prod *= get_real_val(ptr, a->dtype);
                }
                *(double*)(dst + i * out_elem) = prod;
            }
        }
        return result;
    }

    // along specified axis
    int axis_len = a->shape[axis];
    if (axis_len == 0) return result; // empty axis, return directly

    // stride
    int *strides = malloc(ndim * sizeof(int));
    if (!strides) { free_array(result); return NULL; }
    compute_strides(a->shape, ndim, strides);

    // outer ndim combo
    int outer_ndim = ndim - 1;
    int *outer_dims = malloc(outer_ndim * sizeof(int));
    int *outer_shape = malloc(outer_ndim * sizeof(int));
    int *outer_indices = calloc(outer_ndim, sizeof(int));
    if (!outer_dims || !outer_shape || !outer_indices) {
        free(strides); free(outer_dims); free(outer_shape); free(outer_indices);
        free_array(result);
        return NULL;
    }
    int od = 0;
    for (int i = 0; i < ndim; ++i) if (i != axis) outer_dims[od++] = i;
    for (int i = 0; i < outer_ndim; ++i) outer_shape[i] = a->shape[outer_dims[i]];
    int num_slices = 1;
    for (int i = 0; i < outer_ndim; ++i) num_slices *= outer_shape[i];

    for (int slice = 0; slice < num_slices; ++slice) {
        // compute base offset for current slice in input array
        int base = 0;
        for (int j = 0; j < outer_ndim; ++j) {
            base += outer_indices[j] * strides[outer_dims[j]];
        }
        if (is_complex) {
            complex double prod = 1.0 + 0.0*I;
            for (int k = 0; k < axis_len; ++k) {
                void *ptr = src + (base + k * strides[axis]) * in_elem;
                if (!is_nan_complex(ptr, a->dtype)) {
                    prod *= get_complex_val(ptr, a->dtype);
                }
                void *out_ptr = dst + (base + k * strides[axis]) * out_elem;
                if (out_dtype == COMPLEX64) {
                    *(complex float*)out_ptr = (complex float)prod;
                } else {
                    *(complex double*)out_ptr = prod;
                }
            }
        } else {
            double prod = 1.0;
            for (int k = 0; k < axis_len; ++k) {
                void *ptr = src + (base + k * strides[axis]) * in_elem;
                if (!is_nan_real(ptr, a->dtype)) {
                    prod *= get_real_val(ptr, a->dtype);
                }
                *(double*)(dst + (base + k * strides[axis]) * out_elem) = prod;
            }
        }
        // update outer_indices
        int carry = 1;
        for (int j = outer_ndim - 1; j >= 0 && carry; --j) {
            outer_indices[j]++;
            if (outer_indices[j] >= outer_shape[j]) {
                outer_indices[j] = 0;
                carry = 1;
            } else {
                carry = 0;
            }
        }
    }

    free(strides);
    free(outer_dims);
    free(outer_shape);
    free(outer_indices);
    return result;
}