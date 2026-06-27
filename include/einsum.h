#include "def.h"
#include "array.h"

#ifdef __cplusplus
extern "C" {
#endif

int einsum_path(const char *subscripts, Array **operands, int num_operands,
                const char *optimize, int ***out_path, int *out_path_len,
                char **out_info);
Array* einsum(const char *subscripts, Array **operands, int num_operands,
              Array *out, const char *optimize);

#ifdef __cplusplus
}
#endif