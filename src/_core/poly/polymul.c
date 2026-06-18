#include <stdlib.h>
#include <string.h>
#include "array.h"
#include "utils.h"

Array* polymul(Array *a, Array *b) {
    if (!a || !b || a->ndim != 1 || b->ndim != 1) { fprintf(stderr,"polymul: 1D arrays required\n"); return NULL; }
    int na = a->size, nb = b->size;
    int n = na + nb - 1;
    Array *r = create_array((int[]){n}, 1, FLOAT64);
    if (!r) return NULL;
    double *ad = (double*)a->data, *bd = (double*)b->data, *rd = (double*)r->data;
    memset(rd, 0, n * sizeof(double));
    for (int i = 0; i < na; i++)
        for (int j = 0; j < nb; j++)
            rd[i + j] += ad[i] * bd[j];
    return r;
}
