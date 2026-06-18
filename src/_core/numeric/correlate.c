#include "def.h"
#include "array.h"
#include <string.h>

static int check_correlate_inputs(Array *a, Array *v) {
    if (a == NULL || v == NULL) { fprintf(stderr, "correlate: NULL array argument\n"); return 0; }
    if (a->ndim != 1 || v->ndim != 1) { fprintf(stderr, "correlate: inputs must be 1-dimensional\n"); return 0; }
    if (a->dtype != v->dtype) { fprintf(stderr, "correlate: data types must match\n"); return 0; }
    return 1;
}

static int get_correlate_length(int len_a, int len_v, const char *mode) {
    if (strcmp(mode, "full") == 0) return len_a + len_v - 1;
    else if (strcmp(mode, "same") == 0) return len_a;
    else if (strcmp(mode, "valid") == 0) { if (len_a >= len_v) return len_a - len_v + 1; else return 0; }
    else { fprintf(stderr, "correlate: mode must be 'full', 'same', or 'valid'\n"); return -1; }
}

Array* correlate(Array *a, Array *v, const char *mode) {
    if (!check_correlate_inputs(a, v)) return NULL;
    int len_a = a->size, len_v = v->size;
    int out_len = get_correlate_length(len_a, len_v, mode);
    if (out_len < 0) return NULL;
    if (out_len == 0) { int shape[1] = {0}; return create_array(shape, 1, a->dtype); }
    Array *result = create_array((int[]){out_len}, 1, a->dtype);
    if (result == NULL) { fprintf(stderr, "correlate: failed to create result array\n"); return NULL; }
    size_t elem_size = dtype_size(a->dtype);
    char *data_a = (char*)a->data, *data_v = (char*)v->data, *data_out = (char*)result->data;
    for (int i = 0; i < out_len; i++) {
        int start_a, start_v, n;
        if (strcmp(mode, "full") == 0) {
            start_a = 0; start_v = len_v - 1 - i;
            if (start_v < 0) { start_a = -start_v; start_v = 0; }
            n = len_v - (start_v > 0 ? start_v : 0);
            if (len_a - start_a < n) n = len_a - start_a;
            if (len_v - start_v < n) n = len_v - start_v;
        } else if (strcmp(mode, "same") == 0) {
            int offset = (len_v - 1) / 2;
            start_a = i - offset;
            if (start_a < 0) { start_v = -start_a; start_a = 0; }
            else start_v = 0;
            n = len_v - start_v;
            if (start_a + n > len_a) n = len_a - start_a;
        } else { start_a = i; start_v = 0; n = len_v; }
        if (n <= 0) { memset(data_out + i * elem_size, 0, elem_size); continue; }
        switch (a->dtype) {
            case INT8:   { int8_t *pa=(int8_t*)data_a,*pv=(int8_t*)data_v;int64_t s=0;for(int k=0;k<n;k++)s+=(int64_t)pa[start_a+k]*(int64_t)pv[start_v+k];memcpy(data_out+i*elem_size,&s,sizeof(int64_t));break; }
            case INT16:  { int16_t *pa=(int16_t*)data_a,*pv=(int16_t*)data_v;int64_t s=0;for(int k=0;k<n;k++)s+=(int64_t)pa[start_a+k]*(int64_t)pv[start_v+k];*(int64_t*)(data_out+i*elem_size)=s;break; }
            case INT32:  { int32_t *pa=(int32_t*)data_a,*pv=(int32_t*)data_v;int64_t s=0;for(int k=0;k<n;k++)s+=(int64_t)pa[start_a+k]*(int64_t)pv[start_v+k];*(int64_t*)(data_out+i*elem_size)=s;break; }
            case INT64:  { int64_t *pa=(int64_t*)data_a,*pv=(int64_t*)data_v;int64_t s=0;for(int k=0;k<n;k++)s+=pa[start_a+k]*pv[start_v+k];*(int64_t*)(data_out+i*elem_size)=s;break; }
            case UINT8:  { uint8_t *pa=(uint8_t*)data_a,*pv=(uint8_t*)data_v;uint64_t s=0;for(int k=0;k<n;k++)s+=(uint64_t)pa[start_a+k]*(uint64_t)pv[start_v+k];*(uint64_t*)(data_out+i*elem_size)=s;break; }
            case FLOAT32:{ float *pa=(float*)data_a,*pv=(float*)data_v;float s=0;for(int k=0;k<n;k++)s+=pa[start_a+k]*pv[start_v+k];*(float*)(data_out+i*elem_size)=s;break; }
            case FLOAT64:{ double *pa=(double*)data_a,*pv=(double*)data_v;double s=0;for(int k=0;k<n;k++)s+=pa[start_a+k]*pv[start_v+k];*(double*)(data_out+i*elem_size)=s;break; }
            case COMPLEX64:{ complex float *pa=(complex float*)data_a,*pv=(complex float*)data_v;complex float s=0;for(int k=0;k<n;k++)s+=pa[start_a+k]*conjf(pv[start_v+k]);*(complex float*)(data_out+i*elem_size)=s;break; }
            case COMPLEX128:{ complex double *pa=(complex double*)data_a,*pv=(complex double*)data_v;complex double s=0;for(int k=0;k<n;k++)s+=pa[start_a+k]*conj(pv[start_v+k]);*(complex double*)(data_out+i*elem_size)=s;break; }
            default: fprintf(stderr, "correlate: unsupported data type\n"); free_array(result); return NULL;
        }
    }
    return result;
}
