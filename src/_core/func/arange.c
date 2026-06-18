#include "array.h"
#include "utils.h"

// =========================== Create arithmetic sequence arrays ============================

// Create integer arithmetic sequence array (int32)
Array* arange_int(int start, int stop, int step) {
    if (step == 0) {
        fprintf(stderr, "Step cannot be zero\n");
        return NULL;
    }

    // Compute number of elements
    int size = (stop - start + step - (step > 0 ? 1 : -1)) / step;
    if (size <= 0) {
        fprintf(stderr, "Invalid range\n");
        return NULL;
    }

    int shape[1] = {size};
    Array *arr = create_array(shape, 1, INT32);
    if (arr == NULL) {
        return NULL;
    }

    int *data = (int*)arr->data;
    for (int i = 0; i < size; i++) {
        data[i] = start + i * step;
    }

    return arr;
}

// Create floating point arithmetic sequence array (float32)
Array* arange_float(float start, float stop, float step) {
    if (step == 0.0f) {
        fprintf(stderr, "Step cannot be zero\n");
        return NULL;
    }

    // Compute number of elements
    int size = (int)((stop - start) / step);
    if (size <= 0) {
        fprintf(stderr, "Invalid range\n");
        return NULL;
    }

    int shape[1] = {size};
    Array *arr = create_array(shape, 1, FLOAT32);
    if (arr == NULL) {
        return NULL;
    }

    float *data = (float*)arr->data;
    for (int i = 0; i < size; i++) {
        data[i] = start + i * step;
    }

    return arr;
}


Array* arange(double start, double stop, double step, DataType dtype) {
    if (step == 0.0) {
        fprintf(stderr, "arange: step cannot be zero\n");
        return NULL;
    }

    // Compute number of elements
    double range = stop - start;
    int size = (int)(range / step);
    if (size < 0) size = 0;
    // Handle floating point error causing one extra or fewer elements
    if (fabs(range - size * step) > 1e-12) {
        if ((range > 0 && step > 0) || (range < 0 && step < 0)) {
            size++;
        }
    }

    if (size <= 0) {
        fprintf(stderr, "arange: invalid range (start=%f, stop=%f, step=%f)\n", start, stop, step);
        return NULL;
    }

    // Auto-detect data type
    DataType final_dtype = dtype;
    if (final_dtype == UNKNOWN) {
        if (is_integer_val(start) && is_integer_val(step) && is_integer_val(stop)) {
            final_dtype = INT64;  // Use INT64 to avoid overflow
        } else {
            final_dtype = FLOAT64;
        }
    }

    // Create result array
    int shape[1] = {size};
    Array *arr = create_array(shape, 1, final_dtype);
    if (arr == NULL) {
        return NULL;
    }

    // Fill data
    for (int i = 0; i < size; i++) {
        double val = start + i * step;
        void *ptr = (char*)arr->data + i * dtype_size(final_dtype);

        switch (final_dtype) {
            case INT8: {
                int8_t v = (int8_t)val;
                memcpy(ptr, &v, sizeof(int8_t));
                break;
            }
            case INT16: {
                int16_t v = (int16_t)val;
                memcpy(ptr, &v, sizeof(int16_t));
                break;
            }
            case INT32: {
                int32_t v = (int32_t)val;
                memcpy(ptr, &v, sizeof(int32_t));
                break;
            }
            case INT64: {
                int64_t v = (int64_t)val;
                memcpy(ptr, &v, sizeof(int64_t));
                break;
            }
            case UINT8: {
                uint8_t v = (uint8_t)val;
                memcpy(ptr, &v, sizeof(uint8_t));
                break;
            }
            case UINT16: {
                uint16_t v = (uint16_t)val;
                memcpy(ptr, &v, sizeof(uint16_t));
                break;
            }
            case UINT32: {
                uint32_t v = (uint32_t)val;
                memcpy(ptr, &v, sizeof(uint32_t));
                break;
            }
            case UINT64: {
                uint64_t v = (uint64_t)val;
                memcpy(ptr, &v, sizeof(uint64_t));
                break;
            }
            case FLOAT32: {
                float v = (float)val;
                memcpy(ptr, &v, sizeof(float));
                break;
            }
            case FLOAT64: {
                double v = val;
                memcpy(ptr, &v, sizeof(double));
                break;
            }
            case FLOAT128: {
                long double v = (long double)val;
                memcpy(ptr, &v, sizeof(long double));
                break;
            }
            default:
                fprintf(stderr, "arange: unsupported data type\n");
                free_array(arr);
                return NULL;
        }
    }

    return arr;
}