#include <stdlib.h>
#include <string.h>
#include <complex.h>
#include <stdint.h>
#include <math.h>
#include "array.h"
Array* greater(Array *arr1, Array *arr2) {
    if (arr1 == NULL || arr2 == NULL) {
        NP_UTILS_ERROR_LOG("Error", "NULL array argument");
        return NULL;
    }
    // require same data type (could be extended to type promotion)
    if (arr1->dtype != arr2->dtype) {
        NP_UTILS_ERROR_LOG("Error", "Data type mismatch (arr1: %s, arr2: %s)",
                           dtype_name(arr1->dtype), dtype_name(arr2->dtype));
        return NULL;
    }

    // determine output shape (broadcast rules)
    int out_ndim;
    int *out_shape = NULL;
    if (arr1->ndim == 0 && arr2->ndim == 0) {
        out_ndim = 0;
        out_shape = NULL;
    } else if (arr1->ndim == 0) {
        out_ndim = arr2->ndim;
        out_shape = malloc(out_ndim * sizeof(int));
        memcpy(out_shape, arr2->shape, out_ndim * sizeof(int));
    } else if (arr2->ndim == 0) {
        out_ndim = arr1->ndim;
        out_shape = malloc(out_ndim * sizeof(int));
        memcpy(out_shape, arr1->shape, out_ndim * sizeof(int));
    } else {
        // both are multi-dimensional arrays, require same shape (broadcast not yet supported)
        if (!compare_shapes(arr1, arr2)) {
            NP_UTILS_ERROR_LOG("Error", "Shape mismatch for non-scalar arrays");
            return NULL;
        }
        out_ndim = arr1->ndim;
        out_shape = malloc(out_ndim * sizeof(int));
        memcpy(out_shape, arr1->shape, out_ndim * sizeof(int));
    }

    Array *result = create_array(out_shape, out_ndim, BOOL);
    if (out_shape) free(out_shape);
    if (result == NULL) {
        NP_UTILS_ERROR_LOG("Error", "Failed to create result array");
        return NULL;
    }

    uint8_t *res_data = (uint8_t*)result->data;
    size_t elem_size = dtype_size(arr1->dtype);
    char *data1 = (char*)arr1->data;
    char *data2 = (char*)arr2->data;

    // handle based on shape situation
    if (arr1->ndim == 0 && arr2->ndim == 0) {
        // both are scalars
        switch (arr1->dtype) {
            case INT8:   res_data[0] = (*(int8_t*)data1 > *(int8_t*)data2) ? 1 : 0; break;
            case INT16:  res_data[0] = (*(int16_t*)data1 > *(int16_t*)data2) ? 1 : 0; break;
            case INT32:  res_data[0] = (*(int32_t*)data1 > *(int32_t*)data2) ? 1 : 0; break;
            case INT64:  res_data[0] = (*(int64_t*)data1 > *(int64_t*)data2) ? 1 : 0; break;
            case UINT8:  res_data[0] = (*(uint8_t*)data1 > *(uint8_t*)data2) ? 1 : 0; break;
            case UINT16: res_data[0] = (*(uint16_t*)data1 > *(uint16_t*)data2) ? 1 : 0; break;
            case UINT32: res_data[0] = (*(uint32_t*)data1 > *(uint32_t*)data2) ? 1 : 0; break;
            case UINT64: res_data[0] = (*(uint64_t*)data1 > *(uint64_t*)data2) ? 1 : 0; break;
            case FLOAT32: res_data[0] = (*(float*)data1 > *(float*)data2) ? 1 : 0; break;
            case FLOAT64: res_data[0] = (*(double*)data1 > *(double*)data2) ? 1 : 0; break;
            case COMPLEX64: {
                float a = cabsf(*(complex float*)data1);
                float b = cabsf(*(complex float*)data2);
                res_data[0] = (a > b) ? 1 : 0;
                break;
            }
            case COMPLEX128: {
                double a = cabs(*(complex double*)data1);
                double b = cabs(*(complex double*)data2);
                res_data[0] = (a > b) ? 1 : 0;
                break;
            }
            default:
                NP_UTILS_ERROR_LOG("Error", "Unsupported data type (%s)", dtype_name(arr1->dtype));
                free_array(result);
                return NULL;
        }
    } else if (arr1->ndim == 0) {
        // arr1 is scalar, arr2 is multi-dimensional
        for (int i = 0; i < arr2->size; i++) {
            void *p1 = data1;                     // scalar is fixed
            void *p2 = data2 + i * elem_size;
            int cmp = 0;
            switch (arr1->dtype) {
                case INT8:   cmp = (*(int8_t*)p1 > *(int8_t*)p2); break;
                case INT16:  cmp = (*(int16_t*)p1 > *(int16_t*)p2); break;
                case INT32:  cmp = (*(int32_t*)p1 > *(int32_t*)p2); break;
                case INT64:  cmp = (*(int64_t*)p1 > *(int64_t*)p2); break;
                case UINT8:  cmp = (*(uint8_t*)p1 > *(uint8_t*)p2); break;
                case UINT16: cmp = (*(uint16_t*)p1 > *(uint16_t*)p2); break;
                case UINT32: cmp = (*(uint32_t*)p1 > *(uint32_t*)p2); break;
                case UINT64: cmp = (*(uint64_t*)p1 > *(uint64_t*)p2); break;
                case FLOAT32: cmp = (*(float*)p1 > *(float*)p2); break;
                case FLOAT64: cmp = (*(double*)p1 > *(double*)p2); break;
                case COMPLEX64: {
                    float a = cabsf(*(complex float*)p1);
                    float b = cabsf(*(complex float*)p2);
                    cmp = (a > b);
                    break;
                }
                case COMPLEX128: {
                    double a = cabs(*(complex double*)p1);
                    double b = cabs(*(complex double*)p2);
                    cmp = (a > b);
                    break;
                }
                default: cmp = 0; break;
            }
            res_data[i] = cmp ? 1 : 0;
        }
    } else if (arr2->ndim == 0) {
        // arr2 is scalar, arr1 is multi-dimensional
        for (int i = 0; i < arr1->size; i++) {
            void *p1 = data1 + i * elem_size;
            void *p2 = data2;                     // scalar is fixed
            int cmp = 0;
            switch (arr1->dtype) {
                case INT8:   cmp = (*(int8_t*)p1 > *(int8_t*)p2); break;
                case INT16:  cmp = (*(int16_t*)p1 > *(int16_t*)p2); break;
                case INT32:  cmp = (*(int32_t*)p1 > *(int32_t*)p2); break;
                case INT64:  cmp = (*(int64_t*)p1 > *(int64_t*)p2); break;
                case UINT8:  cmp = (*(uint8_t*)p1 > *(uint8_t*)p2); break;
                case UINT16: cmp = (*(uint16_t*)p1 > *(uint16_t*)p2); break;
                case UINT32: cmp = (*(uint32_t*)p1 > *(uint32_t*)p2); break;
                case UINT64: cmp = (*(uint64_t*)p1 > *(uint64_t*)p2); break;
                case FLOAT32: cmp = (*(float*)p1 > *(float*)p2); break;
                case FLOAT64: cmp = (*(double*)p1 > *(double*)p2); break;
                case COMPLEX64: {
                    float a = cabsf(*(complex float*)p1);
                    float b = cabsf(*(complex float*)p2);
                    cmp = (a > b);
                    break;
                }
                case COMPLEX128: {
                    double a = cabs(*(complex double*)p1);
                    double b = cabs(*(complex double*)p2);
                    cmp = (a > b);
                    break;
                }
                default: cmp = 0; break;
            }
            res_data[i] = cmp ? 1 : 0;
        }
    } else {
        // both are multi-dimensional arrays with matching shapes
        for (int i = 0; i < arr1->size; i++) {
            void *p1 = data1 + i * elem_size;
            void *p2 = data2 + i * elem_size;
            int cmp = 0;
            switch (arr1->dtype) {
                case INT8:   cmp = (*(int8_t*)p1 > *(int8_t*)p2); break;
                case INT16:  cmp = (*(int16_t*)p1 > *(int16_t*)p2); break;
                case INT32:  cmp = (*(int32_t*)p1 > *(int32_t*)p2); break;
                case INT64:  cmp = (*(int64_t*)p1 > *(int64_t*)p2); break;
                case UINT8:  cmp = (*(uint8_t*)p1 > *(uint8_t*)p2); break;
                case UINT16: cmp = (*(uint16_t*)p1 > *(uint16_t*)p2); break;
                case UINT32: cmp = (*(uint32_t*)p1 > *(uint32_t*)p2); break;
                case UINT64: cmp = (*(uint64_t*)p1 > *(uint64_t*)p2); break;
                case FLOAT32: cmp = (*(float*)p1 > *(float*)p2); break;
                case FLOAT64: cmp = (*(double*)p1 > *(double*)p2); break;
                case COMPLEX64: {
                    float a = cabsf(*(complex float*)p1);
                    float b = cabsf(*(complex float*)p2);
                    cmp = (a > b);
                    break;
                }
                case COMPLEX128: {
                    double a = cabs(*(complex double*)p1);
                    double b = cabs(*(complex double*)p2);
                    cmp = (a > b);
                    break;
                }
                default: cmp = 0; break;
            }
            res_data[i] = cmp ? 1 : 0;
        }
    }

    return result;
}
