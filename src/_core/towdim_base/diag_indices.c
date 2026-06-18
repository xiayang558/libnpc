#include "def.h"
#include "array.h"
#include "shape.h"
Array** diag_indices(int n, int ndim, int *num_arrays) {
    *num_arrays = ndim;
    if (n <= 0 || ndim <= 0) return NULL;
    Array **result = malloc(ndim * sizeof(Array*));
    if (!result) return NULL;
    for (int d = 0; d < ndim; d++) {
        result[d] = create_array((int[]){n}, 1, INT64);
        if (!result[d]) { for (int j = 0; j < d; j++) free_array(result[j]); free(result); return NULL; }
        int64_t *data = (int64_t*)result[d]->data;
        for (int i = 0; i < n; i++) data[i] = i;
    }
    return result;
}
