#include "array.h"
#include "shape.h"


/**
 * @brief matrix multiplication (like numpy.matmul)
 * @param arr1 first operand
 * @param arr2 second operand
 * @return product array
 */
Array* matmul(Array *arr1, Array *arr2) {
    if (arr1 == NULL || arr2 == NULL) {
        fprintf(stderr, "NULL array argument\n");
        return NULL;
    }

    // check ndim
    int ndim1 = arr1->ndim;
    int ndim2 = arr2->ndim;

    if (ndim1 < 1 || ndim2 < 1) {
        fprintf(stderr, "Arrays must have at least 1 dimension\n");
        return NULL;
    }

    // for 1D arrays, treat them as vectors
    if (ndim1 == 1 && ndim2 == 1) {
        // vector dot product
        if (arr1->shape[0] != arr2->shape[0]) {
            fprintf(stderr, "Vector dimensions do not match for dot product\n");
            return NULL;
        }

        // dot product result is a scalar, return a 0-D array here
        int value = 0;
        Array *result = create_scalar_array(&value, arr1->dtype);
        if (result == NULL) {
            return NULL;
        }

        size_t element_size = dtype_size(arr1->dtype);
        char *data1 = (char*)arr1->data;
        char *data2 = (char*)arr2->data;
        char *res_data = (char*)result->data;

        switch (arr1->dtype) {
            case INT8:
            case INT16:
            case INT32:
            case INT64:
            case UINT8:
            case UINT16:
            case UINT32:
            case UINT64: {
                // for integer types, use int64_t to perform computation
                int64_t sum = 0;
                for (int i = 0; i < arr1->shape[0]; i++) {
                    int64_t a = 0, b = 0;

                    switch (arr1->dtype) {
                        case INT8:
                            a = *(int8_t*)(data1 + i * element_size);
                            b = *(int8_t*)(data2 + i * element_size);
                            break;
                        case INT16:
                            a = *(int16_t*)(data1 + i * element_size);
                            b = *(int16_t*)(data2 + i * element_size);
                            break;
                        case INT32:
                            a = *(int32_t*)(data1 + i * element_size);
                            b = *(int32_t*)(data2 + i * element_size);
                            break;
                        case INT64:
                            a = *(int64_t*)(data1 + i * element_size);
                            b = *(int64_t*)(data2 + i * element_size);
                            break;
                        case UINT8:
                            a = *(uint8_t*)(data1 + i * element_size);
                            b = *(uint8_t*)(data2 + i * element_size);
                            break;
                        case UINT16:
                            a = *(uint16_t*)(data1 + i * element_size);
                            b = *(uint16_t*)(data2 + i * element_size);
                            break;
                        case UINT32:
                            a = *(uint32_t*)(data1 + i * element_size);
                            b = *(uint32_t*)(data2 + i * element_size);
                            break;
                        case UINT64:
                            a = *(uint64_t*)(data1 + i * element_size);
                            b = *(uint64_t*)(data2 + i * element_size);
                            break;
                        default:
                            break;
                    }

                    sum += a * b;
                }

                // convert back to original type
                switch (arr1->dtype) {
                    case INT8:
                        *(int8_t*)res_data = (int8_t)sum;
                        break;
                    case INT16:
                        *(int16_t*)res_data = (int16_t)sum;
                        break;
                    case INT32:
                        *(int32_t*)res_data = (int32_t)sum;
                        break;
                    case INT64:
                        *(int64_t*)res_data = sum;
                        break;
                    case UINT8:
                        *(uint8_t*)res_data = (uint8_t)sum;
                        break;
                    case UINT16:
                        *(uint16_t*)res_data = (uint16_t)sum;
                        break;
                    case UINT32:
                        *(uint32_t*)res_data = (uint32_t)sum;
                        break;
                    case UINT64:
                        *(uint64_t*)res_data = (uint64_t)sum;
                        break;
                    default:
                        break;
                }
                break;
            }

            case FLOAT32: {
                float sum = 0.0f;
                for (int i = 0; i < arr1->shape[0]; i++) {
                    sum += *(float*)(data1 + i * element_size) * *(float*)(data2 + i * element_size);
                }
                *(float*)res_data = sum;
                break;
            }

            case FLOAT64: {
                double sum = 0.0;
                for (int i = 0; i < arr1->shape[0]; i++) {
                    sum += *(double*)(data1 + i * element_size) * *(double*)(data2 + i * element_size);
                }
                *(double*)res_data = sum;
                break;
            }

            case COMPLEX64: {
                complex float sum = 0.0f + 0.0f * I;
                for (int i = 0; i < arr1->shape[0]; i++) {
                    sum += conjf(*(complex float*)(data1 + i * element_size)) * *(complex float*)(data2 + i * element_size);
                }
                *(complex float*)res_data = sum;
                break;
            }

            case COMPLEX128: {
                complex double sum = 0.0 + 0.0 * I;
                for (int i = 0; i < arr1->shape[0]; i++) {
                    sum += conj(*(complex double*)(data1 + i * element_size)) * *(complex double*)(data2 + i * element_size);
                }
                *(complex double*)res_data = sum;
                break;
            }

            default:
                fprintf(stderr, "Unsupported data type for matrix multiplication\n");
                free_array(result);
                return NULL;
        }

        return result;
    }

    // for 2D arrays, perform standard matrix multiplication
    if (ndim1 == 2 && ndim2 == 2) {
        return dot(arr1, arr2);
    }

    // --------------------------------------------------------------
    // 3. Mixed ndim: 1D @ ND or ND @ 1D
    // --------------------------------------------------------------
    if (ndim1 == 1 && ndim2 >= 2) {
        // treat 1D vector as a row vector (1, n)
        int new_shape[2] = {1, arr1->size};
        Array *a_2d = reshape(arr1, new_shape, 2);
        if (!a_2d) return NULL;
        Array *res_2d = matmul(a_2d, arr2);  // recursive call, but won't infinite-loop
        free_array(a_2d);
        if (!res_2d) return NULL;
        // result should be (1, m) shape, need to reduce to 1D
        if (res_2d->ndim == 2 && res_2d->shape[0] == 1) {
            Array *res_1d = reshape(res_2d, (int[]){res_2d->size}, 1);
            free_array(res_2d);
            return res_1d;
        }
        return res_2d;
    }
    if (ndim2 == 1 && ndim1 >= 2) {
        // treat 1D vector as a column vector (n, 1)
        int new_shape[2] = {arr2->size, 1};
        Array *b_2d = reshape(arr2, new_shape, 2);
        if (!b_2d) return NULL;
        Array *res_2d = matmul(arr1, b_2d);
        free_array(b_2d);
        if (!res_2d) return NULL;
        // result should be (m, 1) shape, reduce to 1D
        if (res_2d->ndim == 2 && res_2d->shape[1] == 1) {
            Array *res_1d = reshape(res_2d, (int[]){res_2d->size}, 1);
            free_array(res_2d);
            return res_1d;
        }
        return res_2d;
    }


    // --------------------------------------------------------------
    // 4. high-dimensional batch matrix multiplication (with broadcast support)
    // --------------------------------------------------------------
    // ensure at least 2 dimensions, and the last two dimensions are compatible
    if (ndim1 < 2 || ndim2 < 2) {
        fprintf(stderr, "matmul: unsupported dimensions\n");
        return NULL;
    }
    int m = arr1->shape[ndim1 - 2];
    int k1 = arr1->shape[ndim1 - 1];
    int k2 = arr2->shape[ndim2 - 2];
    int n = arr2->shape[ndim2 - 1];
    if (k1 != k2) {
        fprintf(stderr, "matmul: inner dimension mismatch (%d vs %d)\n", k1, k2);
        return NULL;
    }
    int k = k1;

    // batch dimension broadcast
    int batch_ndim1 = ndim1 - 2;
    int batch_ndim2 = ndim2 - 2;
    int max_batch = (batch_ndim1 > batch_ndim2) ? batch_ndim1 : batch_ndim2;
    int *batch_shape = malloc(max_batch * sizeof(int));
    if (!batch_shape) return NULL;
    for (int i = 0; i < max_batch; ++i) {
        int dim1 = (i < batch_ndim1) ? arr1->shape[i] : 1;
        int dim2 = (i < batch_ndim2) ? arr2->shape[i] : 1;
        if (dim1 != dim2 && dim1 != 1 && dim2 != 1) {
            free(batch_shape);
            fprintf(stderr, "matmul: batch dimensions not broadcastable\n");
            return NULL;
        }
        batch_shape[i] = (dim1 > dim2) ? dim1 : dim2;
    }

    int out_ndim = max_batch + 2;
    int *out_shape = malloc(out_ndim * sizeof(int));
    if (!out_shape) { free(batch_shape); return NULL; }
    for (int i = 0; i < max_batch; ++i) out_shape[i] = batch_shape[i];
    out_shape[out_ndim-2] = m;
    out_shape[out_ndim-1] = n;
    free(batch_shape);

    // determine output type (take higher precision; simplified here to use arr1 type, should be promoted in practice)
    DataType out_dtype = arr1->dtype;
    Array *result = create_array(out_shape, out_ndim, out_dtype);
    free(out_shape);
    if (!result) return NULL;

    // Stride computation
    int *strides1 = malloc(ndim1 * sizeof(int));
    int *strides2 = malloc(ndim2 * sizeof(int));
    int *res_strides = malloc(out_ndim * sizeof(int));
    if (!strides1 || !strides2 || !res_strides) goto error;
    compute_strides(arr1->shape, ndim1, strides1);
    compute_strides(arr2->shape, ndim2, strides2);
    compute_strides(result->shape, out_ndim, res_strides);

    size_t elem_sz = dtype_size(arr1->dtype);
    char *data1 = (char*)arr1->data;
    char *data2 = (char*)arr2->data;
    char *res_data = (char*)result->data;

    int *idx1 = calloc(ndim1, sizeof(int));
    int *idx2 = calloc(ndim2, sizeof(int));
    int *idx_res = calloc(out_ndim, sizeof(int));
    if (!idx1 || !idx2 || !idx_res) goto error;

    int batch_size = 1;
    for (int i = 0; i < max_batch; ++i) batch_size *= result->shape[i];

    for (int b = 0; b < batch_size; ++b) {
        // unpack batch index
        int tmp = b;
        for (int i = max_batch - 1; i >= 0; --i) {
            idx_res[i] = tmp % result->shape[i];
            tmp /= result->shape[i];
        }
        // build arr1 index
        for (int i = 0; i < ndim1; ++i) {
            if (i < batch_ndim1) {
                int dim = arr1->shape[i];
                idx1[i] = (dim == 1) ? 0 : idx_res[i];
            } else if (i >= ndim1 - 2) {
                idx1[i] = 0; // matrix indices filled in later
            } else {
                idx1[i] = 0;
            }
        }
        // build arr2 index
        for (int i = 0; i < ndim2; ++i) {
            if (i < batch_ndim2) {
                int dim = arr2->shape[i];
                idx2[i] = (dim == 1) ? 0 : idx_res[i];
            } else if (i >= ndim2 - 2) {
                idx2[i] = 0;
            } else {
                idx2[i] = 0;
            }
        }
        // compute matrix base offset
        int base1 = 0, base2 = 0;
        for (int i = 0; i < ndim1; ++i) base1 += idx1[i] * strides1[i];
        for (int i = 0; i < ndim2; ++i) base2 += idx2[i] * strides2[i];
        // result base offset
        int res_base = 0;
        for (int i = 0; i < out_ndim - 2; ++i) res_base += idx_res[i] * res_strides[i];

        // matrix product
        for (int i = 0; i < m; ++i) {
            for (int j = 0; j < n; ++j) {
                // select accumulation method based on data type
                switch (out_dtype) {
                    case INT32: {
                        int32_t sum = 0;
                        for (int l = 0; l < k; ++l) {
                            int off1 = base1 + i * strides1[ndim1-2] + l * strides1[ndim1-1];
                            int off2 = base2 + l * strides2[ndim2-2] + j * strides2[ndim2-1];
                            int32_t v1 = *(int32_t*)(data1 + off1 * elem_sz);
                            int32_t v2 = *(int32_t*)(data2 + off2 * elem_sz);
                            sum += v1 * v2;
                        }
                        int off_res = res_base + i * res_strides[out_ndim-2] + j * res_strides[out_ndim-1];
                        *(int32_t*)(res_data + off_res * elem_sz) = sum;
                        break;
                    }
                    case FLOAT64: {
                        double sum = 0.0;
                        for (int l = 0; l < k; ++l) {
                            int off1 = base1 + i * strides1[ndim1-2] + l * strides1[ndim1-1];
                            int off2 = base2 + l * strides2[ndim2-2] + j * strides2[ndim2-1];
                            double v1 = *(double*)(data1 + off1 * elem_sz);
                            double v2 = *(double*)(data2 + off2 * elem_sz);
                            sum += v1 * v2;
                        }
                        int off_res = res_base + i * res_strides[out_ndim-2] + j * res_strides[out_ndim-1];
                        *(double*)(res_data + off_res * elem_sz) = sum;
                        break;
                    }
                    case COMPLEX128: {
                        complex double sum = 0.0 + 0.0*I;
                        for (int l = 0; l < k; ++l) {
                            int off1 = base1 + i * strides1[ndim1-2] + l * strides1[ndim1-1];
                            int off2 = base2 + l * strides2[ndim2-2] + j * strides2[ndim2-1];
                            complex double v1 = *(complex double*)(data1 + off1 * elem_sz);
                            complex double v2 = *(complex double*)(data2 + off2 * elem_sz);
                            sum += v1 * v2;
                        }
                        int off_res = res_base + i * res_strides[out_ndim-2] + j * res_strides[out_ndim-1];
                        *(complex double*)(res_data + off_res * elem_sz) = sum;
                        break;
                    }
                    default: {
                        // other types can be expanded, temporarily use double as fallback
                        double sum = 0.0;
                        for (int l = 0; l < k; ++l) {
                            int off1 = base1 + i * strides1[ndim1-2] + l * strides1[ndim1-1];
                            int off2 = base2 + l * strides2[ndim2-2] + j * strides2[ndim2-1];
                            double v1 = 0.0, v2 = 0.0;
                            // extract based on actual type (omitted)
                            sum += v1 * v2;
                        }
                        int off_res = res_base + i * res_strides[out_ndim-2] + j * res_strides[out_ndim-1];
                        *(double*)(res_data + off_res * elem_sz) = sum;
                        break;
                    }
                }
            }
        }
    }

    free(strides1); free(strides2); free(res_strides);
    free(idx1); free(idx2); free(idx_res);
    return result;

error:
    free(strides1); free(strides2); free(res_strides);
    free(idx1); free(idx2); free(idx_res);
    free_array(result);
    return NULL;

}