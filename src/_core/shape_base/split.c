#include "def.h"
#include "array.h"
#include "shape.h"

int* get_split_points(Array *indices, int axis_len, int *num_points) {
    if (indices == NULL) { *num_points = 0; return NULL; }
    if (indices->ndim == 0) {
        int n_sections = 0;
        switch (indices->dtype) {
            case INT8: n_sections = *(int8_t*)indices->data; break; case INT16: n_sections = *(int16_t*)indices->data; break;
            case INT32: n_sections = *(int32_t*)indices->data; break; case INT64: n_sections = *(int64_t*)indices->data; break;
            case UINT8: n_sections = *(uint8_t*)indices->data; break; case UINT16: n_sections = *(uint16_t*)indices->data; break;
            case UINT32: n_sections = *(uint32_t*)indices->data; break; case UINT64: n_sections = *(uint64_t*)indices->data; break;
            default: fprintf(stderr, "split: indices_or_sections must be integer type\n"); return NULL;
        }
        if (n_sections <= 0) { fprintf(stderr, "split: number of sections must be positive\n"); return NULL; }
        if (axis_len % n_sections != 0) { fprintf(stderr, "split: array split does not result in equal division\n"); return NULL; }
        int step = axis_len / n_sections;
        *num_points = n_sections - 1;
        int *points = malloc((*num_points) * sizeof(int));
        if (!points) return NULL;
        for (int i = 1; i < n_sections; i++) points[i-1] = i * step;
        return points;
    } else if (indices->ndim == 1) {
        int len = indices->size;
        *num_points = len;
        int *points = malloc(len * sizeof(int));
        if (!points) return NULL;
        size_t elem_sz = dtype_size(indices->dtype);
        char *data = (char*)indices->data;
        for (int i = 0; i < len; i++) {
            void *ptr = data + i * elem_sz;
            int64_t val;
            switch (indices->dtype) {
                case INT8: val = *(int8_t*)ptr; break; case INT16: val = *(int16_t*)ptr; break;
                case INT32: val = *(int32_t*)ptr; break; case INT64: val = *(int64_t*)ptr; break;
                case UINT8: val = *(uint8_t*)ptr; break; case UINT16: val = *(uint16_t*)ptr; break;
                case UINT32: val = *(uint32_t*)ptr; break; case UINT64: val = *(uint64_t*)ptr; break;
                default: free(points); fprintf(stderr, "split: indices must be integer type\n"); return NULL;
            }
            if (val < 0 || val > axis_len) {
                free(points); fprintf(stderr, "split: split index %lld out of range [0,%d]\n", (long long)val, axis_len); return NULL;
            }
            points[i] = (int)val;
        }
        for (int i = 1; i < len; i++) {
            if (points[i] <= points[i-1]) { free(points); fprintf(stderr, "split: split indices must be strictly increasing\n"); return NULL; }
        }
        return points;
    } else {
        fprintf(stderr, "split: indices_or_sections must be scalar or 1D array\n"); return NULL;
    }
}

Array** split(Array *ary, Array *indices_or_sections, int axis, int *num_splits) {
    if (ary == NULL || num_splits == NULL) { fprintf(stderr, "split: NULL argument\n"); return NULL; }
    int ndim = ary->ndim, ax = axis;
    if (ax < 0) ax += ndim;
    if (ax < 0 || ax >= ndim) { fprintf(stderr, "split: axis %d out of bounds for %dD array\n", axis, ndim); return NULL; }
    int axis_len = ary->shape[ax], num_points;
    int *split_points = get_split_points(indices_or_sections, axis_len, &num_points);
    if (split_points == NULL && num_points == 0) return NULL;
    int n_splits = num_points + 1;
    *num_splits = n_splits;
    Array **sublists = malloc(n_splits * sizeof(Array*));
    if (!sublists) { free(split_points); return NULL; }
    int *sub_shape = malloc(ndim * sizeof(int));
    if (!sub_shape) { free(split_points); free(sublists); return NULL; }
    memcpy(sub_shape, ary->shape, ndim * sizeof(int));
    size_t elem_sz = dtype_size(ary->dtype); char *data = (char*)ary->data;
    int start = 0;
    for (int i = 0; i < n_splits; i++) {
        int end = (i < num_points) ? split_points[i] : axis_len;
        int seg_len = end - start;
        if (seg_len <= 0) { for (int j = 0; j < i; j++) free_array(sublists[j]); free(sublists); free(split_points); free(sub_shape); return NULL; }
        sub_shape[ax] = seg_len;
        Array *sub = create_array(sub_shape, ndim, ary->dtype);
        if (!sub) { for (int j = 0; j < i; j++) free_array(sublists[j]); free(sublists); free(split_points); free(sub_shape); return NULL; }
        int *in_strides = malloc(ndim*sizeof(int)), *out_strides = malloc(ndim*sizeof(int));
        if (!in_strides || !out_strides) { free(in_strides); free(out_strides); free_array(sub); for(int j=0;j<i;j++)free_array(sublists[j]); free(sublists); free(split_points); free(sub_shape); return NULL; }
        compute_strides(ary->shape, ndim, in_strides); compute_strides(sub->shape, ndim, out_strides);
        char *sub_data = (char*)sub->data;
        int outer_ndim = ndim-1;
        int *outer_dims = malloc(outer_ndim*sizeof(int)), *outer_shape = malloc(outer_ndim*sizeof(int)), *outer_indices = calloc(outer_ndim,sizeof(int));
        if (!outer_dims || !outer_shape || !outer_indices) {
            free(in_strides); free(out_strides); free(outer_dims); free(outer_shape); free(outer_indices);
            free_array(sub); for(int j=0;j<i;j++)free_array(sublists[j]); free(sublists); free(split_points); free(sub_shape); return NULL;
        }
        int idx=0; for(int d=0;d<ndim;d++){if(d!=ax){outer_dims[idx]=d;outer_shape[idx]=ary->shape[d];idx++;}}
        int num_slices=1; for(int d=0;d<outer_ndim;d++)num_slices*=outer_shape[d];
        for(int s=0;s<num_slices;s++){
            int base_in=0,base_out=0;
            for(int j=0;j<outer_ndim;j++){base_in+=outer_indices[j]*in_strides[outer_dims[j]];base_out+=outer_indices[j]*out_strides[outer_dims[j]];}
            for(int k=0;k<seg_len;k++){int in_off=base_in+(start+k)*in_strides[ax];int out_off=base_out+k*out_strides[ax];memcpy(sub_data+out_off*elem_sz,data+in_off*elem_sz,elem_sz);}
            int carry=1;for(int j=outer_ndim-1;j>=0&&carry;j--){outer_indices[j]++;if(outer_indices[j]>=outer_shape[j]){outer_indices[j]=0;carry=1;}else carry=0;}
        }
        free(in_strides);free(out_strides);free(outer_dims);free(outer_shape);free(outer_indices);
        sublists[i]=sub; start=end;
    }
    free(split_points); free(sub_shape);
    return sublists;
}
