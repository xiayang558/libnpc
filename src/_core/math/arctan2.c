#include "np_math_macros.h"

Array* arctan2(Array *arr1, Array *arr2) {
    if (arr1 == NULL || arr2 == NULL) {
        fprintf(stderr, "arctan2: NULL input\n");
        return NULL;
    }
    if (arr1->dtype == COMPLEX64 || arr1->dtype == COMPLEX128 || arr1->dtype == COMPLEX256 ||
        arr2->dtype == COMPLEX64 || arr2->dtype == COMPLEX128 || arr2->dtype == COMPLEX256) {
        fprintf(stderr, "arctan2: complex not supported\n");
        return NULL;
    }
    Array *arrays[2] = {arr1, arr2};
    int out_ndim;
    int *out_shape = broadcast_shapes(2, arrays, &out_ndim);
    if (out_shape == NULL) return NULL;
    DataType out_dtype = (arr1->dtype == FLOAT32 || arr2->dtype == FLOAT32) ? FLOAT32 : FLOAT64;
    Array *res = create_array(out_shape, out_ndim, out_dtype);
    free(out_shape);
    if (res == NULL) return NULL;
    int *y_strides = malloc(arr1->ndim * sizeof(int));
    int *x_strides = malloc(arr2->ndim * sizeof(int));
    int *res_strides = malloc(res->ndim * sizeof(int));
    if (!y_strides || !x_strides || !res_strides) {
        free(y_strides); free(x_strides); free(res_strides);
        free_array(res); return NULL;
    }
    compute_strides(arr1->shape, arr1->ndim, y_strides);
    compute_strides(arr2->shape, arr2->ndim, x_strides);
    compute_strides(res->shape, res->ndim, res_strides);
    size_t y_elem = dtype_size(arr1->dtype), x_elem = dtype_size(arr2->dtype), out_elem = dtype_size(out_dtype);
    char *y_data = (char*)arr1->data, *x_data = (char*)arr2->data, *out_data = (char*)res->data;
    int *indices = calloc(res->ndim, sizeof(int));
    if (!indices) { free(y_strides); free(x_strides); free(res_strides); free_array(res); return NULL; }
    for (int flat = 0; flat < res->size; flat++) {
        int temp = flat;
        for (int i = res->ndim - 1; i >= 0; i--) { indices[i] = temp % res->shape[i]; temp /= res->shape[i]; }
        int y_offset = 0, x_offset = 0;
        for (int i = 0; i < res->ndim; i++) {
            if (arr1->ndim > 0) { int y_dim_idx = i - (res->ndim - arr1->ndim);
                if (y_dim_idx >= 0) { int dim = arr1->shape[y_dim_idx], coord = (dim == 1) ? 0 : indices[i]; y_offset += coord * y_strides[y_dim_idx]; } }
            if (arr2->ndim > 0) { int x_dim_idx = i - (res->ndim - arr2->ndim);
                if (x_dim_idx >= 0) { int dim = arr2->shape[x_dim_idx], coord = (dim == 1) ? 0 : indices[i]; x_offset += coord * x_strides[x_dim_idx]; } }
        }
        double y_val = 0.0, x_val = 0.0;
        switch (arr1->dtype) {
            case INT8: y_val = *(int8_t*)(y_data + y_offset*y_elem); break; case INT16: y_val = *(int16_t*)(y_data + y_offset*y_elem); break;
            case INT32: y_val = *(int32_t*)(y_data + y_offset*y_elem); break; case INT64: y_val = *(int64_t*)(y_data + y_offset*y_elem); break;
            case UINT8: y_val = *(uint8_t*)(y_data + y_offset*y_elem); break; case UINT16: y_val = *(uint16_t*)(y_data + y_offset*y_elem); break;
            case UINT32: y_val = *(uint32_t*)(y_data + y_offset*y_elem); break; case UINT64: y_val = *(uint64_t*)(y_data + y_offset*y_elem); break;
            case FLOAT32: y_val = *(float*)(y_data + y_offset*y_elem); break; case FLOAT64: y_val = *(double*)(y_data + y_offset*y_elem); break;
            default: break;
        }
        switch (arr2->dtype) {
            case INT8: x_val = *(int8_t*)(x_data + x_offset*x_elem); break; case INT16: x_val = *(int16_t*)(x_data + x_offset*x_elem); break;
            case INT32: x_val = *(int32_t*)(x_data + x_offset*x_elem); break; case INT64: x_val = *(int64_t*)(x_data + x_offset*x_elem); break;
            case UINT8: x_val = *(uint8_t*)(x_data + x_offset*x_elem); break; case UINT16: x_val = *(uint16_t*)(x_data + x_offset*x_elem); break;
            case UINT32: x_val = *(uint32_t*)(x_data + x_offset*x_elem); break; case UINT64: x_val = *(uint64_t*)(x_data + x_offset*x_elem); break;
            case FLOAT32: x_val = *(float*)(x_data + x_offset*x_elem); break; case FLOAT64: x_val = *(double*)(x_data + x_offset*x_elem); break;
            default: break;
        }
        double res_val = atan2(y_val, x_val);
        void *out_ptr = out_data + flat * out_elem;
        if (out_dtype == FLOAT32) *(float*)out_ptr = (float)res_val;
        else *(double*)out_ptr = res_val;
    }
    free(y_strides); free(x_strides); free(res_strides); free(indices);
    return res;
}
