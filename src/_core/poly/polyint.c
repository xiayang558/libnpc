#include <stdlib.h>
#include <string.h>
#include "array.h"
#include "utils.h"

Array* polyint(Array *p, int m) {
    if (!p || p->ndim != 1) { fprintf(stderr,"polyint: 1D array required\n"); return NULL; }
    if (m < 0) m = 0;
    int n = p->size;
    int out_n = n + m;
    Array *r = create_array((int[]){out_n}, 1, FLOAT64);
    if (!r) return NULL;
    double *pd = (double*)p->data, *rd = (double*)r->data;
    memset(rd, 0, out_n * sizeof(double));
    for (int i = 0; i < n; i++) {
        double divisor = (double)(n - 1 - i + m);
        double factor = 1.0 / divisor;
        for (int d = 1; d < m; d++) factor /= (double)(n - 1 - i + m - d);
        rd[i] = pd[i] * factor;
    }
    return r;
}
