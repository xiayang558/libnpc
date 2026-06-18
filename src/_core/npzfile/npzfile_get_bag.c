#include "npzfile.h"
#include <stdlib.h>

BagObj* npzfile_get_bag(NpzFile *npz) {
    return npz->f;
}
