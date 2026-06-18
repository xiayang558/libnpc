#include "array.h"

/* ─── value-based partition (partitions data in-place) ─── */

/** Partition a slice of array values so that values[left..right] are rearranged:
 *  the pivot ends up at position `k`, and all elements before are <= pivot,
 *  all after are >= pivot. Uses data directly (byte stride). */
static int value_partition(char *base, size_t stride, int left, int right,
                            int pivot_idx, DataType dtype) {
    double pivot_val = get_value_as_double(base + pivot_idx * stride, dtype);

    /* Swap pivot to the right */
    for (size_t j = 0; j < stride; j++) {
        char tmp = base[pivot_idx * stride + j];
        base[pivot_idx * stride + j] = base[right * stride + j];
        base[right * stride + j] = tmp;
    }

    int i = left;
    for (int j = left; j < right; j++) {
        double cur_val = get_value_as_double(base + j * stride, dtype);
        if (cur_val <= pivot_val) {
            /* swap values[i] and values[j] */
            for (size_t s = 0; s < stride; s++) {
                char tmp = base[i * stride + s];
                base[i * stride + s] = base[j * stride + s];
                base[j * stride + s] = tmp;
            }
            i++;
        }
    }
    /* Swap pivot back */
    for (size_t s = 0; s < stride; s++) {
        char tmp = base[i * stride + s];
        base[i * stride + s] = base[right * stride + s];
        base[right * stride + s] = tmp;
    }
    return i;
}

/** Quickselect on data values — places the k-th smallest at position k */
static void value_quickselect(char *base, size_t stride, int left, int right,
                               int k, DataType dtype) {
    while (left < right) {
        int pivot_idx = left + (right - left) / 2;
        int pos = value_partition(base, stride, left, right, pivot_idx, dtype);
        if (k == pos) return;
        if (k < pos) right = pos - 1;
        else left = pos + 1;
    }
}

/* ─── partition main ─── */

Array* partition(Array *arr, int kth, int axis) {
    if (!check_null(arr, "partition")) return NULL;

    if (arr->dtype == COMPLEX64 || arr->dtype == COMPLEX128 || arr->dtype == COMPLEX256) {
        utils_error_log("Error", "partition", "Complex types not supported");
        return NULL;
    }

    if (axis == -1) axis = arr->ndim - 1;
    if (axis < 0 || axis >= arr->ndim) {
        utils_error_log("Error", "partition", "Invalid axis %d for %dD array", axis, arr->ndim);
        return NULL;
    }

    int axis_len = arr->shape[axis];
    if (kth < 0) kth += axis_len;
    if (kth < 0 || kth >= axis_len) {
        utils_error_log("Error", "partition", "kth %d out of bounds [0,%d)", kth, axis_len);
        return NULL;
    }

    /* Copy input */
    Array *result = copy_array(arr);
    if (!result) return NULL;

    /* Byte strides on result */
    size_t elem_sz = dtype_size(result->dtype);
    size_t *strides = malloc(result->ndim * sizeof(size_t));
    if (!strides) { free_array(result); return NULL; }
    for (int i = 0; i < result->ndim; i++) strides[i] = 0;
    compute_byte_strides(result->shape, result->ndim, elem_sz, strides);

    size_t axis_stride = strides[axis];
    char *data = (char*)result->data;

    /* Outer dimensions (all dims except axis) */
    int outer_ndim = result->ndim - 1;
    if (outer_ndim > 0) {
        int *outer_shape = malloc(outer_ndim * sizeof(int));
        int *outer_indices = calloc(outer_ndim, sizeof(int));
        if (!outer_shape || !outer_indices) {
            free(outer_shape); free(outer_indices); free(strides); free_array(result);
            return NULL;
        }
        int od = 0;
        for (int i = 0; i < result->ndim; i++)
            if (i != axis) outer_shape[od++] = result->shape[i];
        int num_slices = 1;
        for (int i = 0; i < outer_ndim; i++) num_slices *= outer_shape[i];

        for (int s = 0; s < num_slices; s++) {
            size_t off = 0;
            for (int j = 0; j < outer_ndim; j++) {
                int dim = (j < axis) ? j : j + 1;
                off += outer_indices[j] * strides[dim];
            }
            value_quickselect(data + off, axis_stride, 0, axis_len - 1,
                              kth, arr->dtype);

            /* Advance outer indices */
            int carry = 1;
            for (int j = outer_ndim - 1; j >= 0 && carry; j--) {
                outer_indices[j]++;
                if (outer_indices[j] >= outer_shape[j]) {
                    outer_indices[j] = 0; carry = 1;
                } else carry = 0;
            }
        }
        free(outer_shape); free(outer_indices);
    } else {
        /* 1D case: single slice */
        value_quickselect(data, axis_stride, 0, axis_len - 1, kth, arr->dtype);
    }

    free(strides);
    return result;
}
