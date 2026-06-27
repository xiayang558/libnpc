#include "array.h"
#include <stdlib.h>
#include <string.h>
#include <complex.h>


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

    // Determine output dtype and compute the trace
    DataType out_dtype;
    void *scalar_val = NULL;

    switch (arr->dtype) {
        case INT8:
        case INT16:
        case INT32:
        case INT64:
        case UINT8:
        case UINT16:
        case UINT32:
        case UINT64: {
            out_dtype = INT64;
            scalar_val = malloc(sizeof(int64_t));
            if (!scalar_val) return NULL;
            int64_t sum = 0;
            for (int i = 0; i < n; i++) {
                switch (arr->dtype) {
                    case INT8:   sum += *(int8_t*)(data + i * n * element_size + i * element_size); break;
                    case INT16:  sum += *(int16_t*)(data + i * n * element_size + i * element_size); break;
                    case INT32:  sum += *(int32_t*)(data + i * n * element_size + i * element_size); break;
                    case INT64:  sum += *(int64_t*)(data + i * n * element_size + i * element_size); break;
                    case UINT8:  sum += *(uint8_t*)(data + i * n * element_size + i * element_size); break;
                    case UINT16: sum += *(uint16_t*)(data + i * n * element_size + i * element_size); break;
                    case UINT32: sum += *(uint32_t*)(data + i * n * element_size + i * element_size); break;
                    case UINT64: sum += *(uint64_t*)(data + i * n * element_size + i * element_size); break;
                    default: break;
                }
            }
            *(int64_t*)scalar_val = sum;
            break;
        }

        case FLOAT32: {
            out_dtype = FLOAT32;
            scalar_val = malloc(sizeof(float));
            if (!scalar_val) return NULL;
            float sum = 0.0f;
            for (int i = 0; i < n; i++)
                sum += *(float*)(data + i * n * element_size + i * element_size);
            *(float*)scalar_val = sum;
            break;
        }

        case FLOAT64:
        case FLOAT128: {
            out_dtype = FLOAT64;
            scalar_val = malloc(sizeof(double));
            if (!scalar_val) return NULL;
            double sum = 0.0;
            for (int i = 0; i < n; i++)
                sum += *(double*)(data + i * n * element_size + i * element_size);
            *(double*)scalar_val = sum;
            break;
        }

        case COMPLEX64: {
            out_dtype = COMPLEX64;
            scalar_val = malloc(sizeof(complex float));
            if (!scalar_val) return NULL;
            complex float sum = 0.0f + 0.0f * I;
            for (int i = 0; i < n; i++)
                sum += *(complex float*)(data + i * n * element_size + i * element_size);
            *(complex float*)scalar_val = sum;
            break;
        }

        case COMPLEX128:
        case COMPLEX256: {
            out_dtype = COMPLEX128;
            scalar_val = malloc(sizeof(complex double));
            if (!scalar_val) return NULL;
            complex double sum = 0.0 + 0.0 * I;
            for (int i = 0; i < n; i++)
                sum += *(complex double*)(data + i * n * element_size + i * element_size);
            *(complex double*)scalar_val = sum;
            break;
        }

        default:
            fprintf(stderr, "Unsupported data type for trace\n");
            return NULL;
    }

    // Wrap the scalar value in a proper Array struct (0-d scalar)
    Array *result = create_scalar_array(scalar_val, out_dtype);
    free(scalar_val);
    return result;
}
