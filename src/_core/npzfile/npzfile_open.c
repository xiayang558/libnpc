#include "npzfile.h"
#include "array.h"
#include <stdio.h>
#include <stdlib.h>
#include <zip.h>

void npzfile_parse_entries(NpzFile *npz);
NpzFile* npzfile_open(const char *path, bool allow_pickle, int64_t max_header_size) {
    int err;
    zip_t *zip = zip_open(path, 0, &err);
    if (!zip) { fprintf(stderr, "npzfile_open: cannot open %s\n", path); return NULL; }
    NpzFile *npz = calloc(1, sizeof(NpzFile));
    if (!npz) { zip_close(zip); return NULL; }
    npz->filename = strdup(path);
    npz->zip = zip;
    npz->allow_pickle = allow_pickle;
    npz->max_header_size = max_header_size;
    npz->own_fid = false;
    npz->fid = (void*)path;
    npzfile_parse_entries(npz);
    npz->f = malloc(sizeof(BagObj));
    npz->f->parent = npz;
    return npz;
}
