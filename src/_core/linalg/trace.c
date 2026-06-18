#include "array.h"


// matrix trace (sum of diagonal elements)
Array* trace(Array *arr) {
    if (arr == NULL || arr->ndim != 2) {
        fprintf(stderr, "Trace requires 2D array\n");
        return NULL;
    }

    if (arr->shape[0] != arr->shape[1]) {
        fprintf(stderr, "Trace requires square matrix\n");
        return NULL;
    }

    int n = arr->shape[0];
    size_t element_size = dtype_size(arr->dtype);
    char *data = (char*)arr->data;

    void *result;

    switch (arr->dtype) {
        case INT8:
        case INT16:
        case INT32:
        case INT64:
        case UINT8:
        case UINT16:
        case UINT32:
        case UINT64: {
            // for integer types, return int64_t
            result = (int64_t*)malloc(sizeof(int64_t));
            int64_t sum = 0;

            for (int i = 0; i < n; i++) {
                switch (arr->dtype) {
                    case INT8:
                        sum += *(int8_t*)(data + i * n * element_size + i * element_size);
                        break;
                    case INT16:
                        sum += *(int16_t*)(data + i * n * element_size + i * element_size);
                        break;
                    case INT32:
                        sum += *(int32_t*)(data + i * n * element_size + i * element_size);
                        break;
                    case INT64:
                        sum += *(int64_t*)(data + i * n * element_size + i * element_size);
                        break;
                    case UINT8:
                        sum += *(uint8_t*)(data + i * n * element_size + i * element_size);
                        break;
                    case UINT16:
                        sum += *(uint16_t*)(data + i * n * element_size + i * element_size);
                        break;
                    case UINT32:
                        sum += *(uint32_t*)(data + i * n * element_size + i * element_size);
                        break;
                    case UINT64:
                        sum += *(uint64_t*)(data + i * n * element_size + i * element_size);
                        break;
                    default:
                        break;
                }
            }

            *(int64_t*)result = sum;
            break;
        }

        case FLOAT32: {
            result = (float*)malloc(sizeof(float));
            float sum = 0.0f;

            for (int i = 0; i < n; i++) {
                sum += *(float*)(data + i * n * element_size + i * element_size);
            }

            *(float*)result = sum;
            break;
        }

        case FLOAT64: {
            result = (double*)malloc(sizeof(double));
            double sum = 0.0;

            for (int i = 0; i < n; i++) {
                sum += *(double*)(data + i * n * element_size + i * element_size);
            }

            *(double*)result = sum;
            break;
        }

        case COMPLEX64: {
            result = (complex float*)malloc(sizeof(complex float));
            complex float sum = 0.0f + 0.0f * I;

            for (int i = 0; i < n; i++) {
                sum += *(complex float*)(data + i * n * element_size + i * element_size);
            }

            *(complex float*)result = sum;
            break;
        }

        case COMPLEX128: {
            result = (complex double*)malloc(sizeof(complex double));
            complex double sum = 0.0 + 0.0 * I;

            for (int i = 0; i < n; i++) {
                sum += *(complex double*)(data + i * n * element_size + i * element_size);
            }

            *(complex double*)result = sum;
            break;
        }

        default:
            fprintf(stderr, "Unsupported data type for trace\n");
            return NULL;
    }

    return result;
}