#include "def.h"
#include "array.h"
#include "shape.h"
Array* identity(int n, DataType dtype) { return eye(n, n, 0, dtype); }
