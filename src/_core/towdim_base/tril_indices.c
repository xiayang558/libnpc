#include "def.h"
#include "array.h"
#include "shape.h"
#include <string.h>

/**
 * tril_indices  --  Return lower triangle indices (like numpy.tril_indices)
 *
 * @param n number of rows
 * @param k diagonal offset
 * @param m number of columns, -1 means m = n
 * @return 2 x count INT64 array, first row is row indices, second row is column indices
 */
Array* tril_indices(int n, int k, int m) {
    if (m == -1) m = n;
    if (n <= 0 || m <= 0) {
        fprintf(stderr, "tril_indices: n and m must be positive\n");
        return NULL;
    }

    /* Count elements where j <= i + k */
    int count = 0;
    for (int i = 0; i < n; i++) {
        int max_j = i + k;
        if (max_j >= m) max_j = m - 1;
        if (max_j >= 0) count += max_j + 1;
    }

    Array *result = create_array((int[]){2, count}, 2, INT64);
    if (result == NULL) return NULL;

    int64_t *data = (int64_t*)result->data;
    int idx = 0;
    for (int i = 0; i < n; i++) {
        int max_j = i + k;
        if (max_j >= m) max_j = m - 1;
        for (int j = 0; j <= max_j && j < m; j++) {
            data[idx] = i;             /* row index */
            data[count + idx] = j;     /* col index (stored in second row) */
            idx++;
        }
    }

    return result;
}
