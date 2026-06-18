#include "array.h"
#include "def.h"
#include "utils.h"


Array* std(Array *arr, int axis, int keepdims) {
    NP_UTILS_CHECK_NULL(arr, NULL);
    
    if (arr->size <= 1) {
        NP_UTILS_ERROR_LOG("Error", "Cannot compute standard deviation of array with size <= 1");
        return NULL;
    }
    
    // first compute the mean
    Array *mean_arr = mean(arr, axis, 1);  // keep dims for broadcasting
    if (mean_arr == NULL) {
        return NULL;
    }
    
    // compute variance: E[(X - mu)^2]
    Array *diff = subtract(arr, mean_arr);
    if (diff == NULL) {
        NP_UTILS_SAFE_FREE_ARRAY(mean_arr);
        return NULL;
    }
    
    // compute square
    Array *squared = multiply(diff, diff);
    if (squared == NULL) {
        NP_UTILS_SAFE_FREE_ARRAY(mean_arr);
        NP_UTILS_SAFE_FREE_ARRAY(diff);
        return NULL;
    }
    
    // compute mean of squares (variance)
    Array *variance = mean(squared, axis, keepdims);

    // clean up intermediate arrays
    NP_UTILS_SAFE_FREE_ARRAY(mean_arr);
    NP_UTILS_SAFE_FREE_ARRAY(diff);
    NP_UTILS_SAFE_FREE_ARRAY(squared);
    
    if (variance == NULL) {
        return NULL;
    }
    
    // compute standard deviation (square root of variance)
    Array *result = sqrt_array(variance);
    NP_UTILS_SAFE_FREE_ARRAY(variance);
    
    return result;
}