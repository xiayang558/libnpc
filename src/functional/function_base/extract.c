#include <stdlib.h>
#include <string.h>
#include "array.h"
#include "utils.h"

/**
 * extract -- extract elements satisfying condition (like numpy.extract)
 *
 * Flatten arr, return elements where condition is True. 
 *
 * @param condition Boolean array (must be same shape as arr)
 * @param arr       Input array
 * @return 1D array, length = true_count
 */
Array* extract(Array *condition, Array *arr) {
    if (condition == NULL || arr == NULL) {
        fprintf(stderr, "extract: NULL argument\n"); return NULL;
    }
    if (condition->size != arr->size) {
        fprintf(stderr, "extract: condition size %d != arr size %d\n",
                condition->size, arr->size);
        return NULL;
    }

    /* Count true elements */
    size_t cesz = dtype_size(condition->dtype);
    char *cdata = (char*)condition->data;
    int *true_idx = malloc(condition->size * sizeof(int));
    if (!true_idx) return NULL;
    int ntrue = 0;
    for (int i = 0; i < condition->size; i++) {
        if (is_nonzero(cdata + i * cesz, condition->dtype))
            true_idx[ntrue++] = i;
    }

    /* Create output: 1D, same dtype as arr */
    Array *result = create_array((int[]){ntrue}, 1, arr->dtype);
    if (!result) { free(true_idx); return NULL; }

    size_t esz = dtype_size(arr->dtype);
    char *adata = (char*)arr->data;
    char *rd = (char*)result->data;

    for (int i = 0; i < ntrue; i++)
        memcpy(rd + i * esz, adata + true_idx[i] * esz, esz);

    free(true_idx);
    return result;
}
