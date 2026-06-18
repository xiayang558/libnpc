#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <complex.h>
#include <stdint.h>
#include "array.h"
Array* equal(Array *arr1, Array *arr2) {
    // input check
    if (arr1 == NULL || arr2 == NULL) {
        fprintf(stderr, "[Error] equal: NULL array argument\n");
        return NULL;
    }
    if (arr1->dtype != arr2->dtype) {
        fprintf(stderr, "[Error] equal: Data type mismatch (arr1: %s, arr2: %s)\n",
                dtype_name(arr1->dtype), dtype_name(arr2->dtype));
        return NULL;
    }

    // Determine output shape (broadcast rules)
    int out_ndim;
    int *out_shape = NULL;

    if (arr1->ndim == 0 && arr2->ndim == 0) {
        // Both scalars -> output scalar
        out_ndim = 0;
        out_shape = NULL;
    } else if (arr1->ndim == 0) {
        // arr1 is scalar, broadcast to arr2 shape
        out_ndim = arr2->ndim;
        out_shape = malloc(out_ndim * sizeof(int));
        if (out_shape == NULL) {
            fprintf(stderr, "[Error] equal: Memory allocation failed\n");
            return NULL;
        }
        memcpy(out_shape, arr2->shape, out_ndim * sizeof(int));
    } else if (arr2->ndim == 0) {
        // arr2 is scalar, broadcast to arr1 shape
        out_ndim = arr1->ndim;
        out_shape = malloc(out_ndim * sizeof(int));
        if (out_shape == NULL) {
            fprintf(stderr, "[Error] equal: Memory allocation failed\n");
            return NULL;
        }
        memcpy(out_shape, arr1->shape, out_ndim * sizeof(int));
    } else {
        // Both are multi-dimensional, require same shape (full broadcast not yet implemented)
        if (arr1->ndim != arr2->ndim) {
            fprintf(stderr, "[Error] equal: Dimension mismatch (%d vs %d)\n",
                    arr1->ndim, arr2->ndim);
            return NULL;
        }
        for (int i = 0; i < arr1->ndim; i++) {
            if (arr1->shape[i] != arr2->shape[i]) {
                fprintf(stderr, "[Error] equal: Shape mismatch at dim %d (%d vs %d)\n",
                        i, arr1->shape[i], arr2->shape[i]);
                return NULL;
            }
        }
        out_ndim = arr1->ndim;
        out_shape = malloc(out_ndim * sizeof(int));
        if (out_shape == NULL) {
            fprintf(stderr, "[Error] equal: Memory allocation failed\n");
            return NULL;
        }
        memcpy(out_shape, arr1->shape, out_ndim * sizeof(int));
    }

    // Create result array
    // Add scalar support
    Array *result;
    if (out_ndim == 0) {
        int value = FALSE; // Default: not equal
        result = create_scalar_array(&value, BOOL);
        if (result == NULL) {
            fprintf(stderr, "[Error] equal: Failed to create result scalar\n");
            return NULL;
        } 
    } else {
        result = create_array(out_shape, out_ndim, BOOL);
        if (out_shape) free(out_shape);
        if (result == NULL) {
            fprintf(stderr, "[Error] equal: Failed to create result array\n");
            return NULL;
        }
    }
    // Array *result = create_array(out_shape, out_ndim, BOOL);
    // if (out_shape != NULL) free(out_shape);
    // if (result == NULL) {
    //     fprintf(stderr, "[Error] equal: Failed to create result array\n");
    //     return NULL;
    // }

    uint8_t *res_data = (uint8_t*)result->data;
    size_t elem_size = dtype_size(arr1->dtype);
    char *data1 = (char*)arr1->data;
    char *data2 = (char*)arr2->data;

    // Perform comparison based on shape scenario
    if (arr1->ndim == 0 && arr2->ndim == 0) {
        // Both scalars
        void *p1 = data1;
        void *p2 = data2;
        int eq = 0;
        switch (arr1->dtype) {
            case BOOL:    eq = (*(uint8_t*)p1 == *(uint8_t*)p2); break;
            case INT8:    eq = (*(int8_t*)p1 == *(int8_t*)p2); break;
            case INT16:   eq = (*(int16_t*)p1 == *(int16_t*)p2); break;
            case INT32:   eq = (*(int32_t*)p1 == *(int32_t*)p2); break;
            case INT64:   eq = (*(int64_t*)p1 == *(int64_t*)p2); break;
            case UINT8:   eq = (*(uint8_t*)p1 == *(uint8_t*)p2); break;
            case UINT16:  eq = (*(uint16_t*)p1 == *(uint16_t*)p2); break;
            case UINT32:  eq = (*(uint32_t*)p1 == *(uint32_t*)p2); break;
            case UINT64:  eq = (*(uint64_t*)p1 == *(uint64_t*)p2); break;
            case FLOAT32: {
                float a = *(float*)p1, b = *(float*)p2;
                eq = (fabsf(a - b) < M_EPSILON_F32);
                break;
            }
            case FLOAT64: {
                double a = *(double*)p1, b = *(double*)p2;
                eq = (fabs(a - b) < M_EPSILON_F64);
                break;
            }
            case COMPLEX64: {
                complex float a = *(complex float*)p1, b = *(complex float*)p2;
                float dr = fabsf(crealf(a) - crealf(b));
                float di = fabsf(cimagf(a) - cimagf(b));
                eq = (dr < M_EPSILON_F32 && di < M_EPSILON_F32);
                break;
            }
            case COMPLEX128: {
                complex double a = *(complex double*)p1, b = *(complex double*)p2;
                double dr = fabs(creal(a) - creal(b));
                double di = fabs(cimag(a) - cimag(b));
                eq = (dr < M_EPSILON_F64 && di < M_EPSILON_F64);
                break;
            }
            default:
                fprintf(stderr, "[Error] equal: Unsupported data type (%s)\n",
                        dtype_name(arr1->dtype));
                free_array(result);
                return NULL;
        }
        res_data[0] = eq ? 1 : 0;
    } else if (arr1->ndim == 0) {
        // arr1 is scalar, arr2 is multi-dimensional
        void *scalar = data1;
        for (int i = 0; i < arr2->size; i++) {
            void *p = data2 + i * elem_size;
            int eq = 0;
            switch (arr1->dtype) {
                case BOOL:    eq = (*(uint8_t*)scalar == *(uint8_t*)p); break;
                case INT8:    eq = (*(int8_t*)scalar == *(int8_t*)p); break;
                case INT16:   eq = (*(int16_t*)scalar == *(int16_t*)p); break;
                case INT32:   eq = (*(int32_t*)scalar == *(int32_t*)p); break;
                case INT64:   eq = (*(int64_t*)scalar == *(int64_t*)p); break;
                case UINT8:   eq = (*(uint8_t*)scalar == *(uint8_t*)p); break;
                case UINT16:  eq = (*(uint16_t*)scalar == *(uint16_t*)p); break;
                case UINT32:  eq = (*(uint32_t*)scalar == *(uint32_t*)p); break;
                case UINT64:  eq = (*(uint64_t*)scalar == *(uint64_t*)p); break;
                case FLOAT32: {
                    float a = *(float*)scalar, b = *(float*)p;
                    eq = (fabsf(a - b) < M_EPSILON_F32);
                    break;
                }
                case FLOAT64: {
                    double a = *(double*)scalar, b = *(double*)p;
                    eq = (fabs(a - b) < M_EPSILON_F64);
                    break;
                }
                case COMPLEX64: {
                    complex float a = *(complex float*)scalar, b = *(complex float*)p;
                    float dr = fabsf(crealf(a) - crealf(b));
                    float di = fabsf(cimagf(a) - cimagf(b));
                    eq = (dr < M_EPSILON_F32 && di < M_EPSILON_F32);
                    break;
                }
                case COMPLEX128: {
                    complex double a = *(complex double*)scalar, b = *(complex double*)p;
                    double dr = fabs(creal(a) - creal(b));
                    double di = fabs(cimag(a) - cimag(b));
                    eq = (dr < M_EPSILON_F64 && di < M_EPSILON_F64);
                    break;
                }
                default:
                    fprintf(stderr, "[Error] equal: Unsupported data type (%s)\n",
                            dtype_name(arr1->dtype));
                    free_array(result);
                    return NULL;
            }
            res_data[i] = eq ? 1 : 0;
        }
    } else if (arr2->ndim == 0) {
        // arr2 is scalar, arr1 is multi-dimensional
        void *scalar = data2;
        for (int i = 0; i < arr1->size; i++) {
            void *p = data1 + i * elem_size;
            int eq = 0;
            switch (arr1->dtype) {
                case BOOL:    eq = (*(uint8_t*)p == *(uint8_t*)scalar); break; // BOOL is actually stored as uint8_t
                case INT8:    eq = (*(int8_t*)p == *(int8_t*)scalar); break;
                case INT16:   eq = (*(int16_t*)p == *(int16_t*)scalar); break;
                case INT32:   eq = (*(int32_t*)p == *(int32_t*)scalar); break;
                case INT64:   eq = (*(int64_t*)p == *(int64_t*)scalar); break;
                case UINT8:   eq = (*(uint8_t*)p == *(uint8_t*)scalar); break;
                case UINT16:  eq = (*(uint16_t*)p == *(uint16_t*)scalar); break;
                case UINT32:  eq = (*(uint32_t*)p == *(uint32_t*)scalar); break;
                case UINT64:  eq = (*(uint64_t*)p == *(uint64_t*)scalar); break;
                case FLOAT32: {
                    float a = *(float*)p, b = *(float*)scalar;
                    eq = (fabsf(a - b) < M_EPSILON_F32);
                    break;
                }
                case FLOAT64: {
                    double a = *(double*)p, b = *(double*)scalar;
                    eq = (fabs(a - b) < M_EPSILON_F64);
                    break;
                }
                case COMPLEX64: {
                    complex float a = *(complex float*)p, b = *(complex float*)scalar;
                    float dr = fabsf(crealf(a) - crealf(b));
                    float di = fabsf(cimagf(a) - cimagf(b));
                    eq = (dr < M_EPSILON_F32 && di < M_EPSILON_F32);
                    break;
                }
                case COMPLEX128: {
                    complex double a = *(complex double*)p, b = *(complex double*)scalar;
                    double dr = fabs(creal(a) - creal(b));
                    double di = fabs(cimag(a) - cimag(b));
                    eq = (dr < M_EPSILON_F64 && di < M_EPSILON_F64);
                    break;
                }
                default:
                    fprintf(stderr, "[Error] equal: Unsupported data type (%s)\n",
                            dtype_name(arr1->dtype));
                    free_array(result);
                    return NULL;
            }
            res_data[i] = eq ? 1 : 0;
        }
    } else {
        // Both multi-dimensional with same shape
        for (int i = 0; i < arr1->size; i++) {
            void *p1 = data1 + i * elem_size;
            void *p2 = data2 + i * elem_size;
            int eq = 0;
            switch (arr1->dtype) {
                case BOOL:    eq = (*(uint8_t*)p1 == *(uint8_t*)p2); break; // BOOL is actually stored as uint8_t
                case INT8:    eq = (*(int8_t*)p1 == *(int8_t*)p2); break;
                case INT16:   eq = (*(int16_t*)p1 == *(int16_t*)p2); break;
                case INT32:   eq = (*(int32_t*)p1 == *(int32_t*)p2); break;
                case INT64:   eq = (*(int64_t*)p1 == *(int64_t*)p2); break;
                case UINT8:   eq = (*(uint8_t*)p1 == *(uint8_t*)p2); break;
                case UINT16:  eq = (*(uint16_t*)p1 == *(uint16_t*)p2); break;
                case UINT32:  eq = (*(uint32_t*)p1 == *(uint32_t*)p2); break;
                case UINT64:  eq = (*(uint64_t*)p1 == *(uint64_t*)p2); break;
                case FLOAT32: {
                    float a = *(float*)p1, b = *(float*)p2;
                    eq = (fabsf(a - b) < M_EPSILON_F32);
                    break;
                }
                case FLOAT64: {
                    double a = *(double*)p1, b = *(double*)p2;
                    eq = (fabs(a - b) < M_EPSILON_F64);
                    break;
                }
                case COMPLEX64: {
                    complex float a = *(complex float*)p1, b = *(complex float*)p2;
                    float dr = fabsf(crealf(a) - crealf(b));
                    float di = fabsf(cimagf(a) - cimagf(b));
                    eq = (dr < M_EPSILON_F32 && di < M_EPSILON_F32);
                    break;
                }
                case COMPLEX128: {
                    complex double a = *(complex double*)p1, b = *(complex double*)p2;
                    double dr = fabs(creal(a) - creal(b));
                    double di = fabs(cimag(a) - cimag(b));
                    eq = (dr < M_EPSILON_F64 && di < M_EPSILON_F64);
                    break;
                }
                default:
                    fprintf(stderr, "[Error] equal: Unsupported data type (%s)\n",
                            dtype_name(arr1->dtype));
                    free_array(result);
                    return NULL;
            }
            res_data[i] = eq ? 1 : 0;
        }
    }

    return result;
}

// array concatenation
