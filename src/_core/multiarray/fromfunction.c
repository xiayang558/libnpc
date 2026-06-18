#include "array.h"

/**
 * fromfunctiongeneratearray (like numpy.fromfunction)
 * @param func: user-provided function, accepts coordinate array (length=ndim) and ndim, returns double
 * @param shape: shapearray
 * @param ndim: ndim
 * @param dtype: result arraysnumberbased ontype
 * @return: generatearray
 */
Array* fromfunction(double (*func)(int*, int), int *shape, int ndim, DataType dtype) {
    if (func == NULL || shape == NULL || ndim <= 0) {
        fprintf(stderr, "fromfunction: invalid arguments\n");
        return NULL;
    }
    Array *arr = create_array(shape, ndim, dtype);
    if (arr == NULL) return NULL;

    size_t elem_sz = dtype_size(dtype);
    char *data = (char*)arr->data;

    int *strides = malloc(ndim * sizeof(int));
    if (!strides) {
        free_array(arr);
        return NULL;
    }
    strides[ndim - 1] = 1;
    for (int i = ndim - 2; i >= 0; i--) {
        strides[i] = strides[i + 1] * shape[i + 1];
    }

    int *coords = calloc(ndim, sizeof(int));
    if (!coords) {
        free(strides);
        free_array(arr);
        return NULL;
    }

    for (int flat = 0; flat < arr->size; flat++) {
        int tmp = flat;
        for (int i = ndim - 1; i >= 0; i--) {
            coords[i] = tmp % shape[i];
            tmp /= shape[i];
        }
        double val = func(coords, ndim);
        void *ptr = data + flat * elem_sz;
        switch (dtype) {
            case INT8:   *(int8_t*)ptr = (int8_t)val; break;
            case INT16:  *(int16_t*)ptr = (int16_t)val; break;
            case INT32:  *(int32_t*)ptr = (int32_t)val; break;
            case INT64:  *(int64_t*)ptr = (int64_t)val; break;
            case UINT8:  *(uint8_t*)ptr = (uint8_t)val; break;
            case UINT16: *(uint16_t*)ptr = (uint16_t)val; break;
            case UINT32: *(uint32_t*)ptr = (uint32_t)val; break;
            case UINT64: *(uint64_t*)ptr = (uint64_t)val; break;
            case FLOAT32: *(float*)ptr = (float)val; break;
            case FLOAT64: *(double*)ptr = val; break;
            default: break;
        }
    }

    free(strides);
    free(coords);
    return arr;
}
