#include "np_math_macros.h"

Array* logaddexp(Array *arr1, Array *arr2) {
    return binary_math_op(arr1, arr2, logaddexp_double, "logaddexp");
}
