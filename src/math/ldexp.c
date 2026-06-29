#include "array.h"
#include <math.h>

/**
 * npc_ldexp — Return x * 2^exp element-wise (like numpy.ldexp)
 *
 * x can be any numeric type, exp must be an integer type.
 * Result is always FLOAT64 with broadcast shape.
 *
 * @param x   Input array (any numeric type)
 * @param exp Input array (must be integer type)
 * @return FLOAT64 result array, or NULL on error
 */
Array* npc_ldexp(Array *x, Array *exp) {
    if (x == NULL || exp == NULL) {
        fprintf(stderr, "ldexp: NULL array argument\n");
        return NULL;
    }

    /* exp must be integer type */
    if (!is_integer(exp->dtype)) {
        fprintf(stderr, "ldexp: exp must be integer type, got %s\n",
                dtype_name(exp->dtype));
        return NULL;
    }

    /* complex x not supported */
    if (x->dtype == COMPLEX64 || x->dtype == COMPLEX128 ||
        x->dtype == COMPLEX256) {
        fprintf(stderr, "ldexp: complex numbers not supported\n");
        return NULL;
    }

    /* broadcast shapes */
    int out_ndim;
    Array *arrays[] = {x, exp};
    int *out_shape = broadcast_shapes(2, arrays, &out_ndim);
    if (out_shape == NULL) {
        fprintf(stderr, "ldexp: broadcast failed\n");
        return NULL;
    }

    Array *result;
    if (out_ndim == 0) {
        result = (Array*)malloc(sizeof(Array));
        if (!result) { free(out_shape); return NULL; }
        result->data = malloc(sizeof(double));
        if (!result->data) { free(result); free(out_shape); return NULL; }
        result->dtype = FLOAT64;
        result->shape = NULL;
        result->ndim = 0;
        result->size = 1;
        result->strides = NULL;
        result->is_view = 0;
        *(double*)result->data = 0.0;
    } else {
        result = create_array(out_shape, out_ndim, FLOAT64);
        if (!result) { free(out_shape); return NULL; }
    }
    free(out_shape);

    /* strides */
    int *strides_x = NULL, *strides_exp = NULL, *strides_res = NULL;
    if (x->ndim > 0) { strides_x = malloc(x->ndim * sizeof(int)); compute_strides(x->shape, x->ndim, strides_x); }
    if (exp->ndim > 0) { strides_exp = malloc(exp->ndim * sizeof(int)); compute_strides(exp->shape, exp->ndim, strides_exp); }
    if (result->ndim > 0) { strides_res = malloc(result->ndim * sizeof(int)); compute_strides(result->shape, result->ndim, strides_res); }

    size_t sz_x = dtype_size(x->dtype);
    size_t sz_exp = dtype_size(exp->dtype);
    char *data_x = (char*)x->data;
    char *data_exp = (char*)exp->data;
    double *res_data = (double*)result->data;

    int *indices = NULL;
    if (result->ndim > 0) {
        indices = calloc(result->ndim, sizeof(int));
        if (!indices) { free(strides_x); free(strides_exp); free(strides_res); free_array(result); return NULL; }
    }

    for (int flat = 0; flat < result->size; ++flat) {
        if (result->ndim > 0) {
            int tmp = flat;
            for (int i = result->ndim - 1; i >= 0; --i) {
                indices[i] = tmp % result->shape[i];
                tmp /= result->shape[i];
            }
        }

        /* compute x offset */
        int off_x = 0;
        if (x->ndim > 0 && result->ndim > 0) {
            int dim_offset = result->ndim - x->ndim;
            for (int i = 0; i < x->ndim; ++i) {
                int coord = indices[dim_offset + i];
                if (x->shape[i] == 1) coord = 0;
                off_x += coord * strides_x[i];
            }
        }

        /* compute exp offset */
        int off_exp = 0;
        if (exp->ndim > 0 && result->ndim > 0) {
            int dim_offset = result->ndim - exp->ndim;
            for (int i = 0; i < exp->ndim; ++i) {
                int coord = indices[dim_offset + i];
                if (exp->shape[i] == 1) coord = 0;
                off_exp += coord * strides_exp[i];
            }
        }

        void *p_x = data_x + off_x * sz_x;
        void *p_exp = data_exp + off_exp * sz_exp;

        /* extract x value as double */
        double vx = 0.0;
        switch (x->dtype) {
            case BOOL:   vx = *(uint8_t*)p_x;  break;
            case INT8:   vx = *(int8_t*)p_x;   break;
            case INT16:  vx = *(int16_t*)p_x;  break;
            case INT32:  vx = *(int32_t*)p_x;  break;
            case INT64:  vx = *(int64_t*)p_x;  break;
            case UINT8:  vx = *(uint8_t*)p_x;  break;
            case UINT16: vx = *(uint16_t*)p_x; break;
            case UINT32: vx = *(uint32_t*)p_x; break;
            case UINT64: vx = *(uint64_t*)p_x; break;
            case FLOAT32: vx = *(float*)p_x;  break;
            case FLOAT64: vx = *(double*)p_x; break;
            default: break;
        }

        /* extract exp value as int */
        int vexp = 0;
        switch (exp->dtype) {
            case BOOL:   vexp = *(uint8_t*)p_exp;  break;
            case INT8:   vexp = *(int8_t*)p_exp;   break;
            case INT16:  vexp = *(int16_t*)p_exp;  break;
            case INT32:  vexp = *(int32_t*)p_exp;  break;
            case INT64:  vexp = (int)*(int64_t*)p_exp; break;
            case UINT8:  vexp = *(uint8_t*)p_exp;  break;
            case UINT16: vexp = *(uint16_t*)p_exp; break;
            case UINT32: vexp = (int)*(uint32_t*)p_exp; break;
            case UINT64: vexp = (int)*(uint64_t*)p_exp; break;
            default: break;
        }

        double res = ldexp(vx, vexp);

        if (result->ndim == 0) {
            res_data[0] = res;
        } else {
            int off_res = 0;
            for (int i = 0; i < result->ndim; ++i)
                off_res += indices[i] * strides_res[i];
            res_data[off_res] = res;
        }
    }

    free(strides_x); free(strides_exp); free(strides_res);
    if (indices) free(indices);
    return result;
}
