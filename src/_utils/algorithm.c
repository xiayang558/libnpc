#include <stdio.h>
#include "array.h"
#include "function.h"

/**
 * @brief Find the left endpoint index of the bin containing x in strictly monotonically increasing array xp. 
 * @param x   interpolation point
 * @param xp  known x-coordinate array (length len)
 * @param len array length
 * @return If x < xp[0] return -1; if x > xp[len-1] return -2; 
 *         otherwise return the largest idx such that xp[idx] <= x (i.e. the left endpoint of the bin containing x). 
 */
int find_interval(double x, double *xp, int len) {
    if (x < xp[0]) return -1;
    if (x > xp[len-1]) return -2;
    int lo = 0, hi = len - 1;
    while (lo < hi) {
        int mid = (lo + hi + 1) / 2;  // Upper median, avoid infinite loop
        if (xp[mid] <= x)
            lo = mid;
        else
            hi = mid - 1;
    }
    return lo;
}

int64_t gcd_int64(int64_t a, int64_t b) {
    a = llabs(a);
    b = llabs(b);
    while (b != 0) {
        int64_t t = b;
        b = a % b;
        a = t;
    }
    return a;
}

int64_t lcm_int64(int64_t a, int64_t b) {
    if (a == 0 || b == 0) return 0;
    int64_t g = gcd_int64(a, b);
    // May overflow, but NumPy also overflows
    return (a / g) * b;
}

Array* lcm(Array *x1, Array *x2) {
    if (x1 == NULL || x2 == NULL) {
        fprintf(stderr, "lcm: NULL array argument\n");
        return NULL;
    }
    if (!is_integer(x1->dtype) || !is_integer(x2->dtype)) {
        fprintf(stderr, "lcm: only integer types are supported\n");
        return NULL;
    }

    // Compute shape after broadcast
    int out_ndim;
    int *out_shape = broadcast_shapes(2, (Array*[]){x1, x2}, &out_ndim);
    if (out_shape == NULL) {
        fprintf(stderr, "lcm: broadcast failed\n");
        return NULL;
    }

    // create result arrays (type is INT64)
    Array *result = create_array(out_shape, out_ndim, INT64);
    free(out_shape);
    if (result == NULL) return NULL;

    // compute stride
    int *strides1 = NULL, *strides2 = NULL, *res_strides = NULL;
    if (x1->ndim > 0) {
        strides1 = malloc(x1->ndim * sizeof(int));
        compute_strides(x1->shape, x1->ndim, strides1);
    }
    if (x2->ndim > 0) {
        strides2 = malloc(x2->ndim * sizeof(int));
        compute_strides(x2->shape, x2->ndim, strides2);
    }
    if (result->ndim > 0) {
        res_strides = malloc(result->ndim * sizeof(int));
        compute_strides(result->shape, result->ndim, res_strides);
    }

    size_t sz1 = dtype_size(x1->dtype);
    size_t sz2 = dtype_size(x2->dtype);
    char *data1 = (char*)x1->data;
    char *data2 = (char*)x2->data;
    int64_t *res_data = (int64_t*)result->data;

    int *indices = calloc(result->ndim, sizeof(int));
    if (indices == NULL) {
        free(strides1); free(strides2); free(res_strides);
        free_array(result);
        return NULL;
    }

    // Traverse each result element
    for (int flat = 0; flat < result->size; ++flat) {
        // Decode index
        int tmp = flat;
        for (int i = result->ndim - 1; i >= 0; --i) {
            indices[i] = tmp % result->shape[i];
            tmp /= result->shape[i];
        }
        // Get x1 and x2 offset
        int off1 = 0, off2 = 0;
        if (x1->ndim > 0) {
            int dim_offset = result->ndim - x1->ndim;
            for (int i = 0; i < x1->ndim; ++i) {
                int coord = indices[dim_offset + i];
                if (x1->shape[i] == 1) coord = 0;
                off1 += coord * strides1[i];
            }
        }
        if (x2->ndim > 0) {
            int dim_offset = result->ndim - x2->ndim;
            for (int i = 0; i < x2->ndim; ++i) {
                int coord = indices[dim_offset + i];
                if (x2->shape[i] == 1) coord = 0;
                off2 += coord * strides2[i];
            }
        }
        void *p1 = data1 + off1 * sz1;
        void *p2 = data2 + off2 * sz2;
        int64_t v1 = to_int64(p1, x1->dtype);
        int64_t v2 = to_int64(p2, x2->dtype);
        res_data[flat] = lcm_int64(v1, v2);
    }

    free(strides1); free(strides2); free(res_strides);
    free(indices);
    return result;
}

