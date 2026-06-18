#include "npzfile.h"
#include <string.h>

bool npzfile_contains(NpzFile *npz, const char *key) {
    for (int i = 0; i < npz->n_npy_keys; ++i)
        if (strcmp(npz->npy_keys[i], key) == 0) return true;
    return false;
}
