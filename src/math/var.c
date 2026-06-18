#include "array.h"
#include "utils.h"
#include "shape.h"
#include "function.h"


Array* var(Array *a, int axis, int ddof, int keepdims) {
    if (a == NULL) {
        fprintf(stderr, "var: NULL array argument\n");
        return NULL;
    }
    int ndim = a->ndim;
    if (axis < -1 || axis >= ndim) {
        fprintf(stderr, "var: invalid axis %d for %dD array\n", axis, ndim);
        return NULL;
    }

    // case 1: axis == -1, compute variance of all elements (ignoring ndim)
    if (axis == -1) {
        // flatten array to 1D
        Array *flat = flatten(a);
        if (!flat) return NULL;
        // convert to FLOAT64 (if complex, need special handling)
        int is_complex = (a->dtype == COMPLEX64 || a->dtype == COMPLEX128);
        if (is_complex) {
            // complex variance = mean(|z - mean|^2)
            // first compute mean (complex)
            Array *mean_arr = mean(flat, -1, 1);
            if (!mean_arr) { free_array(flat); return NULL; }
            complex double mean = *(complex double*)mean_arr->data;
            // compute squared modulus of difference per element with mean, then take mean
            double sum_sq = 0.0;
            size_t elem_sz = dtype_size(flat->dtype);
            char *data = (char*)flat->data;
            for (int i = 0; i < flat->size; ++i) {
                void *ptr = data + i * elem_sz;
                complex double z = (flat->dtype == COMPLEX64) ?
                    (complex double)*(complex float*)ptr : *(complex double*)ptr;
                double diff_sq = creal(z - mean) * creal(z - mean) + cimag(z - mean) * cimag(z - mean);
                sum_sq += diff_sq;
            }
            double var_val = sum_sq / (flat->size - ddof);
            free_array(flat);
            free_array(mean_arr);
            // construct result array
            int out_ndim = keepdims ? ndim : 0;
            Array *result;
            if (out_ndim == 0) {
                result = (Array*)malloc(sizeof(Array));
                if (!result) return NULL;
                result->data = malloc(sizeof(double));
                if (!result->data) { free(result); return NULL; }
                result->dtype = FLOAT64;
                result->shape = NULL;
                result->ndim = 0;
                result->size = 1;
                result->strides = NULL;
                result->is_view = 0;
                *(double*)result->data = var_val;
            } else {
                int *out_shape = malloc(ndim * sizeof(int));
                for (int i = 0; i < ndim; ++i) out_shape[i] = 1;
                result = create_array(out_shape, ndim, FLOAT64);
                free(out_shape);
                if (result) *(double*)result->data = var_val;
            }
            return result;
        } else {
            // real-valued case
            Array *mean_arr = mean(flat, -1, 1);
            if (!mean_arr) { free_array(flat); return NULL; }
            double mean = *(double*)mean_arr->data;
            double sum_sq = 0.0;
            size_t elem_sz = dtype_size(flat->dtype);
            char *data = (char*)flat->data;
            for (int i = 0; i < flat->size; ++i) {
                void *ptr = data + i * elem_sz;
                double val = to_double(ptr, flat->dtype);
                double diff = val - mean;
                sum_sq += diff * diff;
            }
            double var_val = sum_sq / (flat->size - ddof);
            free_array(flat);
            free_array(mean_arr);
            int out_ndim = keepdims ? ndim : 0;
            Array *result;
            if (out_ndim == 0) {
                result = (Array*)malloc(sizeof(Array));
                if (!result) return NULL;
                result->data = malloc(sizeof(double));
                if (!result->data) { free(result); return NULL; }
                result->dtype = FLOAT64;
                result->shape = NULL;
                result->ndim = 0;
                result->size = 1;
                result->strides = NULL;
                result->is_view = 0;
                *(double*)result->data = var_val;
            } else {
                int *out_shape = malloc(ndim * sizeof(int));
                for (int i = 0; i < ndim; ++i) out_shape[i] = 1;
                result = create_array(out_shape, ndim, FLOAT64);
                free(out_shape);
                if (result) *(double*)result->data = var_val;
            }
            return result;
        }
    }

    // compute variance along specified axis
    int axis_len = a->shape[axis];
    if (axis_len == 0) {
        // empty axis, return NaN array
        int out_ndim = ndim - (keepdims ? 0 : 1);
        int *out_shape = malloc(out_ndim * sizeof(int));
        int idx = 0;
        for (int i = 0; i < ndim; ++i) {
            if (i == axis && !keepdims) continue;
            out_shape[idx++] = (i == axis && keepdims) ? 1 : a->shape[i];
        }
        Array *result = create_array(out_shape, out_ndim, FLOAT64);
        free(out_shape);
        if (!result) return NULL;
        // fill NaN
        double nan_val = NAN;
        for (int i = 0; i < result->size; ++i) ((double*)result->data)[i] = nan_val;
        return result;
    }

    int is_complex = (a->dtype == COMPLEX64 || a->dtype == COMPLEX128);
    // compute mean (keep dims for broadcast)
    Array *mean_arr = mean(a, axis, 1);
    if (!mean_arr) return NULL;
    // compute sum of squared deviations
    // first create a temporary array to store (x - mean) and square
    Array *diff = subtract(a, mean_arr);
    free_array(mean_arr);
    if (!diff) return NULL;

    Array *squared;
    if (is_complex) {
        // complex square is modulus squared (real-valued)
        // manually compute per-element modulus squared
        squared = create_array(a->shape, a->ndim, FLOAT64);
        if (!squared) { free_array(diff); return NULL; }
        size_t in_elem = dtype_size(diff->dtype);
        size_t out_elem = sizeof(double);
        char *src = (char*)diff->data;
        char *dst = (char*)squared->data;
        for (int i = 0; i < diff->size; ++i) {
            void *ptr = src + i * in_elem;
            double val = complex_abs_sq(ptr, diff->dtype);
            *(double*)(dst + i * out_elem) = val;
        }
        free_array(diff);
    } else {
        squared = multiply(diff, diff);
        free_array(diff);
    }
    if (!squared) return NULL;

    // sum along axis
    Array *sum_sq = sum(squared, axis, 1);
    free_array(squared);
    if (!sum_sq) return NULL;

    // normalize factor = 1 / (axis_len - ddof)
    double factor = 1.0 / (axis_len - ddof);
    Array *factor_arr = create_scalar_array(&factor, FLOAT64);
    if (!factor_arr) { free_array(sum_sq); return NULL; }
    Array *var = multiply_scalar(sum_sq, factor_arr);
    free_array(sum_sq);
    free_array(factor_arr);
    if (!var) return NULL;

    // handle keepdims
    if (!keepdims && var->ndim > 0) {
        // remove length-1 dimensions
        int out_ndim = var->ndim - 1;
        int *out_shape = malloc(out_ndim * sizeof(int));
        int idx = 0;
        for (int i = 0; i < var->ndim; ++i) {
            if (i != axis) out_shape[idx++] = var->shape[i];
        }
        Array *result = reshape(var, out_shape, out_ndim);
        free(out_shape);
        free_array(var);
        return result;
    }
    return var;
}