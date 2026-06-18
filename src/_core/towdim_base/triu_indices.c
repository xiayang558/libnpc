#include "def.h"
#include "array.h"
#include "shape.h"
#include <string.h>

/**
 * triu_indices  --  Return upper triangle indices (like numpy.triu_indices)
 *
 * Element set: { (i,j) | j >= i + k, 0 <= i < n, 0 <= j < m }
 *
 * @param n number of rows
 * @param k diagonal offset
 * @param m number of columns, -1 means m = n
 * @return 2 x count INT64 array, first row is row indices, second row is column indices
 */
Array* triu_indices(int n, int k, int m) {
    if (m == -1) m = n;
    if (n <= 0 || m <= 0) {
        fprintf(stderr, "triu_indices: n and m must be positive\n");
        return NULL;
    }

    /* Count elements where j >= i + k */
    int count = 0;
    for (int i = 0; i < n; i++) {
        int min_j = i + k;
        if (min_j < 0) min_j = 0;
        if (min_j < m) count += m - min_j;
    }

    Array *result = create_array((int[]){2, count}, 2, INT64);
    if (result == NULL) return NULL;

    int64_t *data = (int64_t*)result->data;
    int idx = 0;
    for (int i = 0; i < n; i++) {
        int min_j = i + k;
        if (min_j < 0) min_j = 0;
        for (int j = min_j; j < m; j++) {
            data[idx] = i;
            data[count + idx] = j;
            idx++;
        }
    }

    return result;
}
