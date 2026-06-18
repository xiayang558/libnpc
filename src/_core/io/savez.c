#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "array.h"
#include "io.h"

/**
 * savez  --  Save multiple arrays to a .npz file (like numpy.savez)
 *
 * Unnamed arrays get automatic key names arr_0, arr_1, ...
 * Named arrays use the user-provided key names
 *
 * @param filename     output file path
 * @param num_unnamed  number of unnamed arrays
 * @param unnamed      unnamed arrays
 * @param num_named    number of named arrays
 * @param named_keys   key names for named arrays
 * @param named_vals   named arrays
 * @return 0 success, -1 failed
 */
int savez(const char *filename, int num_unnamed, Array **unnamed,
          int num_named, const char **named_keys, Array **named_vals) {
    if (filename == NULL) {
        fprintf(stderr, "savez: NULL filename\n");
        return -1;
    }

    int total = num_unnamed + num_named;
    if (total == 0) {
        fprintf(stderr, "savez: no arrays to save\n");
        return -1;
    }

    /* Build combined keys and arrays */
    const char **keys = malloc(total * sizeof(char*));
    Array **arrays = malloc(total * sizeof(Array*));
    char **auto_keys = malloc(num_unnamed * sizeof(char*));
    if (!keys || !arrays || !auto_keys) {
        free(keys); free(arrays); free(auto_keys);
        fprintf(stderr, "savez: memory allocation failed\n");
        return -1;
    }

    for (int i = 0; i < num_unnamed; i++) {
        auto_keys[i] = malloc(32);
        if (auto_keys[i]) snprintf(auto_keys[i], 32, "arr_%d", i);
    }

    for (int i = 0; i < num_unnamed; i++) {
        keys[i] = auto_keys[i];
        arrays[i] = unnamed[i];
    }
    for (int i = 0; i < num_named; i++) {
        keys[num_unnamed + i] = named_keys[i];
        arrays[num_unnamed + i] = named_vals[i];
    }

    int ret = save_npz(filename, total, keys, arrays);

    for (int i = 0; i < num_unnamed; i++) free(auto_keys[i]);
    free(auto_keys); free(keys); free(arrays);
    return ret;
}
