#include <stdlib.h>
#include <string.h>
#include "array.h"
#include "utils.h"

/**
 * trim_zeros -- remove leading and trailing zeros (like numpy.trim_zeros)
 *
 * @param arr   1D array
 * @param trim  "f" (front), "b" (back), or "fb" (both, default)
 * @return new 1D array
 */
Array* trim_zeros(Array *arr, const char *trim) {
    if (arr == NULL) { fprintf(stderr, "trim_zeros: NULL argument\n"); return NULL; }
    if (arr->ndim != 1) {
        fprintf(stderr, "trim_zeros: input must be 1D\n"); return NULL;
    }

    int trim_f = 1, trim_b = 1;
    if (trim) {
        if (trim[0] == 'f' && trim[1] == 0) trim_b = 0;
        else if (trim[0] == 'b' && trim[1] == 0) trim_f = 0;
    }

    size_t esz = dtype_size(arr->dtype);
    char *src = (char*)arr->data;

    int start = 0, end = arr->size;

    if (trim_f) {
        while (start < end && !is_nonzero(src + start * esz, arr->dtype))
            start++;
    }
    if (trim_b) {
        while (end > start && !is_nonzero(src + (end-1) * esz, arr->dtype))
            end--;
    }

    int new_len = end - start;
    if (new_len < 0) new_len = 0;

    Array *result = create_array((int[]){new_len}, 1, arr->dtype);
    if (!result) return NULL;

    if (new_len > 0)
        memcpy(result->data, src + start * esz, new_len * esz);

    return result;
}