Array* aldexp(Array *x1, Array *x2) {
    if (x1 == NULL || x2 == NULL) {
        fprintf(stderr, "aldexp: NULL array argument\n");
        return NULL;
    }
    if (!is_integer(x2->dtype)) {
        fprintf(stderr, "aldexp: exponent must be integer type\n");
        return NULL;
    }

    // Compute shape after broadcast
    int out_ndim;
    int *out_shape = broadcast_shapes(2, (Array*[]){x1, x2}, &out_ndim);
    if (out_shape == NULL) {
        fprintf(stderr, "aldexp: broadcast failed\n");
        return NULL;
    }

    int is_complex = (x1->dtype == COMPLEX64 || x1->dtype == COMPLEX128);
    DataType out_dtype;
    if (is_complex) {
        out_dtype = (x1->dtype == COMPLEX64) ? COMPLEX64 : COMPLEX128;
    } else {
        out_dtype = FLOAT64;
    }

    // create result arrays (supports scalar)
    Array *result;
    if (out_ndim == 0) {
        // Scalar output: manually create 0-dimensional array
        result = (Array*)malloc(sizeof(Array));
        if (result == NULL) {
            free(out_shape);
            return NULL;
        }
        size_t elem_sz = dtype_size(out_dtype);
        result->data = malloc(elem_sz);
        if (result->data == NULL) {
            free(result);
            free(out_shape);
            return NULL;
        }
        result->dtype = out_dtype;
        result->shape = NULL;
        result->ndim = 0;
        result->size = 1;
        result->strides = NULL;
        result->is_view = 0;
    } else {
        result = create_array(out_shape, out_ndim, out_dtype);
        if (result == NULL) {
            free(out_shape);
            return NULL;
        }
    }
    free(out_shape);

    // Stride computation (only when ndim > 0)
    int *strides1 = NULL, *strides2 = NULL, *res_strides = NULL;
    if (x1->ndim > 0) {
        strides1 = malloc(x1->ndim * sizeof(int));
        compute_strides(x1->shape, x1->ndim, strides1);
    }
    if (x2->ndim > 0) {
        strides2 = malloc(x2->ndim * sizeof(int));
        compute_strides(x2->shape, x2->ndim, strides2);
    }
    if (result->ndim > 0) {
        res_strides = malloc(result->ndim * sizeof(int));
        compute_strides(result->shape, result->ndim, res_strides);
    }

    size_t sz1 = dtype_size(x1->dtype);
    size_t sz2 = dtype_size(x2->dtype);
    size_t out_sz = dtype_size(out_dtype);
    char *data1 = (char*)x1->data;
    char *data2 = (char*)x2->data;
    char *out_data = (char*)result->data;

    int *indices = NULL;
    if (result->ndim > 0) {
        indices = calloc(result->ndim, sizeof(int));
        if (indices == NULL) {
            free(strides1); free(strides2); free(res_strides);
            free_array(result);
            return NULL;
        }
    }

    // Traverse output element
    int total = result->size;
    for (int flat = 0; flat < total; ++flat) {
        if (result->ndim > 0) {
            int tmp = flat;
            for (int i = result->ndim - 1; i >= 0; --i) {
                indices[i] = tmp % result->shape[i];
                tmp /= result->shape[i];
            }
        }
        // compute x1 offset
        int off1 = 0;
        if (x1->ndim > 0 && result->ndim > 0) {
            int dim_offset = result->ndim - x1->ndim;
            for (int i = 0; i < x1->ndim; ++i) {
                int coord = indices[dim_offset + i];
                if (x1->shape[i] == 1) coord = 0;
                off1 += coord * strides1[i];
            }
        }
        // compute x2 offset
        int off2 = 0;
        if (x2->ndim > 0 && result->ndim > 0) {
            int dim_offset = result->ndim - x2->ndim;
            for (int i = 0; i < x2->ndim; ++i) {
                int coord = indices[dim_offset + i];
                if (x2->shape[i] == 1) coord = 0;
                off2 += coord * strides2[i];
            }
        }
        void *p1 = data1 + off1 * sz1;
        void *p2 = data2 + off2 * sz2;
        int exp = to_int64(p2, x2->dtype);
        void *out_ptr = out_data + flat * out_sz;

        if (is_complex) {
            if (x1->dtype == COMPLEX64) {
                complex float val = *(complex float*)p1;
                float real = crealf(val);
                float imag = cimagf(val);
                float real_out = ldexpf(real, exp);
                float imag_out = ldexpf(imag, exp);
                complex float res = real_out + imag_out * I;
                memcpy(out_ptr, &res, out_sz);
            } else {
                complex double val = *(complex double*)p1;
                double real = creal(val);
                double imag = cimag(val);
                double real_out = ldexp(real, exp);
                double imag_out = ldexp(imag, exp);
                complex double res = real_out + imag_out * I;
                memcpy(out_ptr, &res, out_sz);
            }
        } else {
            double val = to_double(p1, x1->dtype);
            double res = ldexp(val, exp);
            if (out_dtype == FLOAT64) {
                *(double*)out_ptr = res;
            } else {
                *(float*)out_ptr = (float)res;
            }
        }
    }

    free(strides1); free(strides2); free(res_strides);
    if (indices) free(indices);
    return result;
}



