#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include "array.h"
#include "utils.h"

static Array* check_finite(Array *arr, int mode) {
    /* mode: 0=isnan, 1=isinf, 2=isfinite */
    if (arr == NULL) return NULL;

    size_t esz = dtype_size(arr->dtype);
    char *src = (char*)arr->data;

    Array *result = create_array(arr->shape, arr->ndim, BOOL);
    if (!result) return NULL;
    uint8_t *rd = (uint8_t*)result->data;

    for (int i = 0; i < arr->size; i++) {
        int nan_flag = 0, inf_flag = 0;
        switch (arr->dtype) {
            case FLOAT32: {
                float v = *(float*)(src + i * esz);
                nan_flag = isnan(v); inf_flag = isinf(v); break;
            }
            case FLOAT64: {
                double v = *(double*)(src + i * esz);
                nan_flag = isnan(v); inf_flag = isinf(v); break;
            }
            case FLOAT128: {
                long double v = *(long double*)(src + i * esz);
                nan_flag = isnan((double)v); inf_flag = isinf((double)v); break;
            }
            case COMPLEX64: {
                complex float v = *(complex float*)(src + i * esz);
                nan_flag = isnan(crealf(v)) || isnan(cimagf(v));
                inf_flag = isinf(crealf(v)) || isinf(cimagf(v));
                break;
            }
            case COMPLEX128: {
                complex double v = *(complex double*)(src + i * esz);
                nan_flag = isnan(creal(v)) || isnan(cimag(v));
                inf_flag = isinf(creal(v)) || isinf(cimag(v));
                break;
            }
            case COMPLEX256: {
                complex long double v = *(complex long double*)(src + i * esz);
                nan_flag = isnan((double)creall(v)) || isnan((double)cimagl(v));
                inf_flag = isinf((double)creall(v)) || isinf((double)cimagl(v));
                break;
            }
            default:
                /* Integer types: never NaN or Inf */
                rd[i] = (mode == 2) ? 1 : 0;
                continue;
        }
        if (mode == 0) rd[i] = nan_flag ? 1 : 0;
        else if (mode == 1) rd[i] = (inf_flag && !nan_flag) ? 1 : 0;
        else rd[i] = (!nan_flag && !inf_flag) ? 1 : 0;
    }
    return result;
}

Array* isnan_array(Array *arr) { return check_finite(arr, 0); }
Array* isinf_array(Array *arr) { return check_finite(arr, 1); }
Array* isfinite_array(Array *arr) { return check_finite(arr, 2); }
