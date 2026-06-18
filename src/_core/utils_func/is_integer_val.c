#include <math.h>
#include "array.h"
int is_integer_val(double x) {
    return fabs(x - round(x)) < 1e-12;
}
