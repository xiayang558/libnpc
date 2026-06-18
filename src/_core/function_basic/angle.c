#include "array.h"


Array* angle(Array *arr, int deg) {
    if (arr == NULL) {
        utils_error_log("Error", "angle", "NULL array argument");
        return NULL;
    }

    // determineresultnumberbased ontype
    DataType result_dtype;
    switch (arr->dtype) {
        case COMPLEX64:
            result_dtype = FLOAT32;
            break;
        case COMPLEX128:
            result_dtype = FLOAT64;
            break;
        case COMPLEX256:
            result_dtype = FLOAT128;
            break;
        case INT8:
        case INT16:
        case INT32:
        case INT64:
        case UINT8:
        case UINT16:
        case UINT32:
        case UINT64:
        case FLOAT32:
        case FLOAT64:
        case FLOAT128:
            // real numbers: return double precision float
            result_dtype = FLOAT64;
            break;
        default:
            utils_error_log("Error", "angle", "Unsupported data type: %s", dtype_name(arr->dtype));
            return NULL;
    }

    // create result arrays
    Array *result = create_array(arr->shape, arr->ndim, result_dtype);
    if (result == NULL) {
        utils_error_log("Error", "angle", "Failed to create result array");
        return NULL;
    }

    size_t elem_size_src = dtype_size(arr->dtype);
    size_t elem_size_dst = dtype_size(result_dtype);
    char *src = (char*)arr->data;
    char *dst = (char*)result->data;
    double factor = deg ? (180.0 / M_PI) : 1.0;

    switch (arr->dtype) {
        case COMPLEX64: {
            float *res = (float*)dst;
            complex float *zdata = (complex float*)src;
            for (int i = 0; i < arr->size; i++) {
                float rad = cargf(zdata[i]);
                res[i] = rad * factor;
            }
            break;
        }
        case COMPLEX128: {
            double *res = (double*)dst;
            complex double *zdata = (complex double*)src;
            for (int i = 0; i < arr->size; i++) {
                double rad = carg(zdata[i]);
                res[i] = rad * factor;
            }
            break;
        }
        case COMPLEX256: {
            long double *res = (long double*)dst;
            complex long double *zdata = (complex long double*)src;
            for (int i = 0; i < arr->size; i++) {
                long double rad = cargl(zdata[i]);
                res[i] = rad * factor;
            }
            break;
        }
        case INT8: {
            double *res = (double*)dst;
            int8_t *zdata = (int8_t*)src;
            for (int i = 0; i < arr->size; i++) {
                double ang = (zdata[i] >= 0) ? 0.0 : M_PI;
                res[i] = ang * factor;
            }
            break;
        }
        case INT16: {
            double *res = (double*)dst;
            int16_t *zdata = (int16_t*)src;
            for (int i = 0; i < arr->size; i++) {
                double ang = (zdata[i] >= 0) ? 0.0 : M_PI;
                res[i] = ang * factor;
            }
            break;
        }
        case INT32: {
            double *res = (double*)dst;
            int32_t *zdata = (int32_t*)src;
            for (int i = 0; i < arr->size; i++) {
                double ang = (zdata[i] >= 0) ? 0.0 : M_PI;
                res[i] = ang * factor;
            }
            break;
        }
        case INT64: {
            double *res = (double*)dst;
            int64_t *zdata = (int64_t*)src;
            for (int i = 0; i < arr->size; i++) {
                double ang = (zdata[i] >= 0) ? 0.0 : M_PI;
                res[i] = ang * factor;
            }
            break;
        }
        case UINT8: {
            double *res = (double*)dst;
            uint8_t *zdata = (uint8_t*)src;
            for (int i = 0; i < arr->size; i++) {
                res[i] = 0.0;  // unsigned: always return 0
            }
            break;
        }
        case UINT16: {
            double *res = (double*)dst;
            uint16_t *zdata = (uint16_t*)src;
            for (int i = 0; i < arr->size; i++) {
                res[i] = 0.0;
            }
            break;
        }
        case UINT32: {
            double *res = (double*)dst;
            uint32_t *zdata = (uint32_t*)src;
            for (int i = 0; i < arr->size; i++) {
                res[i] = 0.0;
            }
            break;
        }
        case UINT64: {
            double *res = (double*)dst;
            uint64_t *zdata = (uint64_t*)src;
            for (int i = 0; i < arr->size; i++) {
                res[i] = 0.0;
            }
            break;
        }
        case FLOAT32: {
            double *res = (double*)dst;
            float *zdata = (float*)src;
            for (int i = 0; i < arr->size; i++) {
                double ang = (zdata[i] >= 0.0f) ? 0.0 : M_PI;
                res[i] = ang * factor;
            }
            break;
        }
        case FLOAT64: {
            double *res = (double*)dst;
            double *zdata = (double*)src;
            for (int i = 0; i < arr->size; i++) {
                double ang = (zdata[i] >= 0.0) ? 0.0 : M_PI;
                res[i] = ang * factor;
            }
            break;
        }
        case FLOAT128: {
            double *res = (double*)dst;
            long double *zdata = (long double*)src;
            for (int i = 0; i < arr->size; i++) {
                double ang = (zdata[i] >= 0.0L) ? 0.0 : M_PI;
                res[i] = ang * factor;
            }
            break;
        }
        default:
            utils_error_log("Error", "angle", "Unsupported data type: %s", dtype_name(arr->dtype));
            free_array(result);
            return NULL;
    }

    return result;
}