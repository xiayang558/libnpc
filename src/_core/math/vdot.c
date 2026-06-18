#include <math.h>
#include <complex.h>
#include <stdlib.h>
#include <string.h>
#include "array.h"
#include "utils.h"

/**
 * vdot  --  conjugate dot product after flattening two arrays (like numpy.vdot)
 *
 * Differences from dot:
 *   - both arrays are flattened to 1D
 *   - for complex, takes the conjugate of the first argument
 *   - returns a scalar
 */
Array* vdot(Array *arr1, Array *arr2) {
    if (arr1 == NULL || arr2 == NULL) {
        fprintf(stderr, "vdot: NULL argument\n");
        return NULL;
    }

    if (arr1->size != arr2->size) {
        fprintf(stderr, "vdot: arrays must have the same size (%d vs %d)\n",
                arr1->size, arr2->size);
        return NULL;
    }

    DataType dt1 = arr1->dtype, dt2 = arr2->dtype;
    int cpx1 = (dt1 == COMPLEX64 || dt1 == COMPLEX128);
    int cpx2 = (dt2 == COMPLEX64 || dt2 == COMPLEX128);
    DataType out_dtype = (cpx1 || cpx2) ? COMPLEX128 : FLOAT64;

    /* Ensure both are FLOAT64 or COMPLEX128 for computation */
    Array *a1 = astype(arr1, (cpx1 || cpx2) ? COMPLEX128 : FLOAT64);
    Array *a2 = astype(arr2, (cpx1 || cpx2) ? COMPLEX128 : FLOAT64);
    if (a1 == NULL || a2 == NULL) {
        if (a1) free_array(a1);
        if (a2) free_array(a2);
        return NULL;
    }

    int n = a1->size;
    complex double sum = 0;
    complex double *d1 = (complex double*)a1->data;
    complex double *d2 = (complex double*)a2->data;

    /* Compute sum(conj(a1[i]) * a2[i]) */
    for (int i = 0; i < n; i++) {
        sum += conj(d1[i]) * d2[i];
    }

    free_array(a1);
    free_array(a2);

    /* Return as scalar FLOAT64 or COMPLEX128 */
    int one = 1;
    Array *result = create_array(&one, 1, out_dtype);
    if (result == NULL) return NULL;

    if (out_dtype == COMPLEX128) {
        *(complex double*)result->data = sum;
    } else {
        *(double*)result->data = creal(sum);
    }

    return result;
}
