#include "array.h"
#include "def.h"
#include "utils.h"

Array* sum(Array *arr, int axis, int keepdims) {
    NP_UTILS_CHECK_NULL(arr, NULL);
    
    if (axis < -1 || axis >= arr->ndim) {
        NP_UTILS_ERROR_LOG("Error", "Invalid axis %d for array with %d dimensions", 
                          axis, arr->ndim);
        return NULL;
    }
    
    // if axis is -1, compute all elements sum
    if (axis == -1) {
        // compute total sum, return scalar
        int64_t sum_int64 = 0;
        uint64_t sum_uint64 = 0;
        double sum_double = 0.0;
        complex double sum_complex128 = 0.0 + 0.0*I;
        DataType result_type = UNKNOWN;
        
        switch (arr->dtype) {
            // signed integer type
            NP_INT_SUM(INT8, int8_t, &sum_int64)
            NP_INT_SUM(INT16, int16_t, &sum_int64)
            NP_INT_SUM(INT32, int32_t, &sum_int64)
            NP_INT_SUM(INT64, int64_t, &sum_int64)
            
            // unsigned integer type
            NP_UINT_SUM(UINT8, uint8_t, &sum_uint64)
            NP_UINT_SUM(UINT16, uint16_t, &sum_uint64)
            NP_UINT_SUM(UINT32, uint32_t, &sum_uint64)
            NP_UINT_SUM(UINT64, uint64_t, &sum_uint64)
            
            // floating point type
            NP_FLOAT_SUM(FLOAT32, float, &sum_double)
            NP_FLOAT_SUM(FLOAT64, double, &sum_double)
            
            // complex type
            NP_COMPLEX_SUM(COMPLEX64, complex float, &sum_complex128, COMPLEX128)
            NP_COMPLEX_SUM(COMPLEX128, complex double, &sum_complex128, COMPLEX128)
            NP_COMPLEX_SUM(COMPLEX256, complex long double, &sum_complex128, COMPLEX256)
            
            default:
                NP_UTILS_ERROR_LOG("Error", "Unsupported data type (%s)", dtype_name(arr->dtype));
                return NULL;
        }
        
        int ndim;
        // create result array (scalar)
        if (keepdims) {
            ndim = arr->ndim;
        } else {
            ndim = 1;
        }

        int *shape = malloc(ndim * sizeof(int));
        // int *shape = keepdims ? malloc(arr->ndim * sizeof(int)) : 1;
        // int ndim = keepdims ? arr->ndim : 1;
        
        // if (keepdims) {
        // }
        for (int i = 0; i < ndim; i++) {
            shape[i] = 1;
        }
        
        // printf("shape: (%d, %d, %d), ndim: %d, result_type: %s\n", 
        //        shape[0], shape[1], shape[2], ndim, dtype_name(result_type));
        Array *result = create_array(shape, ndim, result_type);
        if (shape) free(shape);
        
        if (result == NULL) {
            NP_UTILS_ERROR_LOG("Error", "Failed to create result array");
            return NULL;
        }
        
        // set result value
        switch (result_type) {
            case INT64:
                // *(int64_t*)result->data = sum_int64;
                memcpy(result->data, &sum_int64, sizeof(int64_t));
                break;
            case UINT64:
                *(uint64_t*)result->data = sum_uint64;
                break;
            case FLOAT64:
                *(double*)result->data = sum_double;
                break;
            case COMPLEX128:
                *(complex double*)result->data = sum_complex128;
                break;
            case COMPLEX256:
                *(complex long double*)result->data = sum_complex128;
                break;
            default:
                break;
        }
        
        return result;
    }
    
    // sum along a specific axis (simplified implementation)
    // compute output shape
    int out_ndim = keepdims ? arr->ndim : arr->ndim - 1;
    int *out_shape = malloc(out_ndim * sizeof(int));
    if (out_shape == NULL) {
        NP_UTILS_ERROR_LOG("Error", "Memory allocation failed");
        return NULL;
    }
    int out_idx = 0;
    for (int i = 0; i < arr->ndim; i++) {
        if (i == axis) {
            if (keepdims) out_shape[out_idx++] = 1;
            // else skip this dimension
        } else {
            out_shape[out_idx++] = arr->shape[i];
        }
    }

    // determine output type based on input type (integer promotes to INT64, floating point promotes to FLOAT64, complex promotes to COMPLEX128)
    DataType result_type;
    switch (arr->dtype) {
        case INT8: case INT16: case INT32: case INT64:
        case UINT8: case UINT16: case UINT32: case UINT64:
            result_type = INT64;
            break;
        case FLOAT32: case FLOAT64:
            result_type = FLOAT64;
            break;
        case COMPLEX64: case COMPLEX128: case COMPLEX256:
            result_type = COMPLEX128;
            break;
        default:
            result_type = arr->dtype;
    }

    Array *result = create_array(out_shape, out_ndim, result_type);
    free(out_shape);
    if (result == NULL) {
        NP_UTILS_ERROR_LOG("Error", "Failed to create result array");
        return NULL;
    }

    // compute strides (in elements as units)
    int *in_strides = malloc(arr->ndim * sizeof(int));
    int *out_strides = malloc(result->ndim * sizeof(int));
    if (in_strides == NULL || out_strides == NULL) {
        free(in_strides); free(out_strides);
        free_array(result);
        NP_UTILS_ERROR_LOG("Error", "Memory allocation failed");
        return NULL;
    }
    compute_strides(arr->shape, arr->ndim, in_strides);
    compute_strides(result->shape, result->ndim, out_strides);

    // allocate temporary index array
    int *indices = calloc(result->ndim, sizeof(int));
    if (indices == NULL) {
        free(in_strides); free(out_strides);
        free_array(result);
        NP_UTILS_ERROR_LOG("Error", "Memory allocation failed");
        return NULL;
    }

    size_t in_elem_size = dtype_size(arr->dtype);
    size_t out_elem_size = dtype_size(result_type);
    char *in_data = (char*)arr->data;
    char *out_data = (char*)result->data;

    // iterate over each element of the output array (i.e., combinations of dimensions other than axis)
    int total_out = result->size;
    for (int out_flat = 0; out_flat < total_out; out_flat++) {
        // convert linear index to multi-dimensional index
        int temp = out_flat;
        for (int i = result->ndim - 1; i >= 0; i--) {
            indices[i] = temp % result->shape[i];
            temp /= result->shape[i];
        }

        // compute base offset in input (corresponds to dimensions other than axis)
        int base_offset = 0;
        int out_dim_idx = 0;
        for (int i = 0; i < arr->ndim; i++) {
            if (i == axis) continue;
            int coord = indices[out_dim_idx++];
            base_offset += coord * in_strides[i];
        }

        // accumulate along the axis direction
        int axis_len = arr->shape[axis];
        int64_t sum_int64 = 0;
        uint64_t sum_uint64 = 0;
        double sum_double = 0.0;
        complex double sum_complex128 = 0.0 + 0.0*I;

        switch (arr->dtype) {
            case INT8: {
                for (int k = 0; k < axis_len; k++) {
                    int offset = base_offset + k * in_strides[axis];
                    int8_t val = *(int8_t*)(in_data + offset * in_elem_size);
                    sum_int64 += (int64_t)val;
                }
                break;
            }
            case INT16: {
                for (int k = 0; k < axis_len; k++) {
                    int offset = base_offset + k * in_strides[axis];
                    int16_t val = *(int16_t*)(in_data + offset * in_elem_size);
                    sum_int64 += (int64_t)val;
                }
                break;
            }
            case INT32: {
                for (int k = 0; k < axis_len; k++) {
                    int offset = base_offset + k * in_strides[axis];
                    int32_t val = *(int32_t*)(in_data + offset * in_elem_size);
                    sum_int64 += (int64_t)val;
                }
                break;
            }
            case INT64: {
                for (int k = 0; k < axis_len; k++) {
                    int offset = base_offset + k * in_strides[axis];
                    int64_t val = *(int64_t*)(in_data + offset * in_elem_size);
                    sum_int64 += val;
                }
                break;
            }
            case UINT8: {
                for (int k = 0; k < axis_len; k++) {
                    int offset = base_offset + k * in_strides[axis];
                    uint8_t val = *(uint8_t*)(in_data + offset * in_elem_size);
                    sum_uint64 += (uint64_t)val;
                }
                break;
            }
            case UINT16: {
                for (int k = 0; k < axis_len; k++) {
                    int offset = base_offset + k * in_strides[axis];
                    uint16_t val = *(uint16_t*)(in_data + offset * in_elem_size);
                    sum_uint64 += (uint64_t)val;
                }
                break;
            }
            case UINT32: {
                for (int k = 0; k < axis_len; k++) {
                    int offset = base_offset + k * in_strides[axis];
                    uint32_t val = *(uint32_t*)(in_data + offset * in_elem_size);
                    sum_uint64 += (uint64_t)val;
                }
                break;
            }
            case UINT64: {
                for (int k = 0; k < axis_len; k++) {
                    int offset = base_offset + k * in_strides[axis];
                    uint64_t val = *(uint64_t*)(in_data + offset * in_elem_size);
                    sum_uint64 += val;
                }
                break;
            }
            case FLOAT32: {
                for (int k = 0; k < axis_len; k++) {
                    int offset = base_offset + k * in_strides[axis];
                    float val = *(float*)(in_data + offset * in_elem_size);
                    sum_double += (double)val;
                }
                break;
            }
            case FLOAT64: {
                for (int k = 0; k < axis_len; k++) {
                    int offset = base_offset + k * in_strides[axis];
                    double val = *(double*)(in_data + offset * in_elem_size);
                    sum_double += val;
                }
                break;
            }
            case COMPLEX64: {
                for (int k = 0; k < axis_len; k++) {
                    int offset = base_offset + k * in_strides[axis];
                    complex float val = *(complex float*)(in_data + offset * in_elem_size);
                    sum_complex128 += (complex double)val;
                }
                break;
            }
            case COMPLEX128: {
                for (int k = 0; k < axis_len; k++) {
                    int offset = base_offset + k * in_strides[axis];
                    complex double val = *(complex double*)(in_data + offset * in_elem_size);
                    sum_complex128 += val;
                }
                break;
            }
            case COMPLEX256: {
                for (int k = 0; k < axis_len; k++) {
                    int offset = base_offset + k * in_strides[axis];
                    complex long double val = *(complex long double*)(in_data + offset * in_elem_size);
                    sum_complex128 += (complex double)val;
                }
                break;
            }
            default:
                NP_UTILS_ERROR_LOG("Error", "Unsupported data type");
                free(indices); free(in_strides); free(out_strides);
                free_array(result);
                return NULL;
        }

        // write to output array
        int out_offset = 0;
        for (int i = 0; i < result->ndim; i++) {
            out_offset += indices[i] * out_strides[i];
        }
        void *out_ptr = out_data + out_offset * out_elem_size;
        switch (result_type) {
            case INT64:
                *(int64_t*)out_ptr = sum_int64;
                break;
            case UINT64:
                *(uint64_t*)out_ptr = sum_uint64;
                break;
            case FLOAT64:
                *(double*)out_ptr = sum_double;
                break;
            case COMPLEX128:
                *(complex double*)out_ptr = sum_complex128;
                break;
            default:
                break;
        }
    }

    free(indices);
    free(in_strides);
    free(out_strides);
    return result;
}