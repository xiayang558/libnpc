#include "array.h"
#include "shape.h"


// Compute conjugate transpose of complex matrix
Array* conj_transpose(Array *arr) {
    if (arr == NULL || (arr->dtype != COMPLEX64 && arr->dtype != COMPLEX128)) {
        fprintf(stderr, "conj_transpose: Input must be a complex array.\n");
        return NULL;
    }
    Array *t_arr = transpose(arr);
    if (t_arr == NULL) return NULL;
    if (arr->dtype == COMPLEX64) {
        complex float *data = (complex float*)t_arr->data;
        for (int i = 0; i < t_arr->size; i++) {
            data[i] = conjf(data[i]);
        }
    } else {
        complex double *data = (complex double*)t_arr->data;
        for (int i = 0; i < t_arr->size; i++) {
            data[i] = conj(data[i]);
        }
    }
    return t_arr;
}