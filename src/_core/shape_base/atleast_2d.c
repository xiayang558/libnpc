#include "def.h"
#include "array.h"
#include "shape.h"
Array* atleast_nd(Array *arr, int target_ndim);
Array* atleast_2d(Array *arr) { return atleast_nd(arr, 2); }
