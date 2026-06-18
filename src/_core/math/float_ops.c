#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include "array.h"
#include "utils.h"

/* === fabs (already have abs_array, but fabs is float-only variant) === */
Array* fabs_array(Array *arr) {
    if (arr == NULL) return NULL;
    if (!is_integer(arr->dtype)) { /* complex not supported by fabs */
        /* Use same logic as abs but force float output */
        return abs_array(arr);
    }
    Array *result = create_array(arr->shape, arr->ndim, FLOAT64);
    if (!result) return NULL;
    double *rd = (double*)result->data;
    size_t esz = dtype_size(arr->dtype);
    char *src = (char*)arr->data;
    for (int i = 0; i < arr->size; i++)
        rd[i] = fabs(get_value(src + i * esz, arr->dtype));
    return result;
}

/* === copysign === */
Array* copysign_array(Array *x1, Array *x2) {
    if (x1 == NULL || x2 == NULL) return NULL;
    if (x1->size != x2->size) return NULL;
    Array *result = create_array(x1->shape, x1->ndim, FLOAT64);
    if (!result) return NULL;
    double *rd = (double*)result->data;
    size_t esz1 = dtype_size(x1->dtype), esz2 = dtype_size(x2->dtype);
    char *s1 = (char*)x1->data, *s2 = (char*)x2->data;
    for (int i = 0; i < x1->size; i++)
        rd[i] = copysign(get_value(s1 + i * esz1, x1->dtype),
                          get_value(s2 + i * esz2, x2->dtype));
    return result;
}

/* === nextafter === */
Array* nextafter_array(Array *x1, Array *x2) {
    if (x1 == NULL || x2 == NULL) return NULL;
    if (x1->size != x2->size) return NULL;
    Array *result = create_array(x1->shape, x1->ndim, FLOAT64);
    if (!result) return NULL;
    double *rd = (double*)result->data;
    size_t esz1 = dtype_size(x1->dtype), esz2 = dtype_size(x2->dtype);
    char *s1 = (char*)x1->data, *s2 = (char*)x2->data;
    for (int i = 0; i < x1->size; i++)
        rd[i] = nextafter(get_value(s1 + i * esz1, x1->dtype),
                           get_value(s2 + i * esz2, x2->dtype));
    return result;
}

/* === spacing === */
Array* spacing_array(Array *arr) {
    if (arr == NULL) return NULL;
    Array *result = create_array(arr->shape, arr->ndim, FLOAT64);
    if (!result) return NULL;
    double *rd = (double*)result->data;
    size_t esz = dtype_size(arr->dtype);
    char *src = (char*)arr->data;
    for (int i = 0; i < arr->size; i++) {
        double v = fabs(get_value(src + i * esz, arr->dtype));
        rd[i] = (v == 0.0) ? 0.0 : nextafter(v, INFINITY) - v;
    }
    return result;
}

/* === frexp === */
int frexp_array(Array *arr, Array **mant, Array **exp) {
    if (arr == NULL || !mant || !exp) return -1;
    *mant = create_array(arr->shape, arr->ndim, FLOAT64);
    *exp = create_array(arr->shape, arr->ndim, INT32);
    if (!*mant || !*exp) { free_array(*mant); free_array(*exp); return -1; }
    double *md = (double*)(*mant)->data;
    int32_t *ed = (int32_t*)(*exp)->data;
    size_t esz = dtype_size(arr->dtype);
    char *src = (char*)arr->data;
    for (int i = 0; i < arr->size; i++) {
        double v = get_value(src + i * esz, arr->dtype);
        int e;
        md[i] = frexp(v, &e);
        ed[i] = e;
    }
    return 0;
}

/* === modf === */
int modf_array(Array *arr, Array **frac, Array **integ) {
    if (arr == NULL || !frac || !integ) return -1;
    *frac = create_array(arr->shape, arr->ndim, FLOAT64);
    *integ = create_array(arr->shape, arr->ndim, FLOAT64);
    if (!*frac || !*integ) { free_array(*frac); free_array(*integ); return -1; }
    double *fd = (double*)(*frac)->data;
    double *id = (double*)(*integ)->data;
    size_t esz = dtype_size(arr->dtype);
    char *src = (char*)arr->data;
    for (int i = 0; i < arr->size; i++) {
        double v = get_value(src + i * esz, arr->dtype);
        fd[i] = modf(v, &id[i]);
    }
    return 0;
}
