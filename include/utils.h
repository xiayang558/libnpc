#ifndef UTILS_H
#define UTILS_H

#include "array.h"

#ifndef NP_UTILS_CHECK_NULL
/**
 * @brief Check whether the array is NULL
 */
#define NP_UTILS_CHECK_NULL(arr, retval) \
    if ((arr) == NULL) { \
        NP_UTILS_ERROR_LOG("Error", "NULL array argument"); \
        return retval; \
    }
#endif

#ifndef NP_UTILS_CHECK_SIZE
/**
 * @brief Check whether the array size is valid
 */
#define NP_UTILS_CHECK_SIZE(arr, retval) \
    if ((arr)->size == 0) { \
        NP_UTILS_ERROR_LOG("Error", "Empty array"); \
        return retval; \
    }
#endif

#ifndef NP_UTILS_ITERATE
/**
 * @brief Generic element iteration macro
 */
#define NP_UTILS_ITERATE(arr, type, var, idx) \
    type *var = (type*)(arr)->data; \
    for (int idx = 0; idx < (arr)->size; idx++)
#endif

#ifndef NP_INT_SUM
/**
 * @brief Integer sum macro
 */
#define NP_INT_SUM(DTYPE, TYPE, sum_int64_ptr) \
    case DTYPE: { \
        NP_UTILS_ITERATE(arr, TYPE, data, i) { \
            *sum_int64_ptr += (int64_t)data[i]; \
        } \
        result_type = INT64; \
        break; \
    }
#endif

#ifndef NP_UINT_SUM
/**
 * @brief Unsigned integer sum macro
 */
#define NP_UINT_SUM(DTYPE, TYPE, sum_uint64_ptr) \
    case DTYPE: { \
        NP_UTILS_ITERATE(arr, TYPE, data, i) { \
            *sum_uint64_ptr += (uint64_t)data[i]; \
        } \
        result_type = UINT64; \
        break; \
    }
#endif

#ifndef NP_FLOAT_SUM
/**
 * @brief Floating-point sum macro
 */
#define NP_FLOAT_SUM(DTYPE, TYPE, sum_double_ptr) \
    case DTYPE: { \
        NP_UTILS_ITERATE(arr, TYPE, data, i) { \
            *sum_double_ptr += (double)data[i]; \
        } \
        result_type = FLOAT64; \
        break; \
    }
#endif

#ifndef NP_COMPLEX_SUM
/**
 * @brief Complex number sum macro
 */
#define NP_COMPLEX_SUM(DTYPE, TYPE, sum_complex128_ptr, RES_TYPE) \
    case DTYPE: { \
        NP_UTILS_ITERATE(arr, TYPE, data, i) { \
            *sum_complex128_ptr += (complex double)data[i]; \
        } \
        result_type = RES_TYPE; \
        break; \
    }
#endif

#ifndef NP_UTILS_INT_MIN
/**
 * @brief Integer minimum value macro
 */
#define NP_UTILS_INT_MIN(DTYPE, TYPE, min_val_ptr, result_type_ptr) \
    case DTYPE: { \
        TYPE min_val = ((TYPE*)arr->data)[0]; \
        NP_UTILS_ITERATE(arr, TYPE, data, i) { \
            if (data[i] < min_val) { \
                min_val = data[i]; \
            } \
        } \
        min_val_ptr = min_val; \
        *result_type_ptr = DTYPE; \
        break; \
    }
#endif

#ifndef NP_UTILS_FLOAT_MIN
/**
 * @brief Floating-point minimum value macro
 */
#define NP_UTILS_FLOAT_MIN(DTYPE, TYPE, min_val_ptr, result_type_ptr) \
    case DTYPE: { \
        TYPE min_val = ((TYPE*)arr->data)[0]; \
        NP_UTILS_ITERATE(arr, TYPE, data, i) { \
            if (data[i] < min_val) { \
                min_val = data[i]; \
            } \
        } \
        min_val_ptr = min_val; \
        *result_type_ptr = DTYPE; \
        break; \
    }
#endif

#ifndef NP_UTILS_COMPLEX_MIN
/**
 * @brief Complex number minimum value macro (by magnitude)
 */
#define NP_UTILS_COMPLEX_MIN(DTYPE, TYPE, ABS_FUNC, min_val_ptr, result_type_ptr) \
    case DTYPE: { \
        TYPE min_val = ((TYPE*)arr->data)[0]; \
        double min_abs = ABS_FUNC(min_val); \
        NP_UTILS_ITERATE(arr, TYPE, data, i) { \
            double abs_val = ABS_FUNC(data[i]); \
            if (abs_val < min_abs) { \
                min_abs = abs_val; \
                min_val = data[i]; \
            } \
        } \
        min_val_ptr = min_val; \
        *result_type_ptr = DTYPE; \
        break; \
    }