// Convert shift amount to int (assumes non-negative; negative shifts cause errors in NumPy, we restrict to non-negative)
static int to_shift(void *ptr, DataType dtype) {
    uint64_t v = to_uint64(ptr, dtype);
    if (v > 63) v = 63; // Clamp range to avoid undefined behavior (similar to NumPy behavior)
    return (int)v;
}

// Write result back to original type (truncate based on type)
static void store_shift_result(void *dst, uint64_t val, DataType dtype) {
    switch (dtype) {
        case BOOL:   *(uint8_t*)dst = (uint8_t)val & 1; break;
        case INT8:   *(int8_t*)dst = (int8_t)val; break;
        case INT16:  *(int16_t*)dst = (int16_t)val; break;
        case INT32:  *(int32_t*)dst = (int32_t)val; break;
        case INT64:  *(int64_t*)dst = (int64_t)val; break;
        case UINT8:  *(uint8_t*)dst = (uint8_t)val; break;
        case UINT16: *(uint16_t*)dst = (uint16_t)val; break;
        case UINT32: *(uint32_t*)dst = (uint32_t)val; break;
        case UINT64: *(uint64_t*)dst = val; break;
        default: break;
    }
}

// Generic shift function
static Array* shift_op(Array *x1, Array *x2, int is_left) {
    if (x1 == NULL || x2 == NULL) {
        fprintf(stderr, "shift_op: NULL array argument\n");
        return NULL;
    }
    if (!is_integer(x1->dtype) || !is_integer(x2->dtype)) {
        fprintf(stderr, "shift_op: operands must be integer types\n");
        return NULL;
    }

    // broadcast
    int out_ndim;
    int *out_shape = broadcast_shapes(2, (Array*[]){x1, x2}, &out_ndim);
    if (out_shape == NULL) {
        fprintf(stderr, "shift_op: broadcast failed\n");
        return NULL;
    }

    // output type is same as x1
    Array *result = create_array(out_shape, out_ndim, x1->dtype);
    free(out_shape);
    if (result == NULL) return NULL;

    // stride
    int *strides1 = NULL, *strides2 = NULL, *res_strides = NULL;
    if (x1->ndim > 0) {
        strides1 = malloc(x1->ndim * sizeof(int));
        compute_strides(x1->shape, x1->ndim, strides1);
    }
    if (x2->ndim > 0) {
        strides2 = malloc(x2->ndim * sizeof(int));
        compute_strides(x2->shape, x2->ndim, strides2);
    }
    if (result->ndim > 0) {
        res_strides = malloc(result->ndim * sizeof(int));
        compute_strides(result->shape, result->ndim, res_strides);
    }

    size_t sz1 = dtype_size(x1->dtype);
    size_t sz2 = dtype_size(x2->dtype);
    char *data1 = (char*)x1->data;
    char *data2 = (char*)x2->data;
    char *out_data = (char*)result->data;

    int *indices = NULL;
    if (result->ndim > 0) {
        indices = calloc(result->ndim, sizeof(int));
        if (indices == NULL) {
            free(strides1); free(strides2); free(res_strides);
            free_array(result);
            return NULL;
        }
    }

    int total = result->size;
    for (int flat = 0; flat < total; ++flat) {
        // Decode index
        if (result->ndim > 0) {
            int tmp = flat;
            for (int i = result->ndim - 1; i >= 0; --i) {
                indices[i] = tmp % result->shape[i];
                tmp /= result->shape[i];
            }
        }
        // get x1 offset
        int off1 = 0;
        if (x1->ndim > 0 && result->ndim > 0) {
            int dim_offset = result->ndim - x1->ndim;
            for (int i = 0; i < x1->ndim; ++i) {
                int coord = indices[dim_offset + i];
                if (x1->shape[i] == 1) coord = 0;
                off1 += coord * strides1[i];
            }
        }
        // get x2 offset
        int off2 = 0;
        if (x2->ndim > 0 && result->ndim > 0) {
            int dim_offset = result->ndim - x2->ndim;
            for (int i = 0; i < x2->ndim; ++i) {
                int coord = indices[dim_offset + i];
                if (x2->shape[i] == 1) coord = 0;
                off2 += coord * strides2[i];
            }
        }

        void *p1 = data1 + off1 * sz1;
        void *p2 = data2 + off2 * sz2;
        uint64_t val = to_uint64(p1, x1->dtype);
        int shift = to_shift(p2, x2->dtype);
        uint64_t res_val;
        if (is_left) {
            res_val = val << shift;
        } else {
            res_val = val >> shift;
        }
        void *out_ptr = out_data + flat * dtype_size(x1->dtype);
        store_shift_result(out_ptr, res_val, x1->dtype);
    }

    free(strides1); free(strides2); free(res_strides);
    if (indices) free(indices);
    return result;
}

