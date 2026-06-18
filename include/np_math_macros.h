#ifndef NP_MATH_MACROS_H
#define NP_MATH_MACROS_H

#include "array.h"
#include "def.h"
#include "utils.h"
#include "function.h"

/* ─── Helper functions ─── */
static inline void calculate_strides(const int *shape, int ndim, int *strides) {
    strides[ndim - 1] = 1;
    for (int i = ndim - 2; i >= 0; i--) {
        strides[i] = strides[i + 1] * shape[i + 1];
    }
}

static inline int get_flat_index(const int *indices, const int *strides, int ndim) {
    int idx = 0;
    for (int i = 0; i < ndim; i++) idx += indices[i] * strides[i];
    return idx;
}

/* ─── Overflow checks ─── */
#ifdef __GNUC__
#define NP_ADD_OVERFLOW(a, b, res) __builtin_add_overflow(a, b, res)
#define NP_SUB_OVERFLOW(a, b, res) __builtin_sub_overflow(a, b, res)
#define NP_MUL_OVERFLOW(a, b, res) __builtin_mul_overflow(a, b, res)
#else
static inline int np_add_overflow(int64_t a, int64_t b, int64_t *res) {
    *res = a + b;
    return ((a > 0 && b > 0 && *res < 0) || (a < 0 && b < 0 && *res > 0));
}
static inline int np_sub_overflow(int64_t a, int64_t b, int64_t *res) {
    *res = a - b;
    return ((a < 0 && b > 0 && *res > 0) || (a > 0 && b < 0 && *res < 0));
}
static inline int np_mul_overflow(int64_t a, int64_t b, int64_t *res) {
    *res = a * b;
    return (a != 0 && *res / a != b);
}
#define NP_ADD_OVERFLOW(a, b, res) np_add_overflow((int64_t)a, (int64_t)b, (int64_t*)res)
#define NP_SUB_OVERFLOW(a, b, res) np_sub_overflow((int64_t)a, (int64_t)b, (int64_t*)res)
#define NP_MUL_OVERFLOW(a, b, res) np_mul_overflow((int64_t)a, (int64_t)b, (int64_t*)res)
#endif

/* ─── Element iteration ─── */
#define NP_ITERATE_ELEMENTS(arr, type, var, idx) \
    type var = (type)(arr)->data; \
    for (int idx = 0; idx < (arr)->size; idx++)

/* ─── Element-wise operation macros ─── */
#define NP_ELEMENTWISE_OP(OP_NAME, OP, DTYPE, TYPE) \
    case DTYPE: { \
        NP_ITERATE_ELEMENTS(arr1, TYPE*, data1, i) { \
            TYPE* data2 = (TYPE*)arr2->data; \
            TYPE* res_data = (TYPE*)result->data; \
            res_data[i] = data1[i] OP data2[i]; \
        } \
        break; \
    }

