#include "def.h"
#include "array.h"
#include "shape.h"
Array* eye(int N, int M, int k, DataType dtype) {
    if (N <= 0 || M <= 0) { fprintf(stderr, "eye: N and M must be positive\n"); return NULL; }
    Array *result = create_array((int[]){N, M}, 2, dtype);
    if (result == NULL) return NULL;
    size_t elem_sz = dtype_size(dtype);
    memset(result->data, 0, N * M * elem_sz);
    double one = 1.0;
    for (int i = 0; i < N; i++) {
        int j = i + k;
        if (j >= 0 && j < M) {
            void *ptr = (char*)result->data + (i*M + j)*elem_sz;
            switch (dtype) {
                case INT8: *(int8_t*)ptr=1; break; case INT16: *(int16_t*)ptr=1; break;
                case INT32: *(int32_t*)ptr=1; break; case INT64: *(int64_t*)ptr=1; break;
                case UINT8: *(uint8_t*)ptr=1; break; case UINT16: *(uint16_t*)ptr=1; break;
                case UINT32: *(uint32_t*)ptr=1; break; case UINT64: *(uint64_t*)ptr=1; break;
                case FLOAT32: *(float*)ptr=1.0f; break; case FLOAT64: *(double*)ptr=1.0; break;
                case FLOAT128: *(long double*)ptr=1.0L; break;
                case COMPLEX64: *(complex float*)ptr=1.0f+0.0f*I; break;
                case COMPLEX128: *(complex double*)ptr=1.0+0.0*I; break;
                case COMPLEX256: *(complex long double*)ptr=1.0L+0.0L*I; break;
                default: break;
            }
        }
    }
    return result;
}
