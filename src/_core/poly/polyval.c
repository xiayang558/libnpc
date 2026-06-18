#include <stdlib.h>
#include <string.h>
#include "array.h"
#include "utils.h"

Array* polyval(Array *p, Array *x) {
    if (!p || !x || p->ndim != 1) { fprintf(stderr,"polyval: 1D coeffs, any-shape x required\n"); return NULL; }
    int n = p->size;
    Array *r = create_array(x->shape, x->ndim, FLOAT64);
    if (!r) return NULL;
    double *pd = (double*)p->data, *rd = (double*)r->data;
    double *xd = (double*)malloc(x->size * sizeof(double));
    if (!xd) { free_array(r); return NULL; }
    if (x->dtype == FLOAT64) memcpy(xd, x->data, x->size * sizeof(double));
    else for (int i = 0; i < x->size; i++) xd[i] = get_value((char*)x->data + i*dtype_size(x->dtype), x->dtype);
    for (int i = 0; i < x->size; i++) {
        double v = pd[0];
        for (int j = 1; j < n; j++) v = v * xd[i] + pd[j];
        rd[i] = v;
    }
    free(xd);
    return r;
}
