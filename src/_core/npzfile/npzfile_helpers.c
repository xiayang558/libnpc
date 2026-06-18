#include "npzfile.h"
#include "array.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zip.h>

/* shared static helpers for npzfile */

void* zip_read_file(zip_t *zip, const char *name, size_t *out_size) {
    zip_stat_t st;
    if (zip_stat(zip, name, 0, &st) != 0) return NULL;
    zip_file_t *zf = zip_fopen(zip, name, 0);
    if (!zf) return NULL;
    void *buf = malloc(st.size);
    if (!buf) { zip_fclose(zf); return NULL; }
    zip_int64_t n = zip_fread(zf, buf, st.size);
    if (n != (zip_int64_t)st.size) { free(buf); zip_fclose(zf); return NULL; }
    zip_fclose(zf);
    *out_size = st.size;
    return buf;
}

bool is_npy_magic(const unsigned char *magic) {
    static const unsigned char npy_magic[] = {0x93, 'N', 'U', 'M', 'P', 'Y'};
    return memcmp(magic, npy_magic, 6) == 0;
}

int read_array_data(Array *arr, const uint8_t *data, size_t data_len, bool fortran_order) {
    size_t elem_sz = dtype_size(arr->dtype);
    size_t expected = arr->size * elem_sz;
    if (data_len < expected) return -1;
    if (!fortran_order) { memcpy(arr->data, data, expected); }
    else if (arr->ndim != 2) { memcpy(arr->data, data, expected); }
    else {
        int rows = arr->shape[0], cols = arr->shape[1];
        char *src = (char*)data, *dst = (char*)arr->data;
        for (int i = 0; i < rows; ++i)
            for (int j = 0; j < cols; ++j)
                memcpy(dst + (i*cols + j)*elem_sz, src + (j*rows + i)*elem_sz, elem_sz);
    }
    return 0;
}

Array* read_array_from_buffer(const void *buffer, size_t size, bool allow_pickle, int64_t max_header_size) {
    (void)allow_pickle; (void)max_header_size;
    const uint8_t *buf = (const uint8_t*)buffer;
    if (size < 10) { fprintf(stderr, "Buffer too small\n"); return NULL; }
    if (buf[0] != 0x93 || buf[1] != 'N' || buf[2] != 'U' || buf[3] != 'M' || buf[4] != 'P' || buf[5] != 'Y') {
        fprintf(stderr, "Invalid NPY magic\n"); return NULL;
    }
    int major = buf[6], minor = buf[7];
    if (major != 1 && major != 2) { fprintf(stderr, "Unsupported NPY version %d.%d\n", major, minor); return NULL; }
    int header_len;
    if (major == 1) header_len = *(uint16_t*)(buf + 8);
    else header_len = *(uint32_t*)(buf + 8);
    if (header_len <= 0 || header_len > (int)size - 10) { fprintf(stderr, "Invalid header length\n"); return NULL; }
    char *header = malloc(header_len + 1);
    memcpy(header, buf + 10, header_len); header[header_len] = '\0';
    char *descr = NULL; bool fortran_order = false; int *shape = NULL; int ndim = 0;
    if (parse_npy_header(header, &descr, &fortran_order, &shape, &ndim) != 0) { free(header); return NULL; }
    free(header);
    DataType dtype = descr_to_dtype(descr); free(descr);
    if (dtype == UNKNOWN) { if(shape)free(shape); return NULL; }
    size_t data_offset = 10 + header_len, data_len = size - data_offset;
    if (ndim == 0) {
        size_t elem_sz = dtype_size(dtype);
        if (data_len < elem_sz) { fprintf(stderr, "Insufficient data for scalar\n"); free(shape); return NULL; }
        Array *scalar = malloc(sizeof(Array));
        if (!scalar) { free(shape); return NULL; }
        scalar->data = malloc(elem_sz);
        if (!scalar->data) { free(scalar); free(shape); return NULL; }
        memcpy(scalar->data, buf + data_offset, elem_sz);
        scalar->dtype = dtype; scalar->shape = NULL; scalar->ndim = 0;
        scalar->size = 1; scalar->strides = NULL; scalar->is_view = 0;
        free(shape); return scalar;
    }
    Array *arr = create_array(shape, ndim, dtype); free(shape);
    if (!arr) return NULL;
    size_t expected = arr->size * dtype_size(dtype);
    if (data_len < expected) { fprintf(stderr, "Data size mismatch: expected %zu, got %zu\n", expected, data_len); free_array(arr); return NULL; }
    if (!fortran_order) { memcpy(arr->data, buf + data_offset, expected); }
    else if (arr->ndim == 1) { memcpy(arr->data, buf + data_offset, expected); }
    else if (arr->ndim == 2) {
        int rows = arr->shape[0], cols = arr->shape[1]; size_t es = dtype_size(dtype);
        char *src = (char*)(buf + data_offset), *dst = (char*)arr->data;
        for (int i = 0; i < rows; ++i) for (int j = 0; j < cols; ++j)
            memcpy(dst + (i*cols + j)*es, src + (j*rows + i)*es, es);
    } else { memcpy(arr->data, buf + data_offset, expected); }
    return arr;
}

void npzfile_parse_entries(NpzFile *npz) {
    zip_int64_t num = zip_get_num_entries(npz->zip, 0);
    if (num < 0) return;
    npz->n_files = (int)num;
    npz->files = malloc(num * sizeof(char*));
    npz->npy_keys = malloc(num * sizeof(char*));
    int npy_cnt = 0;
    for (int i = 0; i < num; ++i) {
        const char *name = zip_get_name(npz->zip, i, 0);
        if (!name) continue;
        npz->files[i] = strdup(name);
        if (name && strlen(name) > 4 && strcmp(name + strlen(name) - 4, ".npy") == 0) {
            char *key = strdup(name); key[strlen(key)-4] = '\0';
            npz->npy_keys[npy_cnt++] = key;
        } else { npz->npy_keys[i] = NULL; }
    }
    npz->n_npy_keys = npy_cnt;
}
