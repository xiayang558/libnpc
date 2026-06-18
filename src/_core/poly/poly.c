#include <stdlib.h>
#include <math.h>
#include "array.h"
#include "utils.h"

/**
 * poly  --  compute polynomial coefficients from roots (like numpy.poly)
 *
 * Given roots r_0, r_1, ..., r_{n-1}, return the polynomial
 * (x - r_0)(x - r_1)...(x - r_{n-1}) coefficients.
 * Coefficients are in descending powers (leading coefficient is always 1.0).
 */
Array* poly(Array *roots) {
    if (roots == NULL) {
        fprintf(stderr, "poly: NULL argument\n");
        return NULL;
    }
    if (roots->ndim != 1) {
        fprintf(stderr, "poly: roots must be 1D array\n");
        return NULL;
    }

    int n = roots->size;
    int degree = n;  /* n roots -> degree n polynomial, total n+1 coefficients */

    /* convert to double list */
    double *r = malloc(n * sizeof(double));
    if (!r) return NULL;
    for (int i = 0; i < n; i++) {
        void *ptr = (char*)roots->data + i * dtype_size(roots->dtype);
        r[i] = get_value(ptr, roots->dtype);
    }

    /* coefficient array, from highest to lowest: coeff[0]*x^n + coeff[1]*x^{n-1} + ... + coeff[n] */
    double *coeff = calloc(degree + 1, sizeof(double));
    if (!coeff) { free(r); return NULL; }
    coeff[0] = 1.0;  /* leading coefficient is 1 */

    /* iterate: coeff = conv(coeff, [1, -r_i]) */
    for (int i = 0; i < n; i++) {
        double ri = -r[i];
        /* update from back to front: coeff[j+1] += coeff[j] * ri */
        for (int j = i; j >= 0; j--) {
            coeff[j + 1] += coeff[j] * ri;
        }
    }

    /* output result */
    Array *result = create_array((int[]){degree + 1}, 1, FLOAT64);
    if (!result) { free(r); free(coeff); return NULL; }
    memcpy(result->data, coeff, (degree + 1) * sizeof(double));

    free(r); free(coeff);
    return result;
}
