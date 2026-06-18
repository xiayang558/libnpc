#include "def.h"
#include "array.h"
#include "shape.h"
#include <string.h>

/**
 * rot90 — rotate array counterclockwise by 90 degrees, k times (like numpy.rot90)
 *
 * @param m     Input array (ndim >= 2)
 * @param k     Number of 90-degree rotations (default 1, normalized to [0,3])
 * @param axis1 First rotation axis (-1 defaults to 0)
 * @param axis2 Second rotation axis (-1 defaults to 1)
 * @return Rotated array (copy)
 */
Array* rot90(Array *m, int k, int axis1, int axis2) {
    if (m == NULL) {
        fprintf(stderr, "rot90: NULL array argument\n");
        return NULL;
    }
    if (m->ndim < 2) {
        fprintf(stderr, "rot90: input must have >= 2 dimensions\n");
        return NULL;
    }

    /* Default axes */
    if (axis1 < 0) axis1 = 0;
    if (axis2 < 0) axis2 = 1;
    if (axis1 < 0) axis1 += m->ndim;
    if (axis2 < 0) axis2 += m->ndim;

    if (axis1 == axis2 || axis1 < 0 || axis1 >= m->ndim ||
        axis2 < 0 || axis2 >= m->ndim) {
        fprintf(stderr, "rot90: invalid axes (%d, %d) for %d-d array\n",
                axis1, axis2, m->ndim);
        return NULL;
    }

    k = k % 4;
    if (k < 0) k += 4;
    if (k == 0) return copy_array(m);

    /* Build new shape: swap axis1 and axis2 sizes if k is odd */
    int ndim = m->ndim;
    int *new_shape = malloc(ndim * sizeof(int));
    if (!new_shape) return NULL;
    memcpy(new_shape, m->shape, ndim * sizeof(int));
    if (k % 2 == 1) {
        int tmp = new_shape[axis1];
        new_shape[axis1] = new_shape[axis2];
        new_shape[axis2] = tmp;
    }

    Array *result = create_array(new_shape, ndim, m->dtype);
    if (!result) { free(new_shape); return NULL; }

    size_t elem_sz = dtype_size(m->dtype);
    char *src = (char*)m->data;
    char *dst = (char*)result->data;

    /* Compute strides */
    int *src_strides = malloc(ndim * sizeof(int));
    int *res_strides = malloc(ndim * sizeof(int));
    if (!src_strides || !res_strides) {
        free(src_strides); free(res_strides); free(new_shape);
        free_array(result);
        return NULL;
    }
    compute_strides(m->shape, ndim, src_strides);
    compute_strides(new_shape, ndim, res_strides);

    int *indices = malloc(ndim * sizeof(int));
    if (!indices) {
        free(src_strides); free(res_strides); free(new_shape);
        free_array(result); return NULL;
    }

    int a1_len = m->shape[axis1];
    int a2_len = m->shape[axis2];

    for (int flat = 0; flat < result->size; flat++) {
        /* Decode result indices */
        int tmp = flat;
        for (int i = ndim - 1; i >= 0; i--) {
            indices[i] = tmp % new_shape[i];
            tmp /= new_shape[i];
        }

        /* Map to source indices according to k */
        int src_idx[16];
        memcpy(src_idx, indices, ndim * sizeof(int));

        if (k == 1) {
            /* dst[i][j] = src[j][a2_len-1-i]  (CCW 90) */
            src_idx[axis1] = indices[axis2];
            src_idx[axis2] = a2_len - 1 - indices[axis1];
        } else if (k == 2) {
            /* dst[i][j] = src[a1_len-1-i][a2_len-1-j]  (180) */
            src_idx[axis1] = a1_len - 1 - indices[axis1];
            src_idx[axis2] = a2_len - 1 - indices[axis2];
        } else if (k == 3) {
            /* dst[i][j] = src[a1_len-1-j][i]  (CW 90) */
            src_idx[axis1] = a1_len - 1 - indices[axis2];
            src_idx[axis2] = indices[axis1];
        }

        /* Compute offsets */
        int src_off = 0, dst_off = 0;
        for (int i = 0; i < ndim; i++) {
            src_off += src_idx[i] * src_strides[i];
            dst_off += indices[i] * res_strides[i];
        }
        memcpy(dst + dst_off * elem_sz, src + src_off * elem_sz, elem_sz);
    }

    free(src_strides); free(res_strides); free(indices); free(new_shape);
    return result;
}
