#include "array.h"


// outer product
Array* outer(Array *arr1, Array *arr2) {
    if (arr1 == NULL || arr2 == NULL) {
        fprintf(stderr, "NULL array argument\n");
        return NULL;
    }

    if (arr1->ndim != 1 || arr2->ndim != 1) {
        fprintf(stderr, "Outer product requires 1D arrays\n");
        return NULL;
    }

    int m = arr1->shape[0];
    int n = arr2->shape[0];

    // outer product result shape is (m, n)
    int shape[2] = {m, n};
    Array *result = create_array(shape, 2, arr1->dtype);
    if (result == NULL) {
        return NULL;
    }

    size_t element_size = dtype_size(arr1->dtype);
    char *data1 = (char*)arr1->data;
    char *data2 = (char*)arr2->data;
    char *res_data = (char*)result->data;

    // handle different data types
    if (arr1->dtype != arr2->dtype) {
        // if types differ, promote to higher precision type
        DataType result_dtype;
        if ((arr1->dtype == FLOAT32 && arr2->dtype == FLOAT64) ||
            (arr1->dtype == FLOAT64 && arr2->dtype == FLOAT32)) {
            result_dtype = FLOAT64;
        } else if ((arr1->dtype == COMPLEX64 && arr2->dtype == COMPLEX128) ||
                   (arr1->dtype == COMPLEX128 && arr2->dtype == COMPLEX64)) {
            result_dtype = COMPLEX128;
        } else {
            fprintf(stderr, "Incompatible data types for outer product\n");
            free_array(result);
            return NULL;
        }

        // recreate result array
        free_array(result);
        result = create_array(shape, 2, result_dtype);
        if (result == NULL) {
            return NULL;
        }

        res_data = (char*)result->data;
        element_size = dtype_size(result_dtype);

        // execute outer product (type promoted)
        if (result_dtype == FLOAT64) {
            for (int i = 0; i < m; i++) {
                double a;
                if (arr1->dtype == FLOAT32) {
                    a = *(float*)(data1 + i * dtype_size(arr1->dtype));
                } else {
                    a = *(double*)(data1 + i * dtype_size(arr1->dtype));
                }

                for (int j = 0; j < n; j++) {
                    double b;
                    if (arr2->dtype == FLOAT32) {
                        b = *(float*)(data2 + j * dtype_size(arr2->dtype));
                    } else {
                        b = *(double*)(data2 + j * dtype_size(arr2->dtype));
                    }

                    *(double*)(res_data + i * n * element_size + j * element_size) = a * b;
                }
            }
        } else if (result_dtype == COMPLEX128) {
            for (int i = 0; i < m; i++) {
                complex double a;
                if (arr1->dtype == COMPLEX64) {
                    complex float af = *(complex float*)(data1 + i * dtype_size(arr1->dtype));
                    a = crealf(af) + cimagf(af) * I;
                } else {
                    a = *(complex double*)(data1 + i * dtype_size(arr1->dtype));
                }

                for (int j = 0; j < n; j++) {
                    complex double b;
                    if (arr2->dtype == COMPLEX64) {
                        complex float bf = *(complex float*)(data2 + j * dtype_size(arr2->dtype));
                        b = crealf(bf) + cimagf(bf) * I;
                    } else {
                        b = *(complex double*)(data2 + j * dtype_size(arr2->dtype));
                    }

                    *(complex double*)(res_data + i * n * element_size + j * element_size) = a * b;
                }
            }
        }
    } else {
        // same type outer product
        switch (arr1->dtype) {
            case INT8:
                for (int i = 0; i < m; i++) {
                    int8_t a = *(int8_t*)(data1 + i * element_size);
                    for (int j = 0; j < n; j++) {
                        int8_t b = *(int8_t*)(data2 + j * element_size);
                        *(int8_t*)(res_data + i * n * element_size + j * element_size) = a * b;
                    }
                }
                break;
            case INT16:
                for (int i = 0; i < m; i++) {
                    int16_t a = *(int16_t*)(data1 + i * element_size);
                    for (int j = 0; j < n; j++) {
                        int16_t b = *(int16_t*)(data2 + j * element_size);
                        *(int16_t*)(res_data + i * n * element_size + j * element_size) = a * b;
                    }
                }
                break;
            case INT32:
                for (int i = 0; i < m; i++) {
                    int32_t a = *(int32_t*)(data1 + i * element_size);
                    for (int j = 0; j < n; j++) {
                        int32_t b = *(int32_t*)(data2 + j * element_size);
                        *(int32_t*)(res_data + i * n * element_size + j * element_size) = a * b;
                    }
                }
                break;
            case INT64:
                for (int i = 0; i < m; i++) {
                    int64_t a = *(int64_t*)(data1 + i * element_size);
                    for (int j = 0; j < n; j++) {
                        int64_t b = *(int64_t*)(data2 + j * element_size);
                        *(int64_t*)(res_data + i * n * element_size + j * element_size) = a * b;
                    }
                }
                break;
            case UINT8:
                for (int i = 0; i < m; i++) {
                    uint8_t a = *(uint8_t*)(data1 + i * element_size);
                    for (int j = 0; j < n; j++) {
                        uint8_t b = *(uint8_t*)(data2 + j * element_size);
                        *(uint8_t*)(res_data + i * n * element_size + j * element_size) = a * b;
                    }
                }
                break;
            case UINT16:
                for (int i = 0; i < m; i++) {
                    uint16_t a = *(uint16_t*)(data1 + i * element_size);
                    for (int j = 0; j < n; j++) {
                        uint16_t b = *(uint16_t*)(data2 + j * element_size);
                        *(uint16_t*)(res_data + i * n * element_size + j * element_size) = a * b;
                    }
                }
                break;
            case UINT32:
                for (int i = 0; i < m; i++) {
                    uint32_t a = *(uint32_t*)(data1 + i * element_size);
                    for (int j = 0; j < n; j++) {
                        uint32_t b = *(uint32_t*)(data2 + j * element_size);
                        *(uint32_t*)(res_data + i * n * element_size + j * element_size) = a * b;
                    }
                }
                break;
            case UINT64:
                for (int i = 0; i < m; i++) {
                    uint64_t a = *(uint64_t*)(data1 + i * element_size);
                    for (int j = 0; j < n; j++) {
                        uint64_t b = *(uint64_t*)(data2 + j * element_size);
                        *(uint64_t*)(res_data + i * n * element_size + j * element_size) = a * b;
                    }
                }
                break;
            case FLOAT32:
                for (int i = 0; i < m; i++) {
                    float a = *(float*)(data1 + i * element_size);
                    for (int j = 0; j < n; j++) {
                        float b = *(float*)(data2 + j * element_size);
                        *(float*)(res_data + i * n * element_size + j * element_size) = a * b;
                    }
                }
                break;
            case FLOAT64:
                for (int i = 0; i < m; i++) {
                    double a = *(double*)(data1 + i * element_size);
                    for (int j = 0; j < n; j++) {
                        double b = *(double*)(data2 + j * element_size);
                        *(double*)(res_data + i * n * element_size + j * element_size) = a * b;
                    }
                }
                break;
            case COMPLEX64:
                for (int i = 0; i < m; i++) {
                    complex float a = *(complex float*)(data1 + i * element_size);
                    for (int j = 0; j < n; j++) {
                        complex float b = *(complex float*)(data2 + j * element_size);
                        *(complex float*)(res_data + i * n * element_size + j * element_size) = a * b;
                    }
                }
                break;
            case COMPLEX128:
                for (int i = 0; i < m; i++) {
                    complex double a = *(complex double*)(data1 + i * element_size);
                    for (int j = 0; j < n; j++) {
                        complex double b = *(complex double*)(data2 + j * element_size);
                        *(complex double*)(res_data + i * n * element_size + j * element_size) = a * b;
                    }
                }
                break;
            default:
                fprintf(stderr, "Unsupported data type for outer product\n");
                free_array(result);
                return NULL;
        }
    }

    return result;
}
