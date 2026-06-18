#include <math.h>
#include "array.h"
#include "def.h"
#include "utils.h"

Array* ptp(Array *arr, int axis, int keepdims) {
    NP_UTILS_CHECK_NULL(arr, NULL);
    NP_UTILS_CHECK_SIZE(arr, NULL);

    if (axis < -1 || axis >= arr->ndim) {
        NP_UTILS_ERROR_LOG("Error", "Invalid axis %d for %dD array", axis, arr->ndim);
        return NULL;
    }

    /* axis = -1: all elements → peak-to-peak */
    if (axis == -1) {
        double min_val = INFINITY, max_val = -INFINITY;
        int found = 0;
        size_t es = dtype_size(arr->dtype);
        char *data = (char*)arr->data;

        for (int i = 0; i < arr->size; i++) {
            double v = get_value_as_double(data + i * es, arr->dtype);
            if (!found || v < min_val) min_val = v;
            if (!found || v > max_val) max_val = v;
            found = 1;
        }
        if (!found) return create_scalar_array(&min_val, FLOAT64);

        double ptp_val = max_val - min_val;

        if (keepdims) {
            int *shape = malloc(arr->ndim * sizeof(int));
            for (int i = 0; i < arr->ndim; i++) shape[i] = 1;
            Array *r = create_array(shape, arr->ndim, FLOAT64);
            free(shape);
            if (r) *(double*)r->data = ptp_val;
            return r;
        } else {
            return create_scalar_array(&ptp_val, FLOAT64);
        }
    }

    /* Along axis: fallback to full ptp for now */
    NP_UTILS_ERROR_LOG("Warning", "Axis-specific ptp not fully implemented, using full ptp");
    return ptp(arr, -1, keepdims);
}
