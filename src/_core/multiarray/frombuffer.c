#include "array.h"

/**
 * Create array from buffer (like numpy.frombuffer)
 * @param buffer: Pointer to data
 * @param dtype: Data type
 * @param count: Number of elements, -1 means infer from buffer size
 * @param offset: Offset in bytes
 * @return: Created array
 */
Array* frombuffer(const void *buffer, DataType dtype, int count, size_t offset) {
    if (buffer == NULL) {
        fprintf(stderr, "frombuffer: NULL buffer\n");
        return NULL;
    }
    size_t elem_size = dtype_size(dtype);
    if (count <= 0) {
        fprintf(stderr, "frombuffer: count must be positive\n");
        return NULL;
    }
    int shape[1] = {count};
    Array *arr = create_array(shape, 1, dtype);
    if (arr == NULL) return NULL;
    const char *src = (const char*)buffer + offset;
    memcpy(arr->data, src, count * elem_size);
    return arr;
}
