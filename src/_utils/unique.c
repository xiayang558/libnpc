#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "array.h"
#include "utils.h"
#include "shape.h"

static int cmp_double_qsort(const void *a, const void *b) {
    double da = *(const double*)a;
    double db = *(const double*)b;
    if (da < db) return -1;
    if (da > db) return 1;
    return 0;
}

/**
 * unique  --  Return array's unique elements (like numpy.unique)
 *
 * @param arr             Input array (auto flattened)
 * @param return_index    whether to return first-occurrence indices
 * @param return_inverse  whether to return inverse indices
 * @param return_counts   whether to return occurrence counts
 * @return UniqueResult, caller must free each array
 */
UniqueResult unique(Array *arr, int return_index, int return_inverse, int return_counts) {
    UniqueResult res = {NULL, NULL, NULL, NULL};
    if (arr == NULL) {
        fprintf(stderr, "unique: NULL argument\n");
        return res;
    }

    /* Flatten and convert to FLOAT64 for comparison */
    Array *flat = flatten(arr);
    if (flat == NULL) return res;
    Array *dbl = astype(flat, FLOAT64);
    free_array(flat);
    if (dbl == NULL) return res;

    int n = dbl->size;
    if (n == 0) {
        res.values = create_array((int[]){0}, 1, FLOAT64);
        free_array(dbl);
        return res;
    }

    double *data = (double*)dbl->data;

    /* If we need indices, save original positions */
    int *orig_pos = NULL;
    if (return_index || return_inverse) {
        orig_pos = malloc(n * sizeof(int));
        for (int i = 0; i < n; i++) orig_pos[i] = i;
    }

    /* Sort data (and position array alongside) */
    if (orig_pos) {
        /* Simple bubble-sort style with positions */
        for (int i = 0; i < n - 1; i++) {
            for (int j = i + 1; j < n; j++) {
                if (data[i] > data[j] || (data[i] == data[j] && orig_pos[i] > orig_pos[j])) {
                    double td = data[i]; data[i] = data[j]; data[j] = td;
                    int tp = orig_pos[i]; orig_pos[i] = orig_pos[j]; orig_pos[j] = tp;
                }
            }
        }
    } else {
        qsort(data, n, sizeof(double), cmp_double_qsort);
    }

    /* Count unique values */
    int uniq_count = 0;
    for (int i = 0; i < n; i++) {
        if (i == 0 || data[i] != data[i - 1])
            uniq_count++;
    }

    /* Build unique values array */
    int u_shape[1] = {uniq_count};
    res.values = create_array(u_shape, 1, FLOAT64);
    if (res.values == NULL) { free(dbl); free(orig_pos); return res; }
    double *uvals = (double*)res.values->data;

    /* Build extra arrays */
    int64_t *idx_data = NULL, *inv_data = NULL, *cnt_data = NULL;
    if (return_index) {
        res.indices = create_array(u_shape, 1, INT64);
        if (!res.indices) goto error;
        idx_data = (int64_t*)res.indices->data;
    }
    if (return_inverse) {
        int inv_shape[1] = {n};
        res.inverse = create_array(inv_shape, 1, INT64);
        if (!res.inverse) goto error;
        inv_data = (int64_t*)res.inverse->data;
    }
    if (return_counts) {
        res.counts = create_array(u_shape, 1, INT64);
        if (!res.counts) goto error;
        cnt_data = (int64_t*)res.counts->data;
    }

    /* Populate: iterate sorted data, group by unique value */
    int ui = 0;
    for (int i = 0; i < n; i++) {
        if (i == 0 || data[i] != data[i - 1]) {
            /* New unique value */
            uvals[ui] = data[i];
            if (idx_data) idx_data[ui] = orig_pos ? orig_pos[i] : i;
            if (cnt_data) cnt_data[ui] = 1;
            ui++;
        } else {
            /* Same value: update count */
            if (cnt_data) cnt_data[ui - 1]++;
        }
        /* Inverse: map original position → unique index */
        if (inv_data && orig_pos) inv_data[orig_pos[i]] = ui - 1;
    }

    free(orig_pos);
    free_array(dbl);
    return res;

error:
    if (res.values)  free_array(res.values);
    if (res.indices) free_array(res.indices);
    if (res.inverse) free_array(res.inverse);
    if (res.counts)  free_array(res.counts);
    res.values = NULL; res.indices = NULL; res.inverse = NULL; res.counts = NULL;
    free(orig_pos);
    free_array(dbl);
    return res;
}
