#include <math.h>
#include "array.h"

Array* piecewise(Array *x, Array **condlist, int ncond, pf_func *funclist) {
    if (x == NULL || condlist == NULL || funclist == NULL || ncond <= 0) {
        fprintf(stderr, "piecewise: NULL argument or ncond <= 0\n");
        return NULL;
    }

    int n = x->size;
    Array *result = create_array(x->shape, x->ndim, FLOAT64);
    if (!result) return NULL;

    /* Convert input to double for evaluation */
    Array *x64 = (x->dtype == FLOAT64) ? x : astype(x, FLOAT64);
    if (!x64) { free_array(result); return NULL; }
    double *xd = (double*)x64->data;
    double *rd = (double*)result->data;

    for (int elem = 0; elem < n; elem++) {
        double val = xd[elem];
        double out = 0.0;
        int matched = 0;

        for (int c = 0; c < ncond; c++) {
            if (!condlist[c]) continue;
            uint8_t *cd = (uint8_t*)condlist[c]->data;
            if (cd[elem]) {
                out = funclist[c](val);
                matched = 1;
                /* keep going — last matching condition wins */
            }
        }

        rd[elem] = matched ? out : 0.0;
    }

    if (x64 != x) free_array(x64);
    return result;
}
