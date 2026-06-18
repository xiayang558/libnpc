#include "npzfile.h"

const char** npzfile_keys(NpzFile *npz, int *out_count) {
    *out_count = npz->n_npy_keys;
    return (const char**)npz->npy_keys;
}
