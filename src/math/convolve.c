#include <stdlib.h>
#include "array.h"
#include "shape.h"

/**
 * convolve — discrete linear convolution (like numpy.convolve, mode="full")
 *
 * result[k] = sum_{i} a[i] * v[k-i], for k = 0..(len(a)+len(v)-2)
 *
 * Both arrays are flattened to 1D before convolution.
 *
 * @param arr1 First array (any shape, any dtype)
 * @param arr2 Second array (any shape, any dtype)
 * @return 1D FLOAT64 array of length len(arr1) + len(arr2) - 1
 */
Array* convolve(Array *arr1, Array *arr2) {
    if (arr1 == NULL || arr2 == NULL) {
        fprintf(stderr, "convolve: NULL array argument\n");
        return NULL;
    }

    if (arr1->size == 0 || arr2->size == 0) {
        fprintf(stderr, "convolve: empty array not allowed\n");
        return NULL;
    }

    /* Flatten and convert to double */
    Array *flat1 = flatten(arr1);
    Array *flat2 = flatten(arr2);
    if (!flat1 || !flat2) {
        if (flat1) free_array(flat1);
        if (flat2) free_array(flat2);
        return NULL;
    }

    Array *dbl1 = astype(flat1, FLOAT64);
    Array *dbl2 = astype(flat2, FLOAT64);
    free_array(flat1);
    free_array(flat2);
    if (!dbl1 || !dbl2) {
        if (dbl1) free_array(dbl1);
        if (dbl2) free_array(dbl2);
        return NULL;
    }

    int n1 = dbl1->size;
    int n2 = dbl2->size;
    int out_len = n1 + n2 - 1;

    double *a = (double*)dbl1->data;
    double *b = (double*)dbl2->data;

    Array *result = create_array((int[]){out_len}, 1, FLOAT64);
    if (!result) {
        free_array(dbl1);
        free_array(dbl2);
        return NULL;
    }

    double *out = (double*)result->data;
    for (int k = 0; k < out_len; k++) {
        double sum = 0.0;
        /* sum over i where 0 <= i < n1 and 0 <= k-i < n2 */
        int i_min = (k < n2) ? 0 : k - n2 + 1;
        int i_max = (k < n1) ? k : n1 - 1;
        for (int i = i_min; i <= i_max; i++) {
            sum += a[i] * b[k - i];
        }
        out[k] = sum;
    }

    free_array(dbl1);
    free_array(dbl2);
    return result;
}
