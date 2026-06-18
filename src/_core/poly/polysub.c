#include <stdlib.h>
#include <string.h>
#include "array.h"
#include "utils.h"

Array* polysub(Array *a, Array *b) {
    if (!a || !b || a->ndim != 1 || b->ndim != 1) { fprintf(stderr,"polysub: 1D arrays required\n"); return NULL; }
    int na = a->size, nb = b->size;
    int n = (na > nb) ? na : nb;
    Array *r = create_array((int[]){n}, 1, FLOAT64);
    if (!r) return NULL;
    double *ad = (double*)a->data, *bd = (double*)b->data, *rd = (double*)r->data;
    memset(rd, 0, n * sizeof(double));
    for (int i = 0; i < na; i++) rd[n - na + i] += ad[i];
    for (int i = 0; i < nb; i++) rd[n - nb + i] -= bd[i];
    return r;
}
