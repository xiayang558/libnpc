#include "array.h"
#include "def.h"
#include "utils.h"


Array* cumsum(Array *arr, int axis) {
    if (arr == NULL) {
        fprintf(stderr, "cumsum: NULL array argument\n");
        return NULL;
    }
    if (axis < -1 || axis >= arr->ndim) {
        fprintf(stderr, "cumsum: Invalid axis %d for array with %d dimensions\n",
                axis, arr->ndim);
        return NULL;
    }

    // when axis == -1, flatten all elements, return 1D cumulative sum
    if (axis == -1) {
        Array *result = create_array((int[]){arr->size}, 1, arr->dtype);
        if (result == NULL) return NULL;
        size_t elem_size = dtype_size(arr->dtype);
        char *src = (char*)arr->data;
        char *dst = (char*)result->data;

        switch (arr->dtype) {
            case INT8: {
                int8_t *s = (int8_t*)src, *d = (int8_t*)dst;
                int8_t acc = 0;
                for (int i = 0; i < arr->size; i++) {
                    acc += s[i];
                    d[i] = acc;
                }
                break;
            }
            case INT16: {
                int16_t *s = (int16_t*)src, *d = (int16_t*)dst;
                int16_t acc = 0;
                for (int i = 0; i < arr->size; i++) {
                    acc += s[i];
                    d[i] = acc;
                }
                break;
            }
            case INT32: {
                int32_t *s = (int32_t*)src, *d = (int32_t*)dst;
                int32_t acc = 0;
                for (int i = 0; i < arr->size; i++) {
                    acc += s[i];
                    d[i] = acc;
                }
                break;
            }
            case INT64: {
                int64_t *s = (int64_t*)src, *d = (int64_t*)dst;
                int64_t acc = 0;
                for (int i = 0; i < arr->size; i++) {
                    acc += s[i];
                    d[i] = acc;
                }
                break;
            }
            case UINT8: {
                uint8_t *s = (uint8_t*)src, *d = (uint8_t*)dst;
                uint8_t acc = 0;
                for (int i = 0; i < arr->size; i++) {
                    acc += s[i];
                    d[i] = acc;
                }
                break;
            }
            case UINT16: {
                uint16_t *s = (uint16_t*)src, *d = (uint16_t*)dst;
                uint16_t acc = 0;
                for (int i = 0; i < arr->size; i++) {
                    acc += s[i];
                    d[i] = acc;
                }
                break;
            }
            case UINT32: {
                uint32_t *s = (uint32_t*)src, *d = (uint32_t*)dst;
                uint32_t acc = 0;
                for (int i = 0; i < arr->size; i++) {
                    acc += s[i];
                    d[i] = acc;
                }
                break;
            }
            case UINT64: {
                uint64_t *s = (uint64_t*)src, *d = (uint64_t*)dst;
                uint64_t acc = 0;
                for (int i = 0; i < arr->size; i++) {
                    acc += s[i];
                    d[i] = acc;
                }
                break;
            }
            case FLOAT32: {
                float *s = (float*)src, *d = (float*)dst;
                float acc = 0.0f;
                for (int i = 0; i < arr->size; i++) {
                    acc += s[i];
                    d[i] = acc;
                }
                break;
            }
            case FLOAT64: {
                double *s = (double*)src, *d = (double*)dst;
                double acc = 0.0;
                for (int i = 0; i < arr->size; i++) {
                    acc += s[i];
                    d[i] = acc;
                }
                break;
            }
            case COMPLEX64: {
                complex float *s = (complex float*)src, *d = (complex float*)dst;
                complex float acc = 0.0f + 0.0f*I;
                for (int i = 0; i < arr->size; i++) {
                    acc += s[i];
                    d[i] = acc;
                }
                break;
            }
            case COMPLEX128: {
                complex double *s = (complex double*)src, *d = (complex double*)dst;
                complex double acc = 0.0 + 0.0*I;
                for (int i = 0; i < arr->size; i++) {
                    acc += s[i];
                    d[i] = acc;
                }
                break;
            }
            default:
                fprintf(stderr, "cumsum: unsupported data type\n");
                free_array(result);
                return NULL;
        }
        return result;
    }

    // accumulate along specified axis, output shape same as input array
    Array *result = create_array(arr->shape, arr->ndim, arr->dtype);
    if (result == NULL) return NULL;

    // compute input and output strides (in elements as units)
    int *in_strides = malloc(arr->ndim * sizeof(int));
    int *out_strides = malloc(arr->ndim * sizeof(int));
    if (!in_strides || !out_strides) {
        free(in_strides); free(out_strides);
        free_array(result);
        return NULL;
    }
    compute_strides(arr->shape, arr->ndim, in_strides);
    compute_strides(arr->shape, arr->ndim, out_strides);  // output shape is the same, so stride structure is the same

    size_t elem_size = dtype_size(arr->dtype);
    char *in_data = (char*)arr->data;
    char *out_data = (char*)result->data;

    // build combinations of dimensions other than axis (slices)
    int outer_ndim = arr->ndim - 1;
    int *outer_dims = malloc(outer_ndim * sizeof(int));
    int *outer_shape = malloc(outer_ndim * sizeof(int));
    int *outer_indices = calloc(outer_ndim, sizeof(int));
    if (!outer_dims || !outer_shape || !outer_indices) {
        free(in_strides); free(out_strides);
        free(outer_dims); free(outer_shape); free(outer_indices);
        free_array(result);
        return NULL;
    }

    int idx = 0;
    for (int i = 0; i < arr->ndim; i++) {
        if (i != axis) {
            outer_dims[idx] = i;
            outer_shape[idx] = arr->shape[i];
            idx++;
        }
    }

    int num_slices = 1;
    for (int i = 0; i < outer_ndim; i++) num_slices *= outer_shape[i];

    for (int slice = 0; slice < num_slices; slice++) {
        // base offset of current slice in input/output (fixed indices for dimensions other than axis)
        int base_in = 0, base_out = 0;
        for (int j = 0; j < outer_ndim; j++) {
            base_in  += outer_indices[j] * in_strides[outer_dims[j]];
            base_out += outer_indices[j] * out_strides[outer_dims[j]];
        }

        // perform accumulation along the axis direction
        switch (arr->dtype) {
            case INT8: {
                int8_t *p_in  = (int8_t*)(in_data + base_in * elem_size);
                int8_t *p_out = (int8_t*)(out_data + base_out * elem_size);
                int8_t acc = 0;
                for (int k = 0; k < arr->shape[axis]; k++) {
                    acc += p_in[k * in_strides[axis]];
                    p_out[k * out_strides[axis]] = acc;
                }
                break;
            }
            case INT16: {
                int16_t *p_in  = (int16_t*)(in_data + base_in * elem_size);
                int16_t *p_out = (int16_t*)(out_data + base_out * elem_size);
                int16_t acc = 0;
                for (int k = 0; k < arr->shape[axis]; k++) {
                    acc += p_in[k * in_strides[axis]];
                    p_out[k * out_strides[axis]] = acc;
                }
                break;
            }
            case INT32: {
                int32_t *p_in  = (int32_t*)(in_data + base_in * elem_size);
                int32_t *p_out = (int32_t*)(out_data + base_out * elem_size);
                int32_t acc = 0;
                for (int k = 0; k < arr->shape[axis]; k++) {
                    acc += p_in[k * in_strides[axis]];
                    p_out[k * out_strides[axis]] = acc;
                }
                break;
            }
            case INT64: {
                int64_t *p_in  = (int64_t*)(in_data + base_in * elem_size);
                int64_t *p_out = (int64_t*)(out_data + base_out * elem_size);
                int64_t acc = 0;
                for (int k = 0; k < arr->shape[axis]; k++) {
                    acc += p_in[k * in_strides[axis]];
                    p_out[k * out_strides[axis]] = acc;
                }
                break;
            }
            case UINT8: {
                uint8_t *p_in  = (uint8_t*)(in_data + base_in * elem_size);
                uint8_t *p_out = (uint8_t*)(out_data + base_out * elem_size);
                uint8_t acc = 0;
                for (int k = 0; k < arr->shape[axis]; k++) {
                    acc += p_in[k * in_strides[axis]];
                    p_out[k * out_strides[axis]] = acc;
                }
                break;
            }
            case UINT16: {
                uint16_t *p_in  = (uint16_t*)(in_data + base_in * elem_size);
                uint16_t *p_out = (uint16_t*)(out_data + base_out * elem_size);
                uint16_t acc = 0;
                for (int k = 0; k < arr->shape[axis]; k++) {
                    acc += p_in[k * in_strides[axis]];
                    p_out[k * out_strides[axis]] = acc;
                }
                break;
            }
            case UINT32: {
                uint32_t *p_in  = (uint32_t*)(in_data + base_in * elem_size);
                uint32_t *p_out = (uint32_t*)(out_data + base_out * elem_size);
                uint32_t acc = 0;
                for (int k = 0; k < arr->shape[axis]; k++) {
                    acc += p_in[k * in_strides[axis]];
                    p_out[k * out_strides[axis]] = acc;
                }
                break;
            }
            case UINT64: {
                uint64_t *p_in  = (uint64_t*)(in_data + base_in * elem_size);
                uint64_t *p_out = (uint64_t*)(out_data + base_out * elem_size);
                uint64_t acc = 0;
                for (int k = 0; k < arr->shape[axis]; k++) {
                    acc += p_in[k * in_strides[axis]];
                    p_out[k * out_strides[axis]] = acc;
                }
                break;
            }
            case FLOAT32: {
                float *p_in  = (float*)(in_data + base_in * elem_size);
                float *p_out = (float*)(out_data + base_out * elem_size);
                float acc = 0.0f;
                for (int k = 0; k < arr->shape[axis]; k++) {
                    acc += p_in[k * in_strides[axis]];
                    p_out[k * out_strides[axis]] = acc;
                }
                break;
            }
            case FLOAT64: {
                double *p_in  = (double*)(in_data + base_in * elem_size);
                double *p_out = (double*)(out_data + base_out * elem_size);
                double acc = 0.0;
                for (int k = 0; k < arr->shape[axis]; k++) {
                    acc += p_in[k * in_strides[axis]];
                    p_out[k * out_strides[axis]] = acc;
                }
                break;
            }
            case COMPLEX64: {
                complex float *p_in  = (complex float*)(in_data + base_in * elem_size);
                complex float *p_out = (complex float*)(out_data + base_out * elem_size);
                complex float acc = 0.0f + 0.0f*I;
                for (int k = 0; k < arr->shape[axis]; k++) {
                    acc += p_in[k * in_strides[axis]];
                    p_out[k * out_strides[axis]] = acc;
                }
                break;
            }
            case COMPLEX128: {
                complex double *p_in  = (complex double*)(in_data + base_in * elem_size);
                complex double *p_out = (complex double*)(out_data + base_out * elem_size);
                complex double acc = 0.0 + 0.0*I;
                for (int k = 0; k < arr->shape[axis]; k++) {
                    acc += p_in[k * in_strides[axis]];
                    p_out[k * out_strides[axis]] = acc;
                }
                break;
            }
            default:
                fprintf(stderr, "cumsum: unsupported data type\n");
                free(in_strides); free(out_strides);
                free(outer_dims); free(outer_shape); free(outer_indices);
                free_array(result);
                return NULL;
        }

        // update outer_indices to next slice
        int carry = 1;
        for (int j = outer_ndim - 1; j >= 0 && carry; j--) {
            outer_indices[j]++;
            if (outer_indices[j] >= outer_shape[j]) {
                outer_indices[j] = 0;
                carry = 1;
            } else {
                carry = 0;
            }
        }
        if (carry) break;
    }

    free(in_strides);
    free(out_strides);
    free(outer_dims);
    free(outer_shape);
    free(outer_indices);
    return result;
}