#include "array.h"
#include "def.h"
#include "utils.h"


Array* npc_std(Array *arr, int axis, int keepdims) {
    NP_UTILS_CHECK_NULL(arr, NULL);

    if (arr->size <= 1) {
        NP_UTILS_ERROR_LOG("Error", "Cannot compute standard deviation of array with size <= 1");
        return NULL;
    }

    // Use a loop-based approach to compute (x - mean)^2
    // because libnpc's subtract does not support broadcasting.

    // Compute mean (flattened to a scalar for axis=-1)
    Array *mean_arr = mean(arr, -1, 0); // axis=-1 gives scalar (0-d)
    if (mean_arr == NULL) return NULL;

    double mu;
    switch (mean_arr->dtype) {
        case FLOAT64: mu = *(double*)mean_arr->data; break;
        case FLOAT32: mu = (double)*(float*)mean_arr->data; break;
        case INT64:   mu = (double)*(int64_t*)mean_arr->data; break;
        case INT32:   mu = (double)*(int32_t*)mean_arr->data; break;
        case INT8:    mu = (double)*(int8_t*)mean_arr->data; break;
        default:      mu = 0.0; break;
    }
    NP_UTILS_SAFE_FREE_ARRAY(mean_arr);

    // Compute sum of squared differences
    size_t elem_sz = dtype_size(arr->dtype);
    char *data = (char*)arr->data;
    double sum_sq = 0.0;

    for (int i = 0; i < arr->size; i++) {
        double val;
        void *elem = data + i * elem_sz;
        switch (arr->dtype) {
            case FLOAT64: val = *(double*)elem; break;
            case FLOAT32: val = (double)*(float*)elem; break;
            case INT64:   val = (double)*(int64_t*)elem; break;
            case INT32:   val = (double)*(int32_t*)elem; break;
            case INT8:    val = (double)*(int8_t*)elem; break;
            case UINT64:  val = (double)*(uint64_t*)elem; break;
            case UINT32:  val = (double)*(uint32_t*)elem; break;
            case UINT8:   val = (double)*(uint8_t*)elem; break;
            case BOOL:    val = *(bool*)elem ? 1.0 : 0.0; break;
            default:      val = 0.0; break;
        }
        double diff = val - mu;
        sum_sq += diff * diff;
    }

    // population std = sqrt(sum_sq / N)
    double var_val = sum_sq / arr->size;
    double std_val = sqrt(var_val);

    // Build result array
    int out_ndim = keepdims ? arr->ndim : 0;
    Array *result;
    if (out_ndim == 0) {
        result = (Array*)malloc(sizeof(Array));
        if (!result) return NULL;
        result->data = malloc(sizeof(double));
        if (!result->data) { free(result); return NULL; }
        result->dtype = FLOAT64;
        result->shape = NULL;
        result->ndim = 0;
        result->size = 1;
        result->strides = NULL;
        result->is_view = 0;
        *(double*)result->data = std_val;
    } else {
        int *shape = malloc(arr->ndim * sizeof(int));
        if (!shape) return NULL;
        for (int i = 0; i < arr->ndim; i++) shape[i] = 1;
        result = empty(shape, arr->ndim, FLOAT64);
        free(shape);
        if (!result) return NULL;
        *(double*)result->data = std_val;
    }

    return result;
}