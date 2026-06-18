#include "def.h"
#include "array.h"
#include "shape.h"
#include <math.h>
#include <string.h>

/**
 * pad  --  pad an array (like numpy.pad)
 *
 * Supported modes:
 *   constant   --  fill with constant_value
 *   edge       --  clamp to edge values
 *   reflect    --  mirror reflection (edge elements are repeated once)
 *   symmetric  --  symmetric reflection (edge elements are not repeated)
 *   wrap       --  circular fill
 */

/* compute source coordinate based on mode, return -1 to use constant_value fill */
static int src_coord(int out_coord, int src_len, int pad_before, const char *mode,
                      double *cval_out, double constant_value) {
    int src_idx = out_coord - pad_before;

    if (src_idx >= 0 && src_idx < src_len) {
        /* inside source array, return directly */
        return src_idx;
    }

    if (mode[0] == 'c' || strncmp(mode, "constant", 8) == 0) {
        /* constant: use constant_value outside bounds */
        *cval_out = constant_value;
        return -1;
    }

    if (mode[0] == 'e' || strncmp(mode, "edge", 4) == 0) {
        /* edge: clamp to boundary */
        if (src_idx < 0) return 0;
        return src_len - 1;
    }

    if (mode[0] == 'w' || strncmp(mode, "wrap", 4) == 0) {
        /* wrap: circular */
        int s = src_idx % src_len;
        if (s < 0) s += src_len;
        return s;
    }

    if (mode[0] == 'r' || strncmp(mode, "reflect", 7) == 0) {
        /* reflect: mirror reflection d c b a | a b c d | d c b a */
        /* core region length = 2*src_len - 2 */
        int period = 2 * src_len - 2;
        if (period <= 0) return 0;
        int s = src_idx % period;
        if (s < 0) s += period;
        if (s >= src_len) s = period - s;
        return s;
    }

    if (mode[0] == 's' || strncmp(mode, "symmetric", 9) == 0) {
        /* symmetric: symmetric reflection c b a | a b c d | d c b */
        /* core region length = 2*src_len */
        int period = 2 * src_len;
        if (period <= 0) return 0;
        int s = src_idx % period;
        if (s < 0) s += period;
        if (s >= src_len) s = period - 1 - s;
        return s;
    }

    /* unknown mode: fallback to constant */
    *cval_out = constant_value;
    return -1;
}

Array* pad(Array *arr, Array *pad_width, const char *mode, double constant_value) {
    if (arr == NULL || pad_width == NULL || mode == NULL) {
        fprintf(stderr, "pad: NULL argument\n");
        return NULL;
    }
    int ndim = arr->ndim;
    if (pad_width->ndim != 1 || pad_width->size != ndim * 2) {
        fprintf(stderr, "pad: pad_width must be 1D array of size %d (got size %d)\n",
                ndim * 2, pad_width->size);
        return NULL;
    }

    /* extract pad_width values */
    int64_t *pw = (int64_t*)pad_width->data;
    int pad_before[16], pad_after[16];
    for (int i = 0; i < ndim; i++) {
        pad_before[i] = (int)pw[i * 2];
        pad_after[i]  = (int)pw[i * 2 + 1];
    }

    /* compute output shape */
    int out_shape[16];
    for (int i = 0; i < ndim; i++) {
        out_shape[i] = arr->shape[i] + pad_before[i] + pad_after[i];
    }

    /* create output array */
    Array *result = create_array(out_shape, ndim, arr->dtype);
    if (result == NULL) return NULL;

    size_t elem_sz = dtype_size(arr->dtype);
    char *src = (char*)arr->data;
    char *dst = (char*)result->data;

    /* strides */
    int *src_strides = malloc(ndim * sizeof(int));
    int *dst_strides = malloc(ndim * sizeof(int));
    int *indices = malloc(ndim * sizeof(int));
    if (!src_strides || !dst_strides || !indices) {
        free(src_strides); free(dst_strides); free(indices);
        free_array(result); return NULL;
    }
    compute_strides(arr->shape, ndim, src_strides);
    compute_strides(out_shape, ndim, dst_strides);

    /* traverse output array */
    for (int flat = 0; flat < result->size; flat++) {
        /* decode output coordinates */
        int tmp = flat;
        for (int i = ndim - 1; i >= 0; i--) {
            indices[i] = tmp % out_shape[i];
            tmp /= out_shape[i];
        }

        int use_constant = 0;
        int src_off = 0;
        for (int i = 0; i < ndim; i++) {
            double cval = 0;
            int coord = src_coord(indices[i], arr->shape[i], pad_before[i], mode, &cval, constant_value);
            if (coord < 0) { use_constant = 1; break; }
            src_off += coord * src_strides[i];
        }

        if (use_constant) {
            /* write constant_value by type */
            switch (arr->dtype) {
                case INT8:  *(int8_t*)(dst + flat*elem_sz)  = (int8_t)constant_value; break;
                case INT16: *(int16_t*)(dst + flat*elem_sz) = (int16_t)constant_value; break;
                case INT32: *(int32_t*)(dst + flat*elem_sz) = (int32_t)constant_value; break;
                case INT64: *(int64_t*)(dst + flat*elem_sz) = (int64_t)constant_value; break;
                case UINT8: *(uint8_t*)(dst + flat*elem_sz) = (uint8_t)constant_value; break;
                case UINT16:*(uint16_t*)(dst + flat*elem_sz)= (uint16_t)constant_value; break;
                case UINT32:*(uint32_t*)(dst + flat*elem_sz)= (uint32_t)constant_value; break;
                case UINT64:*(uint64_t*)(dst + flat*elem_sz)= (uint64_t)constant_value; break;
                case FLOAT32:*(float*)(dst + flat*elem_sz)   = (float)constant_value; break;
                case FLOAT64:*(double*)(dst + flat*elem_sz)  = constant_value; break;
                case COMPLEX64:  *(complex float*)(dst + flat*elem_sz)  = (float)constant_value + 0.0f*I; break;
                case COMPLEX128: *(complex double*)(dst + flat*elem_sz) = constant_value + 0.0*I; break;
                default: memset(dst + flat*elem_sz, 0, elem_sz); break;
            }
        } else {
            memcpy(dst + flat * elem_sz, src + src_off * elem_sz, elem_sz);
        }
    }

    free(src_strides); free(dst_strides); free(indices);
    return result;
}
