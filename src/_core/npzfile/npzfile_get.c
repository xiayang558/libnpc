#include "npzfile.h"
#include "array.h"
#include <stdio.h>
#include <stdlib.h>

bool npzfile_contains(NpzFile *npz, const char *key);
void* zip_read_file(zip_t *zip, const char *name, size_t *out_size);
bool is_npy_magic(const unsigned char *magic);
Array* read_array_from_buffer(const void *buffer, size_t size, bool allow_pickle, int64_t max_header_size);

Array* npzfile_get(NpzFile *npz, const char *key) {
    if (!npzfile_contains(npz, key)) return NULL;
    char fname[512];
    snprintf(fname, sizeof(fname), "%s.npy", key);
    size_t size;
    void *buf = zip_read_file(npz->zip, fname, &size);
    if (!buf) return NULL;
    if (size < 6 || !is_npy_magic(buf)) { free(buf); return NULL; }
    Array *arr = read_array_from_buffer(buf, size, npz->allow_pickle, npz->max_header_size);
    free(buf);
    return arr;
}
