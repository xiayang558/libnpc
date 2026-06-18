#include "array.h"


Array* empty(int *shape, int ndim, DataType dtype) {
    // Handle scalar (0-D array)
    if (shape == NULL || ndim == 0) {
        double value = 0.;
        Array *arr = create_scalar_array(&value, dtype);  // createonescalar array
        return arr;
    }

    // For ndim >= 1, use create_array (only allocates memory, does not zero-fill)
    return create_array(shape, ndim, dtype);
}