#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "array.h"
#include "utils.h"

/*
 * Helper: check if a value at ptr is truthy (non-zero)
 */
static int is_truthy(void *ptr, DataType dtype) {
    switch (dtype) {
        case BOOL:   return *(uint8_t*)ptr != 0;
        case INT8:   return *(int8_t*)ptr != 0;
        case INT16:  return *(int16_t*)ptr != 0;
        case INT32:  return *(int32_t*)ptr != 0;
        case INT64:  return *(int64_t*)ptr != 0;
        case UINT8:  return *(uint8_t*)ptr != 0;
        case UINT16: return *(uint16_t*)ptr != 0;
        case UINT32: return *(uint32_t*)ptr != 0;
        case UINT64: return *(uint64_t*)ptr != 0;
        case FLOAT32: return *(float*)ptr != 0.0f;
        case FLOAT64: return *(double*)ptr != 0.0;
        case COMPLEX64:  return (*(complex float*)ptr) != 0.0f;
        case COMPLEX128: return (*(complex double*)ptr) != 0.0;
        default: return 0;
    }
}

/*
 * Helper: promote dtype for x/y mixing
 */
static DataType promote_dtype(DataType a, DataType b) {
    if (a == COMPLEX128 || b == COMPLEX128) return COMPLEX128;
    if (a == COMPLEX64  || b == COMPLEX64)  return COMPLEX128;
    if (a == FLOAT64 || b == FLOAT64) return FLOAT64;
    if (a == FLOAT32 || b == FLOAT32) return FLOAT32;
    if (a == INT64 || b == INT64) return INT64;
    if (a == UINT64 || b == UINT64) return FLOAT64;
    if (a == INT32 || b == INT32) return INT32;
    if (a == UINT32 || b == UINT32) return INT64;
    return INT32;
}

/*
 * Compare str with s; returns 1 if prefix matches and length matches
 * (or s[0]==0 for empty string)
 */
static int streq(const char *a, const char *b) {
    return strcmp(a, b) == 0;
}

/**
 * where  --  condition-based selection, like numpy.where
 *
 * Two calling modes:
 *   where(cond, x, y)  -> select values from x/y based on cond true/false (x,y not NULL)
 *   where(cond, NULL, NULL) -> return indices of truthy elements (like argwhere)
 *
 * @param condition condition array
 * @param x         value to take when condition is true (may be NULL)
 * @param y         value to take when condition is false (may be NULL)
 * @return selection result or index array
 */
Array* where(Array *condition, Array *x, Array *y) {
    if (condition == NULL) {
        fprintf(stderr, "where: NULL condition\n");
        return NULL;
    }

    /* --- indices-only mode: where(cond, NULL, NULL) --- */
    if (x == NULL && y == NULL) {
        /* Count truthy elements */
        size_t esz = dtype_size(condition->dtype);
        char *cd = (char*)condition->data;
        int count = 0;
        for (int i = 0; i < condition->size; i++) {
            if (is_truthy(cd + i * esz, condition->dtype)) count++;
        }

        int out_shape[2] = {count, condition->ndim};
        Array *res = create_array(out_shape, 2, INT64);
        if (!res) return NULL;
        if (count == 0) return res;

        /* strides for index decode */
        int *strides = malloc(condition->ndim * sizeof(int));
        if (!strides) { free_array(res); return NULL; }
        compute_strides(condition->shape, condition->ndim, strides);

        int64_t *rd = (int64_t*)res->data;
        int row = 0;
        for (int flat = 0; flat < condition->size; flat++) {
            if (is_truthy(cd + flat * esz, condition->dtype)) {
                int tmp = flat;
                for (int d = condition->ndim - 1; d >= 0; d--) {
                    rd[row * condition->ndim + d] = tmp % condition->shape[d];
                    tmp /= condition->shape[d];
                }
                row++;
            }
        }
        free(strides);
        return res;
    }

    /* --- selection mode: where(cond, x, y) --- */
    if (x == NULL || y == NULL) {
        fprintf(stderr, "where: both x and y must be provided for selection mode\n");
        return NULL;
    }

    /* Determine output dtype: promote x.dtype (for signals) with y.dtype */
    DataType out_dtype = promote_dtype(x->dtype, y->dtype);

    /* Broadcast condition, x, y together */
    Array *arrays[3] = {condition, x, y};
    int out_ndim;
    int *out_shape = broadcast_shapes(3, arrays, &out_ndim);
    if (!out_shape) {
        fprintf(stderr, "where: broadcast failed\n");
        return NULL;
    }

    Array *result;
    if (out_ndim == 0) {
        int one = 1;
        result = create_array(&one, 1, out_dtype);
    } else {
        result = create_array(out_shape, out_ndim, out_dtype);
    }
    free(out_shape);
    if (!result) return NULL;

    /* Resolve all inputs to out_dtype for simplicity */
    Array *cx = astype(x, out_dtype);
    Array *cy = astype(y, out_dtype);
    if (!cx || !cy) { if (cx) free_array(cx); if (cy) free_array(cy); free_array(result); return NULL; }

    size_t csz = dtype_size(condition->dtype);
    size_t osz = dtype_size(out_dtype);
    char *cd = (char*)condition->data;
    char *xd = (char*)cx->data;
    char *yd = (char*)cy->data;
    char *rd = (char*)result->data;

    int *st_c = NULL, *st_x = NULL, *st_y = NULL, *st_r = NULL;
    if (condition->ndim > 0) { st_c = malloc(condition->ndim * sizeof(int)); compute_strides(condition->shape, condition->ndim, st_c); }
    if (cx->ndim > 0) { st_x = malloc(cx->ndim * sizeof(int)); compute_strides(cx->shape, cx->ndim, st_x); }
    if (cy->ndim > 0) { st_y = malloc(cy->ndim * sizeof(int)); compute_strides(cy->shape, cy->ndim, st_y); }
    if (result->ndim > 0) { st_r = malloc(result->ndim * sizeof(int)); compute_strides(result->shape, result->ndim, st_r); }

    int *indices = NULL;
    if (result->ndim > 0) indices = calloc(result->ndim, sizeof(int));

    for (int flat = 0; flat < result->size; flat++) {
        if (indices) {
            int tmp = flat;
            for (int i = result->ndim - 1; i >= 0; i--) {
                indices[i] = tmp % result->shape[i];
                tmp /= result->shape[i];
            }
        }

        /* Compute offset into each array with broadcasting */
        int off_c = 0, off_x = 0, off_y = 0;
        if (indices && condition->ndim > 0)
            for (int i = 0; i < condition->ndim; i++) {
                int idx = indices[result->ndim - condition->ndim + i];
                if (condition->shape[i] == 1) idx = 0;
                off_c += idx * st_c[i];
            }
        if (indices && cx->ndim > 0)
            for (int i = 0; i < cx->ndim; i++) {
                int idx = indices[result->ndim - cx->ndim + i];
                if (cx->shape[i] == 1) idx = 0;
                off_x += idx * st_x[i];
            }
        if (indices && cy->ndim > 0)
            for (int i = 0; i < cy->ndim; i++) {
                int idx = indices[result->ndim - cy->ndim + i];
                if (cy->shape[i] == 1) idx = 0;
                off_y += idx * st_y[i];
            }

        int truthy = is_truthy(cd + off_c * csz, condition->dtype);
        memcpy(rd + flat * osz, truthy ? (xd + off_x * osz) : (yd + off_y * osz), osz);
    }

    free(st_c); free(st_x); free(st_y); free(st_r); free(indices);
    free_array(cx); free_array(cy);
    return result;
}