#define NP_INTEGER_OP(OP_NAME, OP_FUNC, DTYPE, TYPE) \
    case DTYPE: { \
        NP_ITERATE_ELEMENTS(arr1, TYPE*, data1, i) { \
            TYPE* data2 = (TYPE*)arr2->data; \
            TYPE* res_data = (TYPE*)result->data; \
            if (OP_FUNC(data1[i], data2[i], &res_data[i])) { \
                NP_UTILS_ERROR_LOG("Warning", "Integer %s overflow at index %d (type: %s)", \
                            OP_NAME, i, #TYPE); \
            } \
        } \
        break; \
    }

/* ─── Division ─── */
#define NP_DIV_OP(DTYPE, TYPE, ZERO_CHECK) \
    case DTYPE: { \
        NP_ITERATE_ELEMENTS(arr1, TYPE*, data1, i) { \
            TYPE* data2 = (TYPE*)arr2->data; \
            TYPE* res_data = (TYPE*)result->data; \
            if (ZERO_CHECK) { \
                NP_UTILS_ERROR_LOG("Error", "Division by zero at index %d (type: %s)", i, #TYPE); \
                NP_UTILS_SAFE_FREE_ARRAY(result); \
                return NULL; \
            } \
            res_data[i] = data1[i] / data2[i]; \
        } \
        break; \
    }

#define NP_INTEGER_DIV(DTYPE, TYPE)  NP_DIV_OP(DTYPE, TYPE, data2[i] == 0)
#define NP_FLOAT_DIV(DTYPE, TYPE)    NP_DIV_OP(DTYPE, TYPE, fabs(data2[i]) < 1e-10)
#define NP_COMPLEX_DIV(DTYPE, TYPE)  NP_DIV_OP(DTYPE, TYPE, creal(data2[i]) == 0 && cimag(data2[i]) == 0)

/* ─── Scalar operation macros ─── */
#define NP_SCALAR_OP(OP_NAME, OP, DTYPE, TYPE) \
    case DTYPE: { \
        NP_ITERATE_ELEMENTS(arr, TYPE*, data, i) { \
            TYPE* res_data = (TYPE*)result->data; \
            TYPE s = *(TYPE*)scalar; \
            res_data[i] = data[i] OP s; \
        } \
        break; \
    }

#define NP_INTEGER_SCALAR_OP(OP_NAME, OP_FUNC, DTYPE, TYPE) \
    case DTYPE: { \
        NP_ITERATE_ELEMENTS(arr, TYPE*, data, i) { \
            TYPE* res_data = (TYPE*)result->data; \
            TYPE s = *(TYPE*)scalar; \
            if (OP_FUNC(data[i], s, &res_data[i])) { \
                NP_UTILS_ERROR_LOG("Warning", "Integer scalar %s overflow at index %d (type: %s)", \
                            OP_NAME, i, #TYPE); \
            } \
        } \
        break; \
    }

/* ─── Multi-dimensional dot product macros ─── */
#define NP_INTEGER_DOT_MULTI_OP(DTYPE, TYPE) \
    case DTYPE: { \
        TYPE *data1 = (TYPE*)arr1->data; \
        TYPE *data2 = (TYPE*)arr2->data; \
        TYPE *res_data = (TYPE*)result->data; \
        int *strides1 = malloc(arr1->ndim * sizeof(int)); \
        int *strides2 = malloc(arr2->ndim * sizeof(int)); \
        int *strides_res = malloc(result->ndim * sizeof(int)); \
        if (!strides1 || !strides2 || !strides_res) { \
            NP_UTILS_ERROR_LOG("Error", "Malloc failed for strides (type: %s)", #TYPE); \
            free(strides1); free(strides2); free(strides_res); \
            NP_UTILS_SAFE_FREE_ARRAY(result); \
            return NULL; \
        } \
        calculate_strides(arr1->shape, arr1->ndim, strides1); \
        calculate_strides(arr2->shape, arr2->ndim, strides2); \
        calculate_strides(result->shape, result->ndim, strides_res); \
        int k = arr1->shape[arr1->ndim-1]; \
        int res_ndim = result->ndim; \
        int batch_ndim = res_ndim - (res_ndim >= 1 ? 1 : 0); \
        int *res_indices = calloc(res_ndim, sizeof(int)); \
        if (!res_indices) { \
            NP_UTILS_ERROR_LOG("Error", "Malloc failed for res_indices (type: %s)", #TYPE); \
            free(strides1); free(strides2); free(strides_res); free(res_indices); \
            NP_UTILS_SAFE_FREE_ARRAY(result); \
            return NULL; \
        } \
        while (1) { \
            int res_idx = get_flat_index(res_indices, strides_res, res_ndim); \
            TYPE sum = 0; \
            int *arr1_indices = calloc(arr1->ndim, sizeof(int)); \
            int *arr2_indices = calloc(arr2->ndim, sizeof(int)); \
            if (!arr1_indices || !arr2_indices) { \
                NP_UTILS_ERROR_LOG("Error", "Malloc failed for indices (type: %s)", #TYPE); \
                free(arr1_indices); free(arr2_indices); free(res_indices); \
                free(strides1); free(strides2); free(strides_res); \
                NP_UTILS_SAFE_FREE_ARRAY(result); \
                return NULL; \
            } \
            for (int i = 0; i < batch_ndim; i++) { \
                if (i < arr1->ndim - 1) arr1_indices[i] = res_indices[i]; \
                if (i < arr2->ndim - 2) arr2_indices[i] = res_indices[i]; \
            } \
            if (res_ndim >= 1) arr2_indices[arr2->ndim - 1] = res_indices[res_ndim - 1]; \
            for (int ik = 0; ik < k; ik++) { \
                arr1_indices[arr1->ndim - 1] = ik; \
                arr2_indices[arr2->ndim - 2] = ik; \
                int arr1_idx = get_flat_index(arr1_indices, strides1, arr1->ndim); \
                int arr2_idx = get_flat_index(arr2_indices, strides2, arr2->ndim); \
                TYPE mul_res; \
                if (NP_MUL_OVERFLOW(data1[arr1_idx], data2[arr2_idx], &mul_res)) { \
                    NP_UTILS_ERROR_LOG("Warning", "Integer dot multiply overflow res_idx=%d k=%d (type: %s)", res_idx, ik, #TYPE); \
                } \
                if (NP_ADD_OVERFLOW(sum, mul_res, &sum)) { \
                    NP_UTILS_ERROR_LOG("Warning", "Integer dot add overflow res_idx=%d k=%d (type: %s)", res_idx, ik, #TYPE); \
                } \
            } \
            res_data[res_idx] = sum; \
            free(arr1_indices); free(arr2_indices); \
            int carry = 1; \
            for (int i = res_ndim - 1; i >= 0 && carry; i--) { \
                res_indices[i] += carry; \
                if (res_indices[i] >= result->shape[i]) { res_indices[i] = 0; carry = 1; } \
                else carry = 0; \
            } \
            if (carry) break; \
        } \
        free(res_indices); free(strides1); free(strides2); free(strides_res); \
        break; \
    }

#define NP_GENERAL_DOT_MULTI_OP(DTYPE, TYPE, SUM_INIT) \
    case DTYPE: { \
        TYPE *data1 = (TYPE*)arr1->data; \
        TYPE *data2 = (TYPE*)arr2->data; \
        TYPE *res_data = (TYPE*)result->data; \
        int *strides1 = malloc(arr1->ndim * sizeof(int)); \
        int *strides2 = malloc(arr2->ndim * sizeof(int)); \
        int *strides_res = malloc(result->ndim * sizeof(int)); \
        if (!strides1 || !strides2 || !strides_res) { \
            NP_UTILS_ERROR_LOG("Error", "Malloc failed for strides (type: %s)", #TYPE); \
            free(strides1); free(strides2); free(strides_res); \
            NP_UTILS_SAFE_FREE_ARRAY(result); \
            return NULL; \
        } \
        calculate_strides(arr1->shape, arr1->ndim, strides1); \
        calculate_strides(arr2->shape, arr2->ndim, strides2); \
        calculate_strides(result->shape, result->ndim, strides_res); \
        int k = arr1->shape[arr1->ndim-1]; \
        int res_ndim = result->ndim; \
        int batch_ndim = res_ndim - (res_ndim >= 1 ? 1 : 0); \
        int *res_indices = calloc(res_ndim, sizeof(int)); \
        if (!res_indices) { \
            NP_UTILS_ERROR_LOG("Error", "Malloc failed for res_indices (type: %s)", #TYPE); \
            free(strides1); free(strides2); free(strides_res); free(res_indices); \
            NP_UTILS_SAFE_FREE_ARRAY(result); \
            return NULL; \
        } \
        while (1) { \
            int res_idx = get_flat_index(res_indices, strides_res, res_ndim); \
            TYPE sum = SUM_INIT; \
            int *arr1_indices = calloc(arr1->ndim, sizeof(int)); \
            int *arr2_indices = calloc(arr2->ndim, sizeof(int)); \
            if (!arr1_indices || !arr2_indices) { \
                NP_UTILS_ERROR_LOG("Error", "Malloc failed for indices (type: %s)", #TYPE); \
                free(arr1_indices); free(arr2_indices); free(res_indices); \
                free(strides1); free(strides2); free(strides_res); \
                NP_UTILS_SAFE_FREE_ARRAY(result); \
                return NULL; \
            } \
            for (int i = 0; i < batch_ndim; i++) { \
                if (i < arr1->ndim - 1) arr1_indices[i] = res_indices[i]; \
                if (i < arr2->ndim - 2) arr2_indices[i] = res_indices[i]; \
            } \
            if (res_ndim >= 1) arr2_indices[arr2->ndim - 1] = res_indices[res_ndim - 1]; \
            for (int ik = 0; ik < k; ik++) { \
                arr1_indices[arr1->ndim - 1] = ik; \
                arr2_indices[arr2->ndim - 2] = ik; \
                int arr1_idx = get_flat_index(arr1_indices, strides1, arr1->ndim); \
                int arr2_idx = get_flat_index(arr2_indices, strides2, arr2->ndim); \
                sum += data1[arr1_idx] * data2[arr2_idx]; \
            } \
            res_data[res_idx] = sum; \
            free(arr1_indices); free(arr2_indices); \
            int carry = 1; \
            for (int i = res_ndim - 1; i >= 0 && carry; i--) { \
                res_indices[i] += carry; \
                if (res_indices[i] >= result->shape[i]) { res_indices[i] = 0; carry = 1; } \
                else carry = 0; \
            } \
            if (carry) break; \
        } \
        free(res_indices); free(strides1); free(strides2); free(strides_res); \
        break; \
    }

/* ─── Absolute value ─── */
#define NP_INT_ABS_OP(DTYPE, TYPE) \
    case DTYPE: { \
        NP_ITERATE_ELEMENTS(arr, TYPE*, data, i) { \
            TYPE* res_data = (TYPE*)result->data; \
            res_data[i] = (data[i] < 0) ? -data[i] : data[i]; \
        } \
        break; \
    }

#define NP_FLOAT_ABS_OP(DTYPE, TYPE, ABS_FUNC) \
    case DTYPE: { \
        NP_ITERATE_ELEMENTS(arr, TYPE*, data, i) { \
            TYPE* res_data = (TYPE*)result->data; \
            res_data[i] = ABS_FUNC(data[i]); \
        } \
        break; \
    }

#define NP_COMPLEX_ABS_OP(DTYPE, TYPE, ABS_FUNC, RES_TYPE) \
    case DTYPE: { \
        NP_ITERATE_ELEMENTS(arr, TYPE*, data, i) { \
            RES_TYPE* res_data = (RES_TYPE*)result->data; \
            res_data[i] = ABS_FUNC(data[i]); \
        } \
        break; \
    }

/* ─── Square root ─── */
#define NP_INT_SQRT_OP(DTYPE, TYPE) \
    case DTYPE: { \
        NP_ITERATE_ELEMENTS(arr, TYPE*, data, i) { \
            float* res_data = (float*)result->data; \
            if (data[i] < 0) { \
                NP_UTILS_ERROR_LOG("Error", "Negative value at index %d (type: %s)", i, #TYPE); \
                NP_UTILS_SAFE_FREE_ARRAY(result); \
                return NULL; \
            } \
            res_data[i] = sqrtf((float)data[i]); \
        } \
        break; \
    }

#define NP_UINT_SQRT_OP(DTYPE, TYPE) \
    case DTYPE: { \
        NP_ITERATE_ELEMENTS(arr, TYPE*, data, i) { \
            float* res_data = (float*)result->data; \
            res_data[i] = sqrtf((float)data[i]); \
        } \
        break; \
    }

#define NP_FLOAT_SQRT_OP(DTYPE, TYPE, SQRT_FUNC) \
    case DTYPE: { \
        NP_ITERATE_ELEMENTS(arr, TYPE*, data, i) { \
            TYPE* res_data = (TYPE*)result->data; \
            if (data[i] < 0.0) { \
                NP_UTILS_ERROR_LOG("Error", "Negative value at index %d (type: %s)", i, #TYPE); \
                NP_UTILS_SAFE_FREE_ARRAY(result); \
                return NULL; \
            } \
            res_data[i] = SQRT_FUNC(data[i]); \
        } \
        break; \
    }

#define NP_COMPLEX_SQRT_OP(DTYPE, TYPE, SQRT_FUNC) \
    case DTYPE: { \
        NP_ITERATE_ELEMENTS(arr, TYPE*, data, i) { \
            TYPE* res_data = (TYPE*)result->data; \
            res_data[i] = SQRT_FUNC(data[i]); \
        } \
        break; \
    }

/* ─── Exponential ─── */
#define NP_INT_EXP_OP(DTYPE, TYPE) \
    case DTYPE: { \
        NP_ITERATE_ELEMENTS(arr, TYPE*, data, i) { \
            float* res_data = (float*)result->data; \
            res_data[i] = expf((float)data[i]); \
        } \
        break; \
    }

#define NP_UINT_EXP_OP(DTYPE, TYPE) \
    case DTYPE: { \
        NP_ITERATE_ELEMENTS(arr, TYPE*, data, i) { \
            float* res_data = (float*)result->data; \
            res_data[i] = expf((float)data[i]); \
        } \
        break; \
    }

#define NP_FLOAT_EXP_OP(DTYPE, TYPE, EXP_FUNC) \
    case DTYPE: { \
        NP_ITERATE_ELEMENTS(arr, TYPE*, data, i) { \
            TYPE* res_data = (TYPE*)result->data; \
            res_data[i] = EXP_FUNC(data[i]); \
        } \
        break; \
    }

#define NP_COMPLEX_EXP_OP(DTYPE, TYPE, EXP_FUNC) \
    case DTYPE: { \
        NP_ITERATE_ELEMENTS(arr, TYPE*, data, i) { \
            TYPE* res_data = (TYPE*)result->data; \
            res_data[i] = EXP_FUNC(data[i]); \
        } \
        break; \
    }

/* ─── Logarithm ─── */
#define NP_INT_LOG_OP(DTYPE, TYPE, LOG_FUNC, BASE_MSG) \
    case DTYPE: { \
        NP_ITERATE_ELEMENTS(arr, TYPE*, data, i) { \
            double* res_data = (double*)result->data; \
            if (data[i] <= 0) { \
                NP_UTILS_ERROR_LOG("Error", "Non-positive value at index %d (type: %s)", i, #TYPE); \
                NP_UTILS_SAFE_FREE_ARRAY(result); \
                return NULL; \
            } \
            res_data[i] = LOG_FUNC((double)data[i]); \
        } \
        break; \
    }

#define NP_UINT_LOG_OP(DTYPE, TYPE, LOG_FUNC, BASE_MSG) \
    case DTYPE: { \
        NP_ITERATE_ELEMENTS(arr, TYPE*, data, i) { \
            double* res_data = (double*)result->data; \
            if (data[i] == 0) { \
                NP_UTILS_ERROR_LOG("Error", "Zero value at index %d (type: %s)", i, #TYPE); \
                NP_UTILS_SAFE_FREE_ARRAY(result); \
                return NULL; \
            } \
            res_data[i] = LOG_FUNC((double)data[i]); \
        } \
        break; \
    }

#define NP_FLOAT_LOG_OP(DTYPE, TYPE, LOG_FUNC, BASE_MSG) \
    case DTYPE: { \
        NP_ITERATE_ELEMENTS(arr, TYPE*, data, i) { \
            double* res_data = (double*)result->data; \
            if (data[i] <= 0.0) { \
                NP_UTILS_ERROR_LOG("Error", "Non-positive value at index %d (type: %s)", i, #TYPE); \
                NP_UTILS_SAFE_FREE_ARRAY(result); \
                return NULL; \
            } \
            res_data[i] = LOG_FUNC((double)data[i]); \
        } \
        break; \
    }

#define NP_COMPLEX_LOG_OP(DTYPE, TYPE, CLOG_FUNC, BASE_VAL, BASE_MSG) \
    case DTYPE: { \
        NP_ITERATE_ELEMENTS(arr, TYPE*, data, i) { \
            complex double* res_data = (complex double*)result->data; \
            if (data[i] == 0) { \
                NP_UTILS_ERROR_LOG("Error", "Zero value at index %d (type: %s)", i, #TYPE); \
                NP_UTILS_SAFE_FREE_ARRAY(result); \
                return NULL; \
            } \
            res_data[i] = CLOG_FUNC((complex double)data[i]) / CLOG_FUNC(BASE_VAL); \
        } \
        break; \
    }

#define NP_LONG_COMPLEX_LOG_OP(DTYPE, TYPE, CLOG_FUNC, BASE_VAL, BASE_MSG) \
    case DTYPE: { \
        NP_ITERATE_ELEMENTS(arr, TYPE*, data, i) { \
            long double complex* res_data = (long double complex*)result->data; \
            if (data[i] == 0) { \
                NP_UTILS_ERROR_LOG("Error", "Zero value at index %d (type: %s)", i, #TYPE); \
                NP_UTILS_SAFE_FREE_ARRAY(result); \
                return NULL; \
            } \
            res_data[i] = CLOG_FUNC(data[i]) / CLOG_FUNC(BASE_VAL); \
        } \
        break; \
    }

/* ─── safe_* static helper functions ─── */
static inline double safe_sin(double x) { return sin(x); }
static inline float safe_sinf(float x) { return sinf(x); }
static inline long double safe_sinl(long double x) { return sinl(x); }
static inline double safe_cos(double x) { return cos(x); }
static inline float safe_cosf(float x) { return cosf(x); }
static inline long double safe_cosl(long double x) { return cosl(x); }
static inline double safe_tan(double x) { return tan(x); }
static inline float safe_tanf(float x) { return tanf(x); }
static inline long double safe_tanl(long double x) { return tanl(x); }
static inline double safe_sinh(double x) { return sinh(x); }
static inline float safe_sinhf(float x) { return sinhf(x); }
static inline long double safe_sinhl(long double x) { return sinhl(x); }
static inline double safe_cosh(double x) { return cosh(x); }
static inline float safe_coshf(float x) { return coshf(x); }
static inline long double safe_coshl(long double x) { return coshl(x); }
static inline double safe_tanh(double x) { return tanh(x); }
static inline float safe_tanhf(float x) { return tanhf(x); }
static inline long double safe_tanhl(long double x) { return tanhl(x); }
static inline double safe_acos(double x) { return (x < -1.0 || x > 1.0) ? NAN : acos(x); }
static inline float safe_acosf(float x) { return (x < -1.0f || x > 1.0f) ? (float)NAN : acosf(x); }
static inline long double safe_acosl(long double x) { return (x < -1.0L || x > 1.0L) ? (long double)NAN : acosl(x); }
static inline double safe_acosh(double x) { return (x < 1.0) ? NAN : acosh(x); }
static inline float safe_acoshf(float x) { return (x < 1.0f) ? (float)NAN : acoshf(x); }
static inline long double safe_acoshl(long double x) { return (x < 1.0L) ? (long double)NAN : acoshl(x); }
static inline double safe_asin(double x) { return (x < -1.0 || x > 1.0) ? NAN : asin(x); }
static inline float safe_asinf(float x) { return (x < -1.0f || x > 1.0f) ? (float)NAN : asinf(x); }
static inline long double safe_asinl(long double x) { return (x < -1.0L || x > 1.0L) ? (long double)NAN : asinl(x); }
static inline double safe_asinh(double x) { return asinh(x); }
static inline float safe_asinhf(float x) { return asinhf(x); }
static inline long double safe_asinhl(long double x) { return asinhl(x); }
static inline double safe_atan(double x) { return atan(x); }
static inline float safe_atanf(float x) { return atanf(x); }
static inline long double safe_atanl(long double x) { return atanl(x); }
static inline double safe_atanh(double x) { return (x <= -1.0 || x >= 1.0) ? NAN : atanh(x); }
static inline float safe_atanhf(float x) { return (x <= -1.0f || x >= 1.0f) ? (float)NAN : atanhf(x); }
static inline long double safe_atanhl(long double x) { return (x <= -1.0L || x >= 1.0L) ? (long double)NAN : atanhl(x); }

/* ─── DEFINE_UNARY_FUNC template macro ─── */
#define DEFINE_UNARY_FUNC(name, c_real_func, c_real_funcf, c_real_funcl, c_complex_func, c_complex_funcf, c_complex_funcl) \
Array* name(Array *arr) { \
    if (arr == NULL) { fprintf(stderr, "%s: NULL input\n", #name); return NULL; } \
    DataType out_dtype; int is_complex = 0; \
    switch (arr->dtype) { \
        case INT8: case INT16: case INT32: case INT64: \
        case UINT8: case UINT16: case UINT32: case UINT64: out_dtype = FLOAT64; break; \
        case FLOAT32: out_dtype = FLOAT32; break; \
        case FLOAT64: out_dtype = FLOAT64; break; \
        case FLOAT128: out_dtype = FLOAT128; break; \
        case COMPLEX64: out_dtype = COMPLEX64; is_complex = 1; break; \
        case COMPLEX128: out_dtype = COMPLEX128; is_complex = 1; break; \
        case COMPLEX256: out_dtype = COMPLEX256; is_complex = 1; break; \
        default: fprintf(stderr, "%s: unsupported dtype %s\n", #name, dtype_name(arr->dtype)); return NULL; \
    } \
    Array *res = create_array(arr->shape, arr->ndim, out_dtype); \
    if (res == NULL) return NULL; \
    size_t in_sz = dtype_size(arr->dtype), out_sz = dtype_size(out_dtype); \
    char *in = (char*)arr->data, *out = (char*)res->data; \
    for (int i = 0; i < arr->size; i++) { \
        void *pi = in + i*in_sz, *po = out + i*out_sz; \
        if (is_complex) { \
            switch (arr->dtype) { \
                case COMPLEX64:  *(complex float*)po = c_complex_funcf(*(complex float*)pi); break; \
                case COMPLEX128: *(complex double*)po = c_complex_func(*(complex double*)pi); break; \
                case COMPLEX256: *(complex long double*)po = c_complex_funcl(*(complex long double*)pi); break; \
                default: break; \
            } \
        } else { \
            switch (arr->dtype) { \
                case INT8: case INT16: case INT32: case INT64: \
                case UINT8: case UINT16: case UINT32: case UINT64: \
                case FLOAT64: { \
                    double val; \
                    if (arr->dtype == FLOAT64) val = *(double*)pi; \
                    else if (arr->dtype == INT32) val = *(int32_t*)pi; \
                    else if (arr->dtype == INT64) val = *(int64_t*)pi; \
                    else if (arr->dtype == UINT32) val = *(uint32_t*)pi; \
                    else if (arr->dtype == UINT64) val = *(uint64_t*)pi; \
                    else if (arr->dtype == INT8) val = *(int8_t*)pi; \
                    else if (arr->dtype == INT16) val = *(int16_t*)pi; \
                    else if (arr->dtype == UINT8) val = *(uint8_t*)pi; \
                    else if (arr->dtype == UINT16) val = *(uint16_t*)pi; \
                    else val = 0.0; \
                    *(double*)po = c_real_func(val); \
                    break; \
                } \
                case FLOAT32: { float val = *(float*)pi; *(float*)po = c_real_funcf(val); break; } \
                case FLOAT128: { long double val = *(long double*)pi; *(long double*)po = c_real_funcl(val); break; } \
                default: break; \
            } \
        } \
    } \
    return res; \
}

#define DEFINE_UNARY_FUNC_NOCOMPLEX(name, c_real_func, c_real_funcf, c_real_funcl) \
Array* name(Array *arr) { \
    if (arr == NULL) { fprintf(stderr, #name ": NULL array argument\n"); return NULL; } \
    DataType out_dtype; \
    switch (arr->dtype) { \
        case INT8: case INT16: case INT32: case INT64: \
        case UINT8: case UINT16: case UINT32: case UINT64: out_dtype = FLOAT64; break; \
        case FLOAT32: out_dtype = FLOAT32; break; \
        case FLOAT64: out_dtype = FLOAT64; break; \
        case FLOAT128: out_dtype = FLOAT128; break; \
        case COMPLEX64: case COMPLEX128: case COMPLEX256: \
            fprintf(stderr, #name ": not supported for complex numbers\n"); return NULL; \
        default: fprintf(stderr, #name ": unsupported data type %s\n", dtype_name(arr->dtype)); return NULL; \
    } \
    Array *res = create_array(arr->shape, arr->ndim, out_dtype); \
    if (res == NULL) return NULL; \
    size_t in_sz = dtype_size(arr->dtype), out_sz = dtype_size(out_dtype); \
    char *in = (char*)arr->data, *out = (char*)res->data; \
    for (int i = 0; i < arr->size; i++) { \
        void *pi = in + i*in_sz, *po = out + i*out_sz; \
        switch (arr->dtype) { \
            case INT8: case INT16: case INT32: case INT64: \
            case UINT8: case UINT16: case UINT32: case UINT64: \
            case FLOAT64: { \
                double val; \
                if (arr->dtype == FLOAT64) val = *(double*)pi; \
                else if (arr->dtype == INT32) val = *(int32_t*)pi; \
                else if (arr->dtype == INT64) val = *(int64_t*)pi; \
                else if (arr->dtype == UINT32) val = *(uint32_t*)pi; \
                else if (arr->dtype == UINT64) val = *(uint64_t*)pi; \
                else if (arr->dtype == INT8) val = *(int8_t*)pi; \
                else if (arr->dtype == INT16) val = *(int16_t*)pi; \
                else if (arr->dtype == UINT8) val = *(uint8_t*)pi; \
                else if (arr->dtype == UINT16) val = *(uint16_t*)pi; \
                else val = 0.0; \
                *(double*)po = c_real_func(val); \
                break; \
            } \
            case FLOAT32: { float val = *(float*)pi; *(float*)po = c_real_funcf(val); break; } \
            case FLOAT128: { long double val = *(long double*)pi; *(long double*)po = c_real_funcl(val); break; } \
            default: break; \
        } \
    } \
    return res; \
}

/* ─── deg2rad / degrees static helpers ─── */
static inline double deg2rad_real(double x)    { return x * (M_PI / 180.0); }
static inline float deg2rad_realf(float x)    { return x * (float)(M_PI / 180.0); }
static inline long double deg2rad_reall(long double x) { return x * (M_PI / 180.0L); }
static inline double degrees_real(double x)    { return x * (180.0 / M_PI); }
static inline float degrees_realf(float x)    { return x * (float)(180.0 / M_PI); }
static inline long double degrees_reall(long double x) { return x * (180.0L / M_PI); }
static inline double rad2deg_real(double x)    { return x * (180.0 / M_PI); }
static inline float rad2deg_realf(float x)    { return x * (float)(180.0 / M_PI); }
static inline long double rad2deg_reall(long double x) { return x * (180.0L / M_PI); }
static inline double radians_real(double x)    { return x * (M_PI / 180.0); }
static inline float radians_realf(float x)    { return x * (float)(M_PI / 180.0); }
static inline long double radians_reall(long double x) { return x * (M_PI / 180.0L); }
static inline double floor_real(double x)    { return floor(x); }
static inline float floor_realf(float x)    { return floorf(x); }
static inline long double floor_reall(long double x) { return floorl(x); }
static inline double ceil_real(double x)    { return ceil(x); }
static inline float ceil_realf(float x)    { return ceilf(x); }
static inline long double ceil_reall(long double x) { return ceill(x); }

/* ─── rint helpers ─── */
static inline double rint_real(double x)    { return rint(x); }
static inline float rint_realf(float x)    { return rintf(x); }
static inline long double rint_reall(long double x) { return rintl(x); }

/* ─── trunc helpers ─── */
static inline double trunc_real(double x)    { return trunc(x); }
static inline float trunc_realf(float x)    { return truncf(x); }
static inline long double trunc_reall(long double x) { return truncl(x); }

/* ─── sinc helpers ─── */
static inline double sinc_real(double x)    { if (x == 0.0) return 1.0; double v = M_PI * x; return sin(v) / v; }
static inline float sinc_realf(float x)    { if (x == 0.0f) return 1.0f; float v = (float)M_PI * x; return sinf(v) / v; }
static inline long double sinc_reall(long double x) { if (x == 0.0L) return 1.0L; long double v = (long double)M_PI * x; return sinl(v) / v; }

/* ─── logaddexp helpers ─── */
static inline double logaddexp_double(double a, double b) {
    if (a == -INFINITY && b == -INFINITY) return -INFINITY;
    if (a > b) return a + log1p(exp(b - a));
    else return b + log1p(exp(a - b));
}

static inline double logaddexp2_double(double a, double b) {
    if (a == -INFINITY && b == -INFINITY) return -INFINITY;
    if (a > b) return a + log2(1.0 + exp2(b - a));
    else return b + log2(1.0 + exp2(a - b));
}

/* ─── binary_math_op (broadcasting binary function) ─── */
static inline Array* binary_math_op(Array *x1, Array *x2,
                                     double (*func)(double, double),
                                     const char *name) {
    if (x1 == NULL || x2 == NULL) {
        fprintf(stderr, "%s: NULL array argument\n", name);
        return NULL;
    }
    if (x1->dtype == COMPLEX64 || x1->dtype == COMPLEX128 ||
        x2->dtype == COMPLEX64 || x2->dtype == COMPLEX128) {
        fprintf(stderr, "%s: complex numbers not supported\n", name);
        return NULL;
    }
    int out_ndim;
    int *out_shape = broadcast_shapes(2, (Array*[]){x1, x2}, &out_ndim);
    if (out_shape == NULL) { fprintf(stderr, "%s: broadcast failed\n", name); return NULL; }
    Array *result;
    if (out_ndim == 0) {
        result = (Array*)malloc(sizeof(Array));
        if (!result) { free(out_shape); return NULL; }
        result->data = malloc(sizeof(double));
        if (!result->data) { free(result); free(out_shape); return NULL; }
        result->dtype = FLOAT64;
        result->shape = NULL;
        result->ndim = 0;
        result->size = 1;
        result->strides = NULL;
        result->is_view = 0;
    } else {
        result = create_array(out_shape, out_ndim, FLOAT64);
        if (!result) { free(out_shape); return NULL; }
    }
    free(out_shape);
    int *strides1 = NULL, *strides2 = NULL, *res_strides = NULL;
    if (x1->ndim > 0) { strides1 = malloc(x1->ndim * sizeof(int)); compute_strides(x1->shape, x1->ndim, strides1); }
    if (x2->ndim > 0) { strides2 = malloc(x2->ndim * sizeof(int)); compute_strides(x2->shape, x2->ndim, strides2); }
    if (result->ndim > 0) { res_strides = malloc(result->ndim * sizeof(int)); compute_strides(result->shape, result->ndim, res_strides); }
    size_t sz1 = dtype_size(x1->dtype), sz2 = dtype_size(x2->dtype);
    char *data1 = (char*)x1->data, *data2 = (char*)x2->data;
    double *res_data = (double*)result->data;
    int *indices = NULL;
    if (result->ndim > 0) {
        indices = calloc(result->ndim, sizeof(int));
        if (!indices) { free(strides1); free(strides2); free(res_strides); free_array(result); return NULL; }
    }
    for (int flat = 0; flat < result->size; ++flat) {
        if (result->ndim > 0) {
            int tmp = flat;
            for (int i = result->ndim - 1; i >= 0; --i) { indices[i] = tmp % result->shape[i]; tmp /= result->shape[i]; }
        }
        int off1 = 0, off2 = 0;
        if (x1->ndim > 0 && result->ndim > 0) {
            int dim_offset = result->ndim - x1->ndim;
            for (int i = 0; i < x1->ndim; ++i) {
                int coord = indices[dim_offset + i];
                if (x1->shape[i] == 1) coord = 0;
                off1 += coord * strides1[i];
            }
        }
        if (x2->ndim > 0 && result->ndim > 0) {
            int dim_offset = result->ndim - x2->ndim;
            for (int i = 0; i < x2->ndim; ++i) {
                int coord = indices[dim_offset + i];
                if (x2->shape[i] == 1) coord = 0;
                off2 += coord * strides2[i];
            }
        }
        void *p1 = data1 + off1 * sz1, *p2 = data2 + off2 * sz2;
        double v1 = 0.0, v2 = 0.0;
        switch (x1->dtype) {
            case BOOL:   v1 = *(uint8_t*)p1; break;
            case INT8:   v1 = *(int8_t*)p1; break;
            case INT16:  v1 = *(int16_t*)p1; break;
            case INT32:  v1 = *(int32_t*)p1; break;
            case INT64:  v1 = *(int64_t*)p1; break;
            case UINT8:  v1 = *(uint8_t*)p1; break;
            case UINT16: v1 = *(uint16_t*)p1; break;
            case UINT32: v1 = *(uint32_t*)p1; break;
            case UINT64: v1 = *(uint64_t*)p1; break;
            case FLOAT32: v1 = *(float*)p1; break;
            case FLOAT64: v1 = *(double*)p1; break;
            default: break;
        }
        switch (x2->dtype) {
            case BOOL:   v2 = *(uint8_t*)p2; break;
            case INT8:   v2 = *(int8_t*)p2; break;
            case INT16:  v2 = *(int16_t*)p2; break;
            case INT32:  v2 = *(int32_t*)p2; break;
            case INT64:  v2 = *(int64_t*)p2; break;
            case UINT8:  v2 = *(uint8_t*)p2; break;
            case UINT16: v2 = *(uint16_t*)p2; break;
            case UINT32: v2 = *(uint32_t*)p2; break;
            case UINT64: v2 = *(uint64_t*)p2; break;
            case FLOAT32: v2 = *(float*)p2; break;
            case FLOAT64: v2 = *(double*)p2; break;
            default: break;
        }
        double res = func(v1, v2);
        if (result->ndim == 0) { res_data[0] = res; }
        else { int off_res = 0; for (int i = 0; i < result->ndim; ++i) off_res += indices[i] * res_strides[i]; res_data[off_res] = res; }
    }
    free(strides1); free(strides2); free(res_strides);
    if (indices) free(indices);
    return result;
}

#endif /* NP_MATH_MACROS_H */
