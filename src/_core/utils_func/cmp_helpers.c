#include <math.h>
#include <complex.h>
#include <stdint.h>
#include "array.h"
int cmp_int(const void *a, const void *b) {
    int ia = *(int*)a, ib = *(int*)b;
    return (ia > ib) - (ia < ib);
}

int cmp_int64(const void *a, const void *b) {
    int64_t va = *(int64_t*)a, vb = *(int64_t*)b;
    return (va > vb) - (va < vb);
}

int cmp_uint64(const void *a, const void *b) {
    uint64_t ua = *(uint64_t*)a, ub = *(uint64_t*)b;
    return (ua > ub) - (ua < ub);
}

int cmp_float(const void *a, const void *b) {
    float fa = *(float*)a, fb = *(float*)b;
    return (fa > fb) - (fa < fb);
}

int cmp_double(const void *a, const void *b) {
    double da = *(double*)a, db = *(double*)b;
    if (da < db) return -1;
    if (da > db) return 1;
    return 0;
}

int cmp_complex(const void *a, const void *b) {
    complex double ca = *(complex double*)a, cb = *(complex double*)b;
    double a_mod = cabs(ca), b_mod = cabs(cb);
    if (a_mod < b_mod) return -1;
    if (a_mod > b_mod) return 1;
    // When magnitude is equal, compare by real part then by imaginary part
    double ra = creal(ca), ia = cimag(ca);
    double rb = creal(cb), ib = cimag(cb);
    if (ra < rb) return -1;
    if (ra > rb) return 1;
    if (ia < ib) return -1;
    if (ia > ib) return 1;
    return 0;
}

int cmp_complex64(const void *a, const void *b) {
    float ma = cabsf(*(complex float*)a);
    float mb = cabsf(*(complex float*)b);
    return (ma > mb) - (ma < mb);
}

int cmp_complex128(const void *a, const void *b) {
    double ma = cabs(*(complex double*)a);
    double mb = cabs(*(complex double*)b);
    return (ma > mb) - (ma < mb);
}


// getrealnumbernumbervalue
