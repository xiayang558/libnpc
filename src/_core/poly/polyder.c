#include <stdlib.h>
#include <string.h>
#include "array.h"
#include "utils.h"

Array* polyder(Array *p, int m) {
    if (!p || p->ndim != 1) { fprintf(stderr,"polyder: 1D array required\n"); return NULL; }
    if (m < 0) m = 0;
    int n = p->size;
    if (m >= n) {
        Array *r = create_array((int[]){1}, 1, FLOAT64);
        if (r) *(double*)r->data = 0.0;
        return r;
    }
    int out_n = n - m;
    Array *r = create_array((int[]){out_n}, 1, FLOAT64);
    if (!r) return NULL;
    double *pd = (double*)p->data, *rd = (double*)r->data;
    for (int i = 0; i < out_n; i++) {
        double factor = (double)(n - 1 - i);
        for (int d = 1; d < m; d++) factor *= (double)(n - 1 - i - d);
        if (m == 1) factor = (double)(n - 1 - i);
        else {
            factor = 1.0;
            for (int d = 0; d < m; d++) factor *= (double)(n - 1 - i - d);
        }
        rd[i] = pd[i] * factor;
    }
    return r;
}
