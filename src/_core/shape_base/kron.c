#include "def.h"
#include "array.h"
#include "shape.h"

static DataType promote_dtype(DataType da, DataType db) {
    if (da == COMPLEX64 || da == COMPLEX128 || da == COMPLEX256 ||
        db == COMPLEX64 || db == COMPLEX128 || db == COMPLEX256)
        return COMPLEX128;
    if (da == FLOAT32 || da == FLOAT64 || da == FLOAT128 ||
        db == FLOAT32 || db == FLOAT64 || db == FLOAT128)
        return FLOAT64;
    return FLOAT64;
}

static double get_double_value(void *ptr, DataType dtype) {
    switch (dtype) {
        case INT8:   return *(int8_t*)ptr;   case INT16:  return *(int16_t*)ptr;
        case INT32:  return *(int32_t*)ptr;  case INT64:  return *(int64_t*)ptr;
        case UINT8:  return *(uint8_t*)ptr;  case UINT16: return *(uint16_t*)ptr;
        case UINT32: return *(uint32_t*)ptr; case UINT64: return *(uint64_t*)ptr;
        case FLOAT32: return *(float*)ptr;   case FLOAT64: return *(double*)ptr;
        default: return 0.0;
    }
}

static complex double get_complex_value(void *ptr, DataType dtype) {
    switch (dtype) {
        case COMPLEX64: return (complex double)*(complex float*)ptr;
        case COMPLEX128: return *(complex double*)ptr;
        default: return (complex double)get_double_value(ptr, dtype);
    }
}

Array* kron(Array *arr1, Array *arr2) {
    if (arr1 == NULL || arr2 == NULL) { fprintf(stderr, "kron: NULL array argument\n"); return NULL; }
    int ndim_arr1 = arr1->ndim, ndim_arr2 = arr2->ndim;
    int ndim_out = (ndim_arr1 > ndim_arr2) ? ndim_arr1 : ndim_arr2;
    int a_shape_padded[32] = {0}, b_shape_padded[32] = {0};
    for (int i = 0; i < ndim_out; i++) {
        int a_idx = i - (ndim_out - ndim_arr1), b_idx = i - (ndim_out - ndim_arr2);
        a_shape_padded[i] = (a_idx >= 0) ? arr1->shape[a_idx] : 1;
        b_shape_padded[i] = (b_idx >= 0) ? arr2->shape[b_idx] : 1;
    }
    int out_shape[32];
    for (int i = 0; i < ndim_out; i++) out_shape[i] = a_shape_padded[i] * b_shape_padded[i];
    DataType out_dtype = promote_dtype(arr1->dtype, arr2->dtype);
    Array *res = create_array(out_shape, ndim_out, out_dtype);
    if (res == NULL) return NULL;
    int a_strides[32], b_strides[32], res_strides[32];
    compute_strides(a_shape_padded, ndim_out, a_strides);
    compute_strides(b_shape_padded, ndim_out, b_strides);
    compute_strides(out_shape, ndim_out, res_strides);
    size_t a_elem = dtype_size(arr1->dtype), b_elem = dtype_size(arr2->dtype);
    char *a_data = (char*)arr1->data, *b_data = (char*)arr2->data;
    int divisor[32];
    for (int i = 0; i < ndim_out; i++) divisor[i] = b_shape_padded[i];
    size_t out_elem = dtype_size(out_dtype);
    char *res_data = (char*)res->data;
    int *indices = malloc(ndim_out * sizeof(int));
    if (indices == NULL) { free_array(res); return NULL; }
    for (int flat = 0; flat < res->size; flat++) {
        int tmp = flat;
        for (int i = ndim_out - 1; i >= 0; i--) { indices[i] = tmp % res->shape[i]; tmp /= res->shape[i]; }
        int a_coords[32], b_coords[32];
        for (int i = 0; i < ndim_out; i++) { int coord = indices[i]; a_coords[i] = coord / divisor[i]; b_coords[i] = coord % divisor[i]; }
        int a_offset = 0, a_start_idx = ndim_out - ndim_arr1;
        for (int i = a_start_idx; i < ndim_out; i++) a_offset += a_coords[i] * a_strides[i];
        int b_offset = 0, b_start_idx = ndim_out - ndim_arr2;
        for (int i = b_start_idx; i < ndim_out; i++) b_offset += b_coords[i] * b_strides[i];
        void *a_ptr = a_data + a_offset * a_elem, *b_ptr = b_data + b_offset * b_elem;
        void *res_ptr = res_data + flat * out_elem;
        if (out_dtype == COMPLEX128) {
            complex double av=get_complex_value(a_ptr,arr1->dtype), bv=get_complex_value(b_ptr,arr2->dtype);
            complex double prod=av*bv; memcpy(res_ptr,&prod,sizeof(complex double));
        } else { double av=get_double_value(a_ptr,arr1->dtype), bv=get_double_value(b_ptr,arr2->dtype); double prod=av*bv; memcpy(res_ptr,&prod,sizeof(double)); }
    }
    free(indices);
    return res;
}
