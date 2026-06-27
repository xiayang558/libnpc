#ifndef NPY_IO_H
#define NPY_IO_H

#include "array.h"
#include "npzfile.h"   // Include NpzFile

#ifdef __cplusplus
extern "C" {
#endif

// helper struct: used to pass multiple array info to the save function (when saving .npz)
typedef struct {
    int num_arrays;
    const char **keys;
    Array **arrays;
} NpzSaveInfo;


/**
 * @brief Save an array as a .npy file
 * @param filename Filename (should end with .npy)
 * @param arr      Array to save
 * @return 0 on success, -1 on failure
 */
int save_npy(const char *filename, Array *arr);

int save_npz(const char *filename, int num_arrays, const char **keys, Array **arrays);

/**
 * @brief Save multiple arrays as a .npz file (similar to numpy.savez)
 * @param filename     Output file path
 * @param num_unnamed  Number of unnamed arrays
 * @param unnamed      Unnamed arrays (auto-named arr_0, arr_1, ...)
 * @param num_named    Number of named arrays
 * @param named_keys   Key names for named arrays
 * @param named_vals   Named arrays
 * @return 0 on success, -1 on failure
 */
int savez(const char *filename, int num_unnamed, Array **unnamed,
          int num_named, const char **named_keys, Array **named_vals);

/**
 * @brief Load an array from a .npy file
 * @param filename Path to the .npy file
 * @return Loaded array, or NULL on failure
 */
Array* load_npy(const char *filename);

/**
 * @brief Load an NpzFile object from a .npz file
 * @param filename        Path to the .npz file
 * @param allow_pickle    Whether to allow loading pickled data (not yet implemented; parameter reserved)
 * @param max_header_size Maximum header size (parameter reserved)
 * @return NpzFile object; must be freed by calling npzfile_close
 */
NpzFile* load_npz(const char *filename, bool allow_pickle, int64_t max_header_size);


// unified load function (auto-selects based on file extension)
// Return value: returns Array* or NpzFile* on success; NULL on failure
// out_is_npz indicates the return type (1 = NpzFile*, 0 = Array*)
void* load(const char *filename, bool allow_pickle, int64_t max_header_size, int *out_is_npz);

// unified save function (auto-selects based on file extension)
// Note: for .npy, obj should be Array*; for .npz, obj should be NpzSaveInfo* struct (see below)
int save(const char *filename, void *obj, int compress);

/**
 * @brief Save an array as a text file (similar to numpy.savetxt)
 * @param filename  Output file path
 * @param arr       Array to save (1-D or 2-D)
 * @param fmt       Format string (e.g. "%.18e", default "%g")
 * @param delimiter Column delimiter (default " ")
 * @param newline   Row delimiter (default "\n")
 * @param header    File header comment (may be NULL)
 * @param footer    File footer comment (may be NULL)
 * @param comments  Comment prefix (default "# "; no comments written if NULL)
 * @return 0 on success, -1 on failure
 */
int savetxt(const char *filename, Array *arr, const char *fmt,
            const char *delimiter, const char *newline,
            const char *header, const char *footer, const char *comments);

#ifdef __cplusplus
}
#endif

#endif