#include "def.h"
#include "array.h"
#include "shape.h"
Array* atleast_nd(Array *arr, int target_ndim);
Array* atleast_3d(Array *arr) { return atleast_nd(arr, 3); }
