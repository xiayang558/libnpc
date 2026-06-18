#include <stdlib.h>
#include <string.h>
#include "array.h"

int meshgrid(Array **arrays, int n, const char *indexing, Array ***out) {
    if (!arrays || n < 1 || !out) return -1;

    int xy_mode = !(indexing && indexing[0] == 'i' && indexing[1] == 'j');

    int *lens = malloc(n * sizeof(int));
    if (!lens) return -1;
    for (int i = 0; i < n; i++) {
        if (!arrays[i] || arrays[i]->ndim != 1) { free(lens); return -1; }
        lens[i] = arrays[i]->size;
    }

    /* Output shape: for xy, swap first two lengths */
    int *out_shape = malloc(n * sizeof(int));
    if (!out_shape) { free(lens); return -1; }
    if (xy_mode && n >= 2) {
        out_shape[0] = lens[1]; out_shape[1] = lens[0];
        for (int i = 2; i < n; i++) out_shape[i] = lens[i];
    } else {
        for (int i = 0; i < n; i++) out_shape[i] = lens[i];
    }

    Array **results = malloc(n * sizeof(Array*));
    if (!results) { free(lens); free(out_shape); return -1; }

    for (int axis = 0; axis < n; axis++) {
        results[axis] = create_array(out_shape, n, arrays[axis]->dtype);
        if (!results[axis]) {
            for (int j = 0; j < axis; j++) free_array(results[j]);
            free(results); free(lens); free(out_shape); return -1;
        }
    }

    /* Fill each grid */
    for (int axis = 0; axis < n; axis++) {
        int src_idx = axis;
        int fill_axis = axis;
        if (xy_mode && n >= 2 && axis < 2) {
            fill_axis = 1 - axis;  /* swap which output axis data varies along */
        }

        size_t esz = dtype_size(arrays[src_idx]->dtype);
        char *grid_data = (char*)results[axis]->data;
        char *src_data = (char*)arrays[src_idx]->data;

        int total = results[axis]->size;
        int *indices = calloc(n, sizeof(int));
        for (int flat = 0; flat < total; flat++) {
            int rem = flat;
            for (int d = n - 1; d >= 0; d--) {
                indices[d] = rem % out_shape[d];
                rem /= out_shape[d];
            }
            int src_i = indices[fill_axis];
            memcpy(grid_data + flat * esz, src_data + src_i * esz, esz);
        }
        free(indices);
    }

    *out = results;
    free(lens);
    free(out_shape);
    return 0;
}
