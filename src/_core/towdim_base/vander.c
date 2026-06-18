#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "array.h"
#include "utils.h"

/**
 * vander  --  generate Vandermonde matrix (like numpy.vander)
 *
 * The column values depend on the increasing flag:
 *   increasing=0 (default): x[i]^(N-1-j), i.e. descending powers [x^(N-1) ... x^1 x^0]
 *   increasing=1:           x[i]^j,       i.e. ascending powers [x^0 x^1 ... x^(N-1)]
 *
 * @param x          1D Input array
 * @param N          number of columns, defaults to x->size when <= 0
 * @param increasing whether to use ascending powers
 * @return M x N matrix (M = x->size), data type is FLOAT64
 */
Array* vander(Array *x, int N, int increasing) {
    if (x == NULL) {
        fprintf(stderr, "vander: NULL argument\n");
        return NULL;
    }
    if (x->ndim != 1) {
        fprintf(stderr, "vander: input must be 1D array\n");
        return NULL;
    }

    int M = x->size;
    if (N <= 0) N = M;

    Array *result = create_array((int[]){M, N}, 2, FLOAT64);
    if (result == NULL) return NULL;

    double *res = (double*)result->data;

    for (int i = 0; i < M; i++) {
        double xi = get_value((char*)x->data + i * dtype_size(x->dtype), x->dtype);

        /* x^0 = 1 */
        double pow_val = 1.0;

        for (int j = 0; j < N; j++) {
            int col = increasing ? j : (N - 1 - j);
            res[i * N + col] = pow_val;
            pow_val *= xi;
        }
    }

    return result;
}
