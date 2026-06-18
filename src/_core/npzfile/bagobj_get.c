#include "npzfile.h"
#include "array.h"

Array* npzfile_get(NpzFile *npz, const char *key);
Array* bagobj_get(BagObj *bag, const char *attr) {
    return npzfile_get(bag->parent, attr);
}