// Element-wise left shift (like numpy.left_shift)
Array* left_shift(Array *x1, Array *x2) {
    return shift_op(x1, x2, 1);
}

// Element-wise right shift (like numpy.right_shift)
Array* right_shift(Array *x1, Array *x2) {
    return shift_op(x1, x2, 0);
}


Array* less(Array *arr1, Array *arr2) {
    if (arr1 == NULL || arr2 == NULL) {
        fprintf(stderr, "less: NULL array argument\n");
        return NULL;
    }
    if (arr1->dtype != arr2->dtype) {
        fprintf(stderr, "less: data type mismatch\n");
        return NULL;
    }

    // 1. Compute shape after broadcast
    int out_ndim;
    int *out_shape = broadcast_shapes(2, (Array*[]){arr1, arr2}, &out_ndim);
    if (out_shape == NULL) {
        fprintf(stderr, "less: broadcast failed\n");
        return NULL;
    }

    // 2. create result arrays (BOOLtype)
    Array *result;
    if (out_ndim == 0) {
        result = (Array*)malloc(sizeof(Array));
        if (!result) { free(out_shape); return NULL; }
        result->data = malloc(sizeof(uint8_t));
        if (!result->data) { free(result); free(out_shape); return NULL; }
        result->dtype = BOOL;
        result->shape = NULL;
        result->ndim = 0;
        result->size = 1;
        result->strides = NULL;
        result->is_view = 0;
    } else {
        result = create_array(out_shape, out_ndim, BOOL);
        if (!result) {
            free(out_shape);
            return NULL;
        }
    }
    free(out_shape);

    // 3. compute stride
    int *strides1 = NULL, *strides2 = NULL, *res_strides = NULL;
    if (arr1->ndim > 0) {
        strides1 = malloc(arr1->ndim * sizeof(int));
        compute_strides(arr1->shape, arr1->ndim, strides1);
    }
    if (arr2->ndim > 0) {
        strides2 = malloc(arr2->ndim * sizeof(int));
        compute_strides(arr2->shape, arr2->ndim, strides2);
    }
    if (result->ndim > 0) {
        res_strides = malloc(result->ndim * sizeof(int));
        compute_strides(result->shape, result->ndim, res_strides);
    }

    size_t elem_sz = dtype_size(arr1->dtype);
    char *data1 = (char*)arr1->data;
    char *data2 = (char*)arr2->data;
    uint8_t *res_data = (uint8_t*)result->data;

    int *indices = NULL;
    if (result->ndim > 0) {
        indices = calloc(result->ndim, sizeof(int));
        if (!indices) {
            free(strides1); free(strides2); free(res_strides);
            free_array(result);
            return NULL;
        }
    }

    int total = result->size;
    for (int flat = 0; flat < total; ++flat) {
        // Decode output coordinates
        if (result->ndim > 0) {
            int tmp = flat;
            for (int i = result->ndim - 1; i >= 0; --i) {
                indices[i] = tmp % result->shape[i];
                tmp /= result->shape[i];
            }
        }

        // get arr1 offset (broadcast)
        int off1 = 0;
        if (arr1->ndim > 0 && result->ndim > 0) {
            int dim_offset = result->ndim - arr1->ndim;
            for (int i = 0; i < arr1->ndim; ++i) {
                int coord = indices[dim_offset + i];
                if (arr1->shape[i] == 1) coord = 0;
                off1 += coord * strides1[i];
            }
        }

        // get arr2 offset (broadcast)
        int off2 = 0;
        if (arr2->ndim > 0 && result->ndim > 0) {
            int dim_offset = result->ndim - arr2->ndim;
            for (int i = 0; i < arr2->ndim; ++i) {
                int coord = indices[dim_offset + i];
                if (arr2->shape[i] == 1) coord = 0;
                off2 += coord * strides2[i];
            }
        }

        void *p1 = data1 + off1 * elem_sz;
        void *p2 = data2 + off2 * elem_sz;

        // Compare: use < sign
        int cmp = 0;
        switch (arr1->dtype) {
            case INT8:   cmp = *(int8_t*)p1 < *(int8_t*)p2; break;
            case INT16:  cmp = *(int16_t*)p1 < *(int16_t*)p2; break;
            case INT32:  cmp = *(int32_t*)p1 < *(int32_t*)p2; break;
            case INT64:  cmp = *(int64_t*)p1 < *(int64_t*)p2; break;
            case UINT8:  cmp = *(uint8_t*)p1 < *(uint8_t*)p2; break;
            case UINT16: cmp = *(uint16_t*)p1 < *(uint16_t*)p2; break;
            case UINT32: cmp = *(uint32_t*)p1 < *(uint32_t*)p2; break;
            case UINT64: cmp = *(uint64_t*)p1 < *(uint64_t*)p2; break;
            case FLOAT32: cmp = *(float*)p1 < *(float*)p2; break;
            case FLOAT64: cmp = *(double*)p1 < *(double*)p2; break;
            case COMPLEX64: {
                float a = cabsf(*(complex float*)p1);
                float b = cabsf(*(complex float*)p2);
                cmp = (a < b);
                break;
            }
            case COMPLEX128: {
                double a = cabs(*(complex double*)p1);
                double b = cabs(*(complex double*)p2);
                cmp = (a < b);
                break;
            }
            default: break;
        }

        // writeresult
        if (result->ndim == 0) {
            res_data[0] = cmp ? 1 : 0;
        } else {
            int res_off = 0;
            for (int i = 0; i < result->ndim; ++i) {
                res_off += indices[i] * res_strides[i];
            }
            res_data[res_off] = cmp ? 1 : 0;
        }
    }

    free(strides1); free(strides2); free(res_strides);
    if (indices) free(indices);
    return result;
}



