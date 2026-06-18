#include <stdlib.h>
#include <string.h>
#include "array.h"
#include "function.h"
#include "utils.h"

/**
 * searchsorted -- binary search insertion point (like numpy.searchsorted)
 *
 * @param a     alreadysort 1D array
 * @param v     Values to find insertion points for (any shape)
 * @param side  "left" or "right"
 * @return INT64 array, same shape as v
 */
Array* searchsorted(Array *a, Array *v, const char *side) {
    if (a == NULL || v == NULL) {
        fprintf(stderr, "searchsorted: NULL argument\n"); return NULL;
    }
    if (a->ndim != 1) {
        fprintf(stderr, "searchsorted: a must be 1D\n"); return NULL;
    }

    int right = (side && side[0] == 'r');

    /* Convert a to double for find_interval */
    int n = a->size;
    double *sorted = malloc(n * sizeof(double));
    if (!sorted) return NULL;
    size_t esz = dtype_size(a->dtype);
    char *adata = (char*)a->data;
    for (int i = 0; i < n; i++)
        sorted[i] = get_value(adata + i * esz, a->dtype);

    Array *result = create_array(v->shape, v->ndim, INT64);
    if (!result) { free(sorted); return NULL; }

    int64_t *rd = (int64_t*)result->data;
    size_t vesz = dtype_size(v->dtype);
    char *vdata = (char*)v->data;

    for (int i = 0; i < v->size; i++) {
        double val = get_value(vdata + i * vesz, v->dtype);
        int lo = 0, hi = n;
        if (right) {
            while (lo < hi) {
                int mid = (lo + hi) / 2;
                if (val < sorted[mid]) hi = mid;
                else lo = mid + 1;
            }
        } else {
            while (lo < hi) {
                int mid = (lo + hi) / 2;
                if (val <= sorted[mid]) hi = mid;
                else lo = mid + 1;
            }
        }
        rd[i] = lo;
    }

    free(sorted);
    return result;
}
