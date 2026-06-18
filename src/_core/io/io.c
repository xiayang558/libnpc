#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "array.h"
#include "shape.h"
#include "io.h"
#include "npzfile.h"


// helper: Generate NPY descr string based on data type
static const char* dtype_to_descr(DataType dtype) {
    switch (dtype) {
        case BOOL:    return "|b1";
        case INT8:    return "|i1";
        case INT16:   return "<i2";
        case INT32:   return "<i4";
        case INT64:   return "<i8";
        case UINT8:   return "|u1";
        case UINT16:  return "<u2";
        case UINT32:  return "<u4";
        case UINT64:  return "<u8";
        case FLOAT32: return "<f4";
        case FLOAT64: return "<f8";
        case COMPLEX64: return "<c8";
        case COMPLEX128: return "<c16";
        default:      return NULL;
    }
}

// helper: Generate NPY header string (Python dict format)
static char* make_npy_header(Array *arr, int *header_len) {
    const char *descr = dtype_to_descr(arr->dtype);
    if (!descr) return NULL;
    // Build shape string, e.g., "(3,4)"
    char shape_str[256] = "(";
    for (int i = 0; i < arr->ndim; ++i) {
        char buf[32];
        sprintf(buf, "%d", arr->shape[i]);
        strcat(shape_str, buf);
        if (i < arr->ndim - 1) strcat(shape_str, ", ");
    }
    strcat(shape_str, ")");
    // Header format: { 'descr': '<i8', 'fortran_order': False, 'shape': (10,), }
    // Note: total header length need not be 16-byte aligned; NPY allows any length
    char header[1024];
    sprintf(header, "{'descr': '%s', 'fortran_order': False, 'shape': %s, }",
            descr, shape_str);
    *header_len = strlen(header);
    return strdup(header);
}

int save_npy(const char *filename, Array *arr) {
    if (!arr || !filename) return -1;
    FILE *fp = fopen(filename, "wb");
    if (!fp) return -1;

    // 1. Generate header
    int header_len;
    char *header = make_npy_header(arr, &header_len);
    if (!header) {
        fclose(fp);
        return -1;
    }
    // Header length alignment: NPY spec uses actual length. No alignment requirement. 
    // For simplicity, use real length. 
    // Write magic number (1,0) version
    uint8_t magic[6] = {0x93, 'N', 'U', 'M', 'P', 'Y'};
    fwrite(magic, 1, 6, fp);
    // Version number (1,0)
    uint8_t version[2] = {1, 0};
    fwrite(version, 1, 2, fp);
    // Header length (2 bytes little-endian)
    uint16_t header_len_le = header_len;
    fwrite(&header_len_le, 2, 1, fp);
    // Write header
    fwrite(header, 1, header_len, fp);
    // Header alignment fill (optional). Does NPY require header length to be padded to a multiple of 64? In practice many files don't pad, but for compatibility, we do no extra fill here.
    // Write data
    size_t elem_sz = dtype_size(arr->dtype);
    size_t data_sz = arr->size * elem_sz;
    fwrite(arr->data, 1, data_sz, fp);
    fclose(fp);
    free(header);
    return 0;
}

