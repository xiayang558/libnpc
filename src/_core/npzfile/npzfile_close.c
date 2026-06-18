#include "npzfile.h"
#include "array.h"
#include <stdio.h>
#include <stdlib.h>
#include <zip.h>

void npzfile_close(NpzFile *npz) {
    if (!npz) return;
    if (npz->zip) { zip_close(npz->zip); npz->zip = NULL; }
    if (npz->files) { for (int i = 0; i < npz->n_files; ++i) free(npz->files[i]); free(npz->files); }
    if (npz->npy_keys) { for (int i = 0; i < npz->n_npy_keys; ++i) free(npz->npy_keys[i]); free(npz->npy_keys); }
    if (npz->f) free(npz->f);
    if (npz->own_fid && npz->fid) fclose((FILE*)npz->fid);
    free(npz->filename);
    free(npz);
}
