#include "def.h"
#include "array.h"
#include "shape.h"
#include "function.h"
#include "utils.h"


// deletearrayinelement
Array* delete(Array *arr, Array *obj, int axis) {
    if (arr == NULL || obj == NULL) {
        fprintf(stderr, "delete: NULL argument\n");
        return NULL;
    }
    int use_flat = (axis == -1);
    if (!use_flat && (axis < 0 || axis >= arr->ndim)) {
        fprintf(stderr, "delete: axis %d out of bounds for %dD array\n", axis, arr->ndim);
        return NULL;
    }
    int max_idx = use_flat ? arr->size : arr->shape[axis];
    int del_count;
    int64_t *del_indices = prepare_delete_indices(obj, &del_count, max_idx);
    if (del_indices == NULL) return NULL;

    Array *result;
    if (use_flat) {
        // flattenarrayisone-D
        int flat_shape[1] = {arr->size};
        Array *flat = create_array(flat_shape, 1, arr->dtype);
        if (!flat) {
            free(del_indices);
            return NULL;
        }
        memcpy(flat->data, arr->data, arr->size * dtype_size(arr->dtype));
        result = delete_along_axis(flat, 0, del_indices, del_count);
        free_array(flat);
    } else {
        result = delete_along_axis(arr, axis, del_indices, del_count);
    }
    free(del_indices);
    return result;
}