Array* load_npy(const char *filename) {
    FILE *fp = fopen(filename, "rb");
    if (!fp) return NULL;
    // Read magic number
    uint8_t magic[6];
    if (fread(magic, 1, 6, fp) != 6) { fclose(fp); return NULL; }
    if (memcmp(magic, "\x93NUMPY", 6) != 0) { fclose(fp); return NULL; }
    uint8_t version[2];
    if (fread(version, 1, 2, fp) != 2) { fclose(fp); return NULL; }
    int major = version[0], minor = version[1];
    if (major != 1) { fclose(fp); return NULL; } // Only supports version 1
    uint16_t header_len;
    if (fread(&header_len, 2, 1, fp) != 1) { fclose(fp); return NULL; }
    char *header = malloc(header_len + 1);
    if (fread(header, 1, header_len, fp) != header_len) {
        free(header); fclose(fp); return NULL;
    }
    header[header_len] = '\0';
    // Parse header (Reuse the earlier parse_npy_header function)
    char *descr = NULL;
    bool fortran_order = false;
    int *shape = NULL;
    int ndim = 0;
    if (parse_npy_header(header, &descr, &fortran_order, &shape, &ndim) != 0) {
        free(header); fclose(fp); return NULL;
    }
    free(header);
    DataType dtype = descr_to_dtype(descr);
    free(descr);
    if (dtype == UNKNOWN) {
        if (shape) free(shape);
        fclose(fp);
        return NULL;
    }
    // Read data portion
    Array *arr = NULL;
    if (ndim == 0) {
        // scalar
        size_t elem_sz = dtype_size(dtype);
        arr = malloc(sizeof(Array));
        arr->data = malloc(elem_sz);
        if (!arr->data) { free(arr); fclose(fp); return NULL; }
        if (fread(arr->data, 1, elem_sz, fp) != elem_sz) {
            free(arr->data); free(arr); fclose(fp); return NULL;
        }
        arr->dtype = dtype;
        arr->shape = NULL;
        arr->ndim = 0;
        arr->size = 1;
        arr->strides = NULL;
        arr->is_view = 0;
    } else {
        arr = create_array(shape, ndim, dtype);
        if (!arr) { fclose(fp); return NULL; }
        size_t data_sz = arr->size * dtype_size(dtype);
        if (fread(arr->data, 1, data_sz, fp) != data_sz) {
            free_array(arr);
            fclose(fp);
            return NULL;
        }
    }
    fclose(fp);
    if (shape) free(shape);
    return arr;
}

NpzFile* load_npz(const char *filename, bool allow_pickle, int64_t max_header_size) {
    return npzfile_open(filename, allow_pickle, max_header_size);
}


// helper: Convert Array to .npy buffer in memory
static void* array_to_npy_buffer(Array *arr, size_t *out_size) {
    // Generate header (Reuse make_npy_header from save_npy)
    int header_len;
    char *header = make_npy_header(arr, &header_len);
    if (!header) return NULL;
    size_t data_sz = arr->size * dtype_size(arr->dtype);
    size_t total = 6 + 2 + 2 + header_len + data_sz;
    void *buf = malloc(total);
    if (!buf) { free(header); return NULL; }
    uint8_t *p = buf;
    memcpy(p, "\x93NUMPY", 6); p += 6;
    p[0] = 1; p[1] = 0; p += 2;
    uint16_t hlen_le = header_len;
    memcpy(p, &hlen_le, 2); p += 2;
    memcpy(p, header, header_len); p += header_len;
    memcpy(p, arr->data, data_sz);
    *out_size = total;
    free(header);
    return buf;
}


int save_npz(const char *filename, int num_arrays, const char **keys, Array **arrays) {
    int err = 0;
    zip_t *zip = zip_open(filename, ZIP_CREATE | ZIP_TRUNCATE, &err);
    if (!zip) return -1;
    for (int i = 0; i < num_arrays; ++i) {
        // Convert array to .npy memory buffer
        size_t npy_size;
        void *npy_data = array_to_npy_buffer(arrays[i], &npy_size);
        if (!npy_data) continue;
        char entry_name[256];
        snprintf(entry_name, sizeof(entry_name), "%s.npy", keys[i]);
        zip_source_t *src = zip_source_buffer(zip, npy_data, npy_size, 1); // 1 = free on close
        if (!src) {
            free(npy_data);
            continue;
        }
        zip_file_add(zip, entry_name, src, ZIP_FL_OVERWRITE);
    }
    zip_close(zip);
    return 0;
}

void* load(const char *filename, bool allow_pickle, int64_t max_header_size, int *out_is_npz) {
    if (!filename || !out_is_npz) return NULL;
    const char *ext = strrchr(filename, '.');
    if (!ext) return NULL;
    *out_is_npz = 0;
    if (strcmp(ext, ".npy") == 0) {
        return load_npy(filename);
    } else if (strcmp(ext, ".npz") == 0) {
        *out_is_npz = 1;
        return load_npz(filename, allow_pickle, max_header_size);
    }
    return NULL;
}

int save(const char *filename, void *obj, int compress) {
    (void)compress; // Ignore compress@param
    if (!filename || !obj) return -1;
    const char *ext = strrchr(filename, '.');
    if (!ext) return -1;
    if (strcmp(ext, ".npy") == 0) {
        return save_npy(filename, (Array*)obj);
    } else if (strcmp(ext, ".npz") == 0) {
        NpzSaveInfo *info = (NpzSaveInfo*)obj;
        return save_npz(filename, info->num_arrays, info->keys, info->arrays);
    }
    return -1;
}