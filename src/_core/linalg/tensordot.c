#include "array.h"


/* --------------------------------- tensordot ------------------------------------ */
// helper: convert axes array (0D or 1D integer) to integer list
static int* parse_axes(Array *axes, int *len) {
    if (axes->ndim == 0) {
        // scalar, treat as length-1 single-element list
        *len = 1;
        int *res = malloc(sizeof(int));
        if (!res) return NULL;
        switch (axes->dtype) {
            case INT8:   *res = *(int8_t*)axes->data; break;
            case INT16:  *res = *(int16_t*)axes->data; break;
            case INT32:  *res = *(int32_t*)axes->data; break;
            case INT64:  *res = (int)*(int64_t*)axes->data; break;
            case UINT8:  *res = *(uint8_t*)axes->data; break;
            case UINT16: *res = *(uint16_t*)axes->data; break;
            case UINT32: *res = *(uint32_t*)axes->data; break;
            case UINT64: *res = (int)*(uint64_t*)axes->data; break;
            default: free(res); return NULL;
        }
        return res;
    } else if (axes->ndim == 1) {
        *len = axes->size;
        int *res = malloc((*len) * sizeof(int));
        if (!res) return NULL;
        size_t elem_sz = dtype_size(axes->dtype);
        char *data = (char*)axes->data;
        for (int i = 0; i < *len; i++) {
            void *ptr = data + i * elem_sz;
            switch (axes->dtype) {
                case INT8:   res[i] = *(int8_t*)ptr; break;
                case INT16:  res[i] = *(int16_t*)ptr; break;
                case INT32:  res[i] = *(int32_t*)ptr; break;
                case INT64:  res[i] = (int)*(int64_t*)ptr; break;
                case UINT8:  res[i] = *(uint8_t*)ptr; break;
                case UINT16: res[i] = *(uint16_t*)ptr; break;
                case UINT32: res[i] = *(uint32_t*)ptr; break;
                case UINT64: res[i] = (int)*(uint64_t*)ptr; break;
                default: free(res); return NULL;
            }
        }
        return res;
    } else {
        fprintf(stderr, "tensordot: axes must be scalar or 1D array\n");
        return NULL;
    }
}