#endif

#ifndef NP_UTILS_INT_MAX
/**
 * @brief Integer maximum value macro
 */
#define NP_UTILS_INT_MAX(DTYPE, TYPE, max_val_ptr, result_type_ptr) \
    case DTYPE: { \
        TYPE max_val = ((TYPE*)arr->data)[0]; \
        NP_UTILS_ITERATE(arr, TYPE, data, i) { \
            if (data[i] > max_val) { \
                max_val = data[i]; \
            } \
        } \
        max_val_ptr = max_val; \
        *result_type_ptr = DTYPE; \
        break; \
    }
#endif

#ifndef NP_UTILS_FLOAT_MAX
/**
 * @brief Floating-point maximum value macro
 */
#define NP_UTILS_FLOAT_MAX(DTYPE, TYPE, max_val_ptr, result_type_ptr) \
    case DTYPE: { \
        TYPE max_val = ((TYPE*)arr->data)[0]; \
        NP_UTILS_ITERATE(arr, TYPE, data, i) { \
            if (data[i] > max_val) { \
                max_val = data[i]; \
            } \
        } \
        max_val_ptr = max_val; \
        *result_type_ptr = DTYPE; \
        break; \
    }
#endif

#ifndef NP_UTILS_COMPLEX_MAX
/**
 * @brief Complex number maximum value macro (by magnitude)
 */
#define NP_UTILS_COMPLEX_MAX(DTYPE, TYPE, ABS_FUNC, max_val_ptr, result_type_ptr) \
    case DTYPE: { \
        TYPE max_val = ((TYPE*)arr->data)[0]; \
        double max_abs = ABS_FUNC(max_val); \
        NP_UTILS_ITERATE(arr, TYPE, data, i) { \
            double abs_val = ABS_FUNC(data[i]); \
            if (abs_val > max_abs) { \
                max_abs = abs_val; \
                max_val = data[i]; \
            } \
        } \
        max_val_ptr = max_val; \
        *result_type_ptr = DTYPE; \
        break; \
    }
#endif


#ifndef NP_UTILS_SAFE_FREE_ARRAY
/**
 * @brief Safely free an array and return NULL (general error handling)
 */
#define NP_UTILS_SAFE_FREE_ARRAY(arr) \
    do { \
        if ((arr) != NULL) { \
            free_array(arr); \
            (arr) = NULL; \
        } \
    } while (0)
#endif

int is_integer_val(double x);

int is_nan_float(float x);
int is_nan_double(double x);
int is_nan_long_double(long double x);
int is_nan_complex(void *ptr, DataType dtype);
int is_nan_complex64(complex float z);
int is_nan_complex128(complex double z);

int is_nan(void *ptr, DataType dtype);
int is_nan_real(void *ptr, DataType dtype);

double get_value(void *ptr, DataType dtype);
double get_real_val(void *ptr, DataType dtype);
complex double get_complex_val(void *ptr, DataType dtype);


int cmp_int(const void *a, const void *b);
int cmp_int64(const void *a, const void *b);
int cmp_uint64(const void *a, const void *b);
int cmp_float(const void *a, const void *b);
int cmp_double(const void *a, const void *b);
int cmp_complex(const void *a, const void *b);
int cmp_complex64(const void *a, const void *b);
int cmp_complex128(const void *a, const void *b);

double complex_abs_sq(void *ptr, DataType dtype);

int* prepare_indices(Array *obj, int *out_len, int max_idx);
Array* prepare_values(Array *values, int num_insert);
int64_t* prepare_delete_indices(Array *obj, int *out_len, int max_idx);
Array* delete_along_axis(Array *arr, int axis, int64_t *del_indices, int del_count);


/**
 * @brief Replace NaN and infinity in an array with finite values (like numpy.nan_to_num)
 * @param arr Input array
 * @return New array with NaN/inf replaced
 */
Array* nan_to_num(Array *arr);

/**
 * @brief Return indices of maxima along an axis, ignoring NaN (like numpy.nanargmax)
 * @param a    Input array
 * @param axis Axis (-1 means all elements)
 * @return Index array (INT64); shape = input shape without axis dimension
 */
Array* nanargmax(Array *a, int axis);

/**
 * @brief Return indices of minima along an axis, ignoring NaN (like numpy.nanargmin)
 * @param a    Input array
 * @param axis Axis (-1 means all elements)
 * @return Index array (INT64); shape = input shape without axis dimension
 */
Array* nanargmin(Array *a, int axis);
Array* isnan_array(Array *arr);       // Detect NaN elements (numpy.isnan)
Array* isinf_array(Array *arr);       // Detect infinite elements (numpy.isinf)
Array* isfinite_array(Array *arr);    // Detect finite elements (numpy.isfinite)

#endif // UTILS_H