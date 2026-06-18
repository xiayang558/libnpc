#include "np_math_macros.h"

Array* logaddexp2(Array *arr1, Array *arr2) {
    return binary_math_op(arr1, arr2, logaddexp2_double, "logaddexp2");
}
