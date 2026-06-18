#include "utils.h"
#include "array.h"

double complex_abs_sq(void *ptr, DataType dtype) {
    if (dtype == COMPLEX64) {
        complex float z = *(complex float*)ptr;
        return crealf(z)*crealf(z) + cimagf(z)*cimagf(z);
    } else if (dtype == COMPLEX128) {
        complex double z = *(complex double*)ptr;
        return creal(z)*creal(z) + cimag(z)*cimag(z);
    }
    return 0.0;
}
