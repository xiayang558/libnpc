#include "array.h"
#include <string.h>

/**
 * positive — Return +x element-wise, i.e. a numeric copy (like numpy.positive)
 *
 * Returns a deep copy with the same dtype and values.
 *
 * @param x Input array
 * @return Deep copy of x, or NULL on error
 */
Array* positive(Array *x) {
    if (x == NULL) {
        fprintf(stderr, "positive: NULL array argument\n");
        return NULL;
    }

    return copy_array(x);
}