Array* tensordot(Array *arr1, Array *arr2, Array *axes_a, Array *axes_b) {
    if (arr1 == NULL || arr2 == NULL || axes_a == NULL || axes_b == NULL) {
        fprintf(stderr, "tensordot: NULL argument\n");
        return NULL;
    }
    // parse axes
    int len_a, len_b;
    int *a_axes = parse_axes(axes_a, &len_a);
    int *b_axes = parse_axes(axes_b, &len_b);
    if (a_axes == NULL || b_axes == NULL) {
        free(a_axes); free(b_axes);
        return NULL;
    }
    if (len_a != len_b) {
        fprintf(stderr, "tensordot: axes lengths must be equal\n");
        free(a_axes); free(b_axes);
        return NULL;
    }
    // check axes ranges
    for (int i = 0; i < len_a; i++) {
        if (a_axes[i] < 0) a_axes[i] += arr1->ndim;
        if (b_axes[i] < 0) b_axes[i] += arr2->ndim;
        if (a_axes[i] < 0 || a_axes[i] >= arr1->ndim) {
            fprintf(stderr, "tensordot: axis %d out of range for arr1\n", a_axes[i]);
            free(a_axes); free(b_axes);
            return NULL;
        }
        if (b_axes[i] < 0 || b_axes[i] >= arr2->ndim) {
            fprintf(stderr, "tensordot: axis %d out of range for arr2\n", b_axes[i]);
            free(a_axes); free(b_axes);
            return NULL;
        }
    }

    // compute new shape: arr1 non-contracted axes (in original order), arr2 non-contracted axes (in original order)
    int keep_a_ndim = arr1->ndim - len_a;
    int keep_b_ndim = arr2->ndim - len_b;
    int *keep_a_dims = malloc(keep_a_ndim * sizeof(int));
    int *keep_b_dims = malloc(keep_b_ndim * sizeof(int));
    int keep_a_idx = 0, keep_b_idx = 0;
    // mark contracted axes
    int *a_shrink = calloc(arr1->ndim, sizeof(int));
    for (int i = 0; i < len_a; i++) a_shrink[a_axes[i]] = 1;
    int *b_shrink = calloc(arr2->ndim, sizeof(int));
    for (int i = 0; i < len_b; i++) b_shrink[b_axes[i]] = 1;
    for (int i = 0; i < arr1->ndim; i++) if (!a_shrink[i]) keep_a_dims[keep_a_idx++] = arr1->shape[i];
    for (int i = 0; i < arr2->ndim; i++) if (!b_shrink[i]) keep_b_dims[keep_b_idx++] = arr2->shape[i];

    // output shape = keep_a_dims + keep_b_dims
    int out_ndim = keep_a_ndim + keep_b_ndim;
    int *out_shape = malloc(out_ndim * sizeof(int));
    memcpy(out_shape, keep_a_dims, keep_a_ndim * sizeof(int));
    memcpy(out_shape + keep_a_ndim, keep_b_dims, keep_b_ndim * sizeof(int));

    // create result array, type promotion (simplified here, using arr1 type)
    size_t elem_sz = dtype_size(arr1->dtype);
    Array *result;
    if (out_ndim == 0) {
        // Create scalar array
        result = (Array*)malloc(sizeof(Array));
        if (result == NULL) {
            free(a_axes); free(b_axes); free(keep_a_dims); free(keep_b_dims);
            free(a_shrink); free(b_shrink); free(out_shape);
            return NULL;
        }
        result->data = malloc(elem_sz);
        if (result->data == NULL) {
            free(result);
            free(a_axes); free(b_axes); free(keep_a_dims); free(keep_b_dims);
            free(a_shrink); free(b_shrink); free(out_shape);
            return NULL;
        }
        result->dtype = arr1->dtype;
        result->shape = NULL;
        result->ndim = 0;
        result->size = 1;
        result->strides = NULL;
        result->is_view = 0;
    } else {
        result = create_array(out_shape, out_ndim, arr1->dtype);
        if (result == NULL) {
            free(a_axes); free(b_axes); free(keep_a_dims); free(keep_b_dims);
            free(a_shrink); free(b_shrink); free(out_shape);
            return NULL;
        }
    }

    // compute contracted dimensions total size (used in inner loop)
    int shrink_size = 1;
    for (int i = 0; i < len_a; i++) shrink_size *= arr1->shape[a_axes[i]];

    // compute strides
    int *a_strides = malloc(arr1->ndim * sizeof(int));
    int *b_strides = malloc(arr2->ndim * sizeof(int));
    int *res_strides = malloc(out_ndim * sizeof(int));
    compute_strides(arr1->shape, arr1->ndim, a_strides);
    compute_strides(arr2->shape, arr2->ndim, b_strides);
    compute_strides(result->shape, out_ndim, res_strides);

    
    char *a_data = (char*)arr1->data;
    char *b_data = (char*)arr2->data;
    char *res_data = (char*)result->data;

    // outer ndim: combinations of arr1 and arr2 kept ndim
    int total_keep = result->size;
    // allocate temporary index array
    int *a_indices = malloc(arr1->ndim * sizeof(int));
    int *b_indices = malloc(arr2->ndim * sizeof(int));
    int *res_indices = malloc(out_ndim * sizeof(int));

    // double loop over all combinations of kept ndim
    for (int res_flat = 0; res_flat < total_keep; res_flat++) {
        // convert res_flat to res_indices
        int tmp = res_flat;
        for (int i = out_ndim - 1; i >= 0; i--) {
            res_indices[i] = tmp % result->shape[i];
            tmp /= result->shape[i];
        }
        // build arr1 index (keep ndim values from front keep_a_ndim of res_indices)
        int a_idx_pos = 0;
        for (int i = 0; i < arr1->ndim; i++) {
            if (a_shrink[i]) {
                a_indices[i] = 0; // temporary placeholder, traversed later
            } else {
                a_indices[i] = res_indices[a_idx_pos++];
            }
        }
        // build arr2 index (keep ndim values from back keep_b_ndim of res_indices)
        int b_idx_pos = 0;
        for (int i = 0; i < arr2->ndim; i++) {
            if (b_shrink[i]) {
                b_indices[i] = 0;
            } else {
                b_indices[i] = res_indices[keep_a_ndim + b_idx_pos++];
            }
        }

        // accumulate sum over contracted dimensions
        double sum = 0.0;  // hardcoded float64, should actually handle different types
        int *shrink_coords = calloc(len_a, sizeof(int));
        for (int s = 0; s < shrink_size; s++) {
            // convert s to contracted coordinates
            int tmp_s = s;
            for (int i = len_a - 1; i >= 0; i--) {
                shrink_coords[i] = tmp_s % arr1->shape[a_axes[i]];
                tmp_s /= arr1->shape[a_axes[i]];
            }
            // set contracted axis indices for arr1 and arr2
            for (int i = 0; i < len_a; i++) {
                a_indices[a_axes[i]] = shrink_coords[i];
                b_indices[b_axes[i]] = shrink_coords[i];
            }
            // compute arr1 flat index
            // use pre-computed stride array to compute flat index
            int a_flat = 0;
            for (int i = 0; i < arr1->ndim; i++) {
                a_flat += a_indices[i] * a_strides[i];
            }
            int b_flat = 0;
            for (int i = 0; i < arr2->ndim; i++) {
                b_flat += b_indices[i] * b_strides[i];
            }
            // accumulate product
            double aval, bval;
            switch (arr1->dtype) {
                case FLOAT64: aval = *(double*)(a_data + a_flat * elem_sz); break;
                default: aval = 0.0;
            }
            switch (arr2->dtype) {
                case FLOAT64: bval = *(double*)(b_data + b_flat * elem_sz); break;
                default: bval = 0.0;
            }
            sum += aval * bval;
        }
        free(shrink_coords);

        // write result
        int res_flat_idx = 0;
        for (int i = 0; i < out_ndim; i++) {
            res_flat_idx += res_indices[i] * res_strides[i];
        }
        *(double*)(res_data + res_flat_idx * elem_sz) = sum;
    }

    free(a_axes); free(b_axes);
    free(keep_a_dims); free(keep_b_dims);
    free(a_shrink); free(b_shrink); free(out_shape);
    free(a_strides); free(b_strides); free(res_strides);
    free(a_indices); free(b_indices); free(res_indices);
    return result;
}


Array* tensordot_scalar(Array *arr1, Array *arr2, int axes) {
    if (axes < 0 || axes > arr1->ndim || axes > arr2->ndim) {
        fprintf(stderr, "tensordot_scalar: axes out of range\n");
        return NULL;
    }
    int *axes_a = malloc(axes * sizeof(int));
    int *axes_b = malloc(axes * sizeof(int));
    for (int i = 0; i < axes; i++) {
        axes_a[i] = arr1->ndim - axes + i;   // last axes dimensions
        axes_b[i] = i;                    // first axes dimensions
    }
    Array *axes_arr_a = create_array(&axes, 1, INT32);
    memcpy(axes_arr_a->data, axes_a, axes * sizeof(int));
    Array *axes_arr_b = create_array(&axes, 1, INT32);
    memcpy(axes_arr_b->data, axes_b, axes * sizeof(int));
    Array *result = tensordot(arr1, arr2, axes_arr_a, axes_arr_b);
    free(axes_a); free(axes_b);
    free_array(axes_arr_a); free_array(axes_arr_b);
    return result;
}