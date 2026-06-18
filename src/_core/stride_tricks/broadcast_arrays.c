#include <stdlib.h>
#include <stdio.h>
#include "array.h"

Array** broadcast_arrays(Array **arrays, int num_arrays, int *out_ndim, int **out_shape) {
    if (arrays == NULL || num_arrays <= 0) {
        fprintf(stderr, "broadcast_arrays: Invalid arguments\n");
        return NULL;
    }
    int bcast_ndim = 0;
    int *bcast_shape = broadcast_shapes(num_arrays, arrays, &bcast_ndim);
    if (bcast_shape == NULL) {
        fprintf(stderr, "broadcast_arrays: broadcast_shapes failed\n");
        return NULL;
    }
    Array **out_arrays = malloc(num_arrays * sizeof(Array*));
    if (out_arrays == NULL) {
        free(bcast_shape);
        fprintf(stderr, "broadcast_arrays: Memory allocation failed\n");
        return NULL;
    }
    for (int i = 0; i < num_arrays; i++) {
        out_arrays[i] = broadcast_to(arrays[i], bcast_shape, bcast_ndim);
        if (out_arrays[i] == NULL) {
            for (int j = 0; j < i; j++) free_array(out_arrays[j]);
            free(out_arrays);
            free(bcast_shape);
            return NULL;
        }
    }
    *out_ndim = bcast_ndim;
    *out_shape = bcast_shape;
    return out_arrays;
}
