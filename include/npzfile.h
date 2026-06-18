#ifndef NPZFILE_H
#define NPZFILE_H

#include <stdint.h>
#include <stdbool.h>
#include <zip.h>
#include "array.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct BagObj BagObj;
typedef struct NpzFile NpzFile;

/**
 * @brief Similar to Python's BagObj, supports attribute-style access (e.g. npz.f.x)
 */
struct BagObj {
    NpzFile *parent;       // pointer to the owning NpzFile object
};

/**
 * @brief NpzFile struct, mimics numpy.lib.npyio.NpzFile
 */
struct NpzFile {
    char *filename;                // filename (for display)
    zip_t *zip;                    // libzip handle
    char **files;                  // all entry names (including .npy and other files)
    char **npy_keys;               // base names of .npy files only (without extension)
    int n_files;                   // count of files
    int n_npy_keys;                // count of npy_keys
    BagObj *f;                     // object for attribute access
    bool own_fid;                  // whether fid should be closed (if using FILE*)
    void *fid;                     // raw file handle (FILE* or path string)
    bool allow_pickle;             // whether to allow loading pickle (default false)
    int64_t max_header_size;       // maximum header size
    void *pickle_kwargs;           // not yet supported
};

/**
 * @brief Create an NpzFile object from a file path
 * @param path   Path to the .npz file
 * @return       New NpzFile object; must be freed by calling npzfile_close
 */
NpzFile* npzfile_open(const char *path, bool allow_pickle, int64_t max_header_size);

/**
 * @brief Create an NpzFile object from a FILE* handle
 * @param fp      Open FILE* (must be readable and support fseek)
 * @param own_fid Whether to close fp on close
 * @return        New NpzFile object
 */
NpzFile* npzfile_open_fp(FILE *fp, bool own_fid, bool allow_pickle, int64_t max_header_size);

/**
 * @brief Close the NpzFile and release resources
 */
void npzfile_close(NpzFile *npz);

/**
 * @brief Get all keys (base names of .npy files)
 */
const char** npzfile_keys(NpzFile *npz, int *out_count);

/**
 * @brief Check whether a given key exists
 */
bool npzfile_contains(NpzFile *npz, const char *key);

/**
 * @brief Read the array corresponding to a key (lazy load, returns a newly allocated Array)
 * @param npz    NpzFile object
 * @param key    Key name (without .npy extension)
 * @return       Returns Array* (must call free_array), or NULL on failure
 */
Array* npzfile_get(NpzFile *npz, const char *key);

/**
 * @brief Get the BagObj object for attribute access (npz->f->...)
 */
BagObj* npzfile_get_bag(NpzFile *npz);

// BagObj get function
Array* bagobj_get(BagObj *bag, const char *attr);

int parse_npy_header(const char *header, char **descr, bool *fortran_order, int **shape, int *ndim);

DataType descr_to_dtype(const char *descr);
#ifdef __cplusplus
}
#endif

#endif // NPZFILE_H