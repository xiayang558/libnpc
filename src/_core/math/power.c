#include "np_math_macros.h"

Array* power(Array *arr1, Array *arr2) {
    if (!arr1 || !arr2) return NULL;
    int out_ndim;
    int *out_shape = broadcast_shapes(2, (Array*[]){arr1, arr2}, &out_ndim);
    if (!out_shape) return NULL;
    int is_complex = (arr1->dtype == COMPLEX64 || arr1->dtype == COMPLEX128 ||
                      arr2->dtype == COMPLEX64 || arr2->dtype == COMPLEX128);
    DataType out_dtype = is_complex ? COMPLEX128 : FLOAT64;
    Array *result;
    if (out_ndim == 0) {
        result = (Array*)malloc(sizeof(Array));
        if (!result) { free(out_shape); return NULL; }
        result->data = malloc(dtype_size(out_dtype));
        if (!result->data) { free(result); free(out_shape); return NULL; }
        result->dtype = out_dtype;
        result->shape = NULL; result->ndim = 0; result->size = 1;
        result->strides = NULL; result->is_view = 0;
    } else {
        result = create_array(out_shape, out_ndim, out_dtype);
        if (!result) { free(out_shape); return NULL; }
    }
    free(out_shape);
    int *strides1 = NULL, *strides2 = NULL, *res_strides = NULL;
    if (arr1->ndim) strides1 = malloc(arr1->ndim * sizeof(int));
    if (arr2->ndim) strides2 = malloc(arr2->ndim * sizeof(int));
    if (result->ndim) res_strides = malloc(result->ndim * sizeof(int));
    if (strides1 && arr1->ndim) compute_strides(arr1->shape, arr1->ndim, strides1);
    if (strides2 && arr2->ndim) compute_strides(arr2->shape, arr2->ndim, strides2);
    if (res_strides && result->ndim) compute_strides(result->shape, result->ndim, res_strides);
    size_t sz1 = dtype_size(arr1->dtype), sz2 = dtype_size(arr2->dtype), out_sz = dtype_size(out_dtype);
    char *data1 = (char*)arr1->data, *data2 = (char*)arr2->data, *res_data = (char*)result->data;
    int *indices = result->ndim ? calloc(result->ndim, sizeof(int)) : NULL;
    if (result->ndim && !indices) { free(strides1); free(strides2); free(res_strides); free_array(result); return NULL; }
    int total = result->size;
    for (int flat = 0; flat < total; ++flat) {
        if (result->ndim) {
            int tmp = flat;
            for (int i = result->ndim - 1; i >= 0; --i) { indices[i] = tmp % result->shape[i]; tmp /= result->shape[i]; }
        }
        int off1 = 0, off2 = 0;
        if (arr1->ndim && result->ndim) {
            int dim_offset = result->ndim - arr1->ndim;
            for (int i = 0; i < arr1->ndim; ++i) { int coord = indices[dim_offset+i]; if (arr1->shape[i]==1) coord=0; off1 += coord*strides1[i]; }
        }
        if (arr2->ndim && result->ndim) {
            int dim_offset = result->ndim - arr2->ndim;
            for (int i = 0; i < arr2->ndim; ++i) { int coord = indices[dim_offset+i]; if (arr2->shape[i]==1) coord=0; off2 += coord*strides2[i]; }
        }
        void *p1 = data1 + off1*sz1, *p2 = data2 + off2*sz2;
        complex double v1 = to_complex(p1, arr1->dtype);
        complex double v2 = to_complex(p2, arr2->dtype);
        complex double res = cpow(v1, v2);
        if (result->ndim == 0) {
            if (out_dtype == COMPLEX128) *(complex double*)res_data = res; else *(double*)res_data = creal(res);
        } else {
            int off_res = 0; for (int i = 0; i < result->ndim; ++i) off_res += indices[i]*res_strides[i];
            if (out_dtype == COMPLEX128) ((complex double*)res_data)[off_res] = res; else ((double*)res_data)[off_res] = creal(res);
        }
    }
    free(strides1); free(strides2); free(res_strides);
    if (indices) free(indices);
    return result;
}
