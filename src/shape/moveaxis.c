#include <stdlib.h>
#include <string.h>
#include "array.h"

Array* moveaxis(Array *a, int source, int destination) {
    if (a == NULL) {
        fprintf(stderr, "moveaxis: NULL array argument\n");
        return NULL;
    }
    int ndim = a->ndim;
    if (ndim == 0) {
        return copy_array(a);
    }
    // handle negative axis indices
    if (source < 0) source += ndim;
    if (destination < 0) destination += ndim;
    if (source < 0 || source >= ndim || destination < 0 || destination >= ndim) {
        fprintf(stderr, "moveaxis: axis out of range\n");
        return NULL;
    }
    if (source == destination) {
        return copy_array(a);
    }

    // build new shape: remove source axis, then insert at destination position
    int *new_shape = malloc(ndim * sizeof(int));
    if (!new_shape) return NULL;

    // first copy all axis dims except source
    int idx = 0;
    for (int i = 0; i < ndim; ++i) {
        if (i != source) new_shape[idx++] = a->shape[i];
    }
    // insert source axis dim at destination position
    int source_dim = a->shape[source];
    // shift elements backward starting from destination
    for (int i = ndim - 1; i > destination; --i) {
        new_shape[i] = new_shape[i-1];
    }
    new_shape[destination] = source_dim;

    // create new array
    Array *result = create_array(new_shape, ndim, a->dtype);
    free(new_shape);
    if (!result) return NULL;

    // Stride computation
    int *a_strides = malloc(ndim * sizeof(int));
    int *res_strides = malloc(ndim * sizeof(int));
    if (!a_strides || !res_strides) {
        free(a_strides); free(res_strides);
        free_array(result);
        return NULL;
    }
    compute_strides(a->shape, ndim, a_strides);
    compute_strides(result->shape, ndim, res_strides);

    size_t elem_sz = dtype_size(a->dtype);
    char *a_data = (char*)a->data;
    char *res_data = (char*)result->data;

    int *indices = malloc(ndim * sizeof(int));
    if (!indices) {
        free(a_strides); free(res_strides);
        free_array(result);
        return NULL;
    }

    int total = result->size;
    for (int flat = 0; flat < total; ++flat) {
        // decode new array indices
        int tmp = flat;
        for (int i = ndim - 1; i >= 0; --i) {
            indices[i] = tmp % result->shape[i];
            tmp /= result->shape[i];
        }
        // map to original array indices
        int a_idx[ndim];
        int idx_pos = 0;
        // for axes other than source, take indices in original order from the first ndim-1
        // positions of the new array (note destination position is equivalent to source)
        for (int i = 0; i < ndim; ++i) {
            if (i == source) continue;
            a_idx[i] = indices[idx_pos++];
        }
        a_idx[source] = indices[destination];

        // compute original array offset
        int a_off = 0;
        for (int i = 0; i < ndim; ++i) {
            a_off += a_idx[i] * a_strides[i];
        }
        // compute new array offset
        int res_off = 0;
        for (int i = 0; i < ndim; ++i) {
            res_off += indices[i] * res_strides[i];
        }
        memcpy(res_data + res_off * elem_sz, a_data + a_off * elem_sz, elem_sz);
    }

    free(a_strides);
    free(res_strides);
    free(indices);
    return result;
}