Array* less_equal(Array *arr1, Array *arr2) {
    if (arr1 == NULL || arr2 == NULL) {
        fprintf(stderr, "less_equal: NULL array argument\n");
        return NULL;
    }
    if (arr1->dtype != arr2->dtype) {
        fprintf(stderr, "less_equal: data type mismatch\n");
        return NULL;
    }

    // 1. Compute shape after broadcast
    int out_ndim;
    int *out_shape = broadcast_shapes(2, (Array*[]){arr1, arr2}, &out_ndim);
    if (out_shape == NULL) {
        fprintf(stderr, "less_equal: broadcast failed\n");
        return NULL;
    }

    // 2. create result arrays (BOOLtype)
    Array *result;
    if (out_ndim == 0) {
        // Scalar output
        result = (Array*)malloc(sizeof(Array));
        if (!result) { free(out_shape); return NULL; }
        result->data = malloc(sizeof(uint8_t));
        if (!result->data) { free(result); free(out_shape); return NULL; }
        result->dtype = BOOL;
        result->shape = NULL;
        result->ndim = 0;
        result->size = 1;
        result->strides = NULL;
        result->is_view = 0;
    } else {
        result = create_array(out_shape, out_ndim, BOOL);
        if (!result) {
            free(out_shape);
            return NULL;
        }
    }
    free(out_shape);

    // 3. compute strides (only when array is not scalar)
    int *strides1 = NULL, *strides2 = NULL, *res_strides = NULL;
    if (arr1->ndim > 0) {
        strides1 = malloc(arr1->ndim * sizeof(int));
        compute_strides(arr1->shape, arr1->ndim, strides1);
    }
    if (arr2->ndim > 0) {
        strides2 = malloc(arr2->ndim * sizeof(int));
        compute_strides(arr2->shape, arr2->ndim, strides2);
    }
    if (result->ndim > 0) {
        res_strides = malloc(result->ndim * sizeof(int));
        compute_strides(result->shape, result->ndim, res_strides);
    }

    size_t elem_sz = dtype_size(arr1->dtype);
    char *data1 = (char*)arr1->data;
    char *data2 = (char*)arr2->data;
    uint8_t *res_data = (uint8_t*)result->data;

    // 4. traverseoutput arrayper element
    int *indices = NULL;
    if (result->ndim > 0) {
        indices = calloc(result->ndim, sizeof(int));
        if (!indices) {
            free(strides1); free(strides2); free(res_strides);
            free_array(result);
            return NULL;
        }
    }

    int total = result->size;
    for (int flat = 0; flat < total; ++flat) {
        // Decode output coordinates
        if (result->ndim > 0) {
            int tmp = flat;
            for (int i = result->ndim - 1; i >= 0; --i) {
                indices[i] = tmp % result->shape[i];
                tmp /= result->shape[i];
            }
        }

        // Compute offset in arr1 (consider broadcast)
        int off1 = 0;
        if (arr1->ndim > 0 && result->ndim > 0) {
            int dim_offset = result->ndim - arr1->ndim;
            for (int i = 0; i < arr1->ndim; ++i) {
                int coord = indices[dim_offset + i];
                if (arr1->shape[i] == 1) coord = 0;
                off1 += coord * strides1[i];
            }
        }

        // Compute offset in arr2 (consider broadcast)
        int off2 = 0;
        if (arr2->ndim > 0 && result->ndim > 0) {
            int dim_offset = result->ndim - arr2->ndim;
            for (int i = 0; i < arr2->ndim; ++i) {
                int coord = indices[dim_offset + i];
                if (arr2->shape[i] == 1) coord = 0;
                off2 += coord * strides2[i];
            }
        }

        void *p1 = data1 + off1 * elem_sz;
        void *p2 = data2 + off2 * elem_sz;

        // compare
        int cmp = 0;
        switch (arr1->dtype) {
            case INT8:   cmp = *(int8_t*)p1 <= *(int8_t*)p2; break;
            case INT16:  cmp = *(int16_t*)p1 <= *(int16_t*)p2; break;
            case INT32:  cmp = *(int32_t*)p1 <= *(int32_t*)p2; break;
            case INT64:  cmp = *(int64_t*)p1 <= *(int64_t*)p2; break;
            case UINT8:  cmp = *(uint8_t*)p1 <= *(uint8_t*)p2; break;
            case UINT16: cmp = *(uint16_t*)p1 <= *(uint16_t*)p2; break;
            case UINT32: cmp = *(uint32_t*)p1 <= *(uint32_t*)p2; break;
            case UINT64: cmp = *(uint64_t*)p1 <= *(uint64_t*)p2; break;
            case FLOAT32: cmp = *(float*)p1 <= *(float*)p2; break;
            case FLOAT64: cmp = *(double*)p1 <= *(double*)p2; break;
            case COMPLEX64: {
                float a = cabsf(*(complex float*)p1);
                float b = cabsf(*(complex float*)p2);
                cmp = (a <= b);
                break;
            }
            case COMPLEX128: {
                double a = cabs(*(complex double*)p1);
                double b = cabs(*(complex double*)p2);
                cmp = (a <= b);
                break;
            }
            default: break;
        }

        // writeresult
        if (result->ndim == 0) {
            res_data[0] = cmp ? 1 : 0;
        } else {
            int res_off = 0;
            for (int i = 0; i < result->ndim; ++i) {
                res_off += indices[i] * res_strides[i];
            }
            res_data[res_off] = cmp ? 1 : 0;
        }
    }

    free(strides1); free(strides2); free(res_strides);
    if (indices) free(indices);
    return result;
}

