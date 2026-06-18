#include "npzfile.h"
#include "array.h"
#include <stdio.h>
#include <stdlib.h>
#include <zip.h>

void npzfile_parse_entries(NpzFile *npz);
NpzFile* npzfile_open_fp(FILE *fp, bool own_fid, bool allow_pickle, int64_t max_header_size) {
    int fd = fileno(fp);
    zip_t *zip = zip_fdopen(fd, 0, NULL);
    if (!zip) return NULL;
    NpzFile *npz = calloc(1, sizeof(NpzFile));
    if (!npz) { zip_close(zip); return NULL; }
    npz->filename = strdup("file");
    npz->zip = zip;
    npz->allow_pickle = allow_pickle;
    npz->max_header_size = max_header_size;
    npz->own_fid = own_fid;
    npz->fid = fp;
    npzfile_parse_entries(npz);
    npz->f = malloc(sizeof(BagObj));
    npz->f->parent = npz;
    return npz;
}
