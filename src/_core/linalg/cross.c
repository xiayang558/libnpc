#include "array.h"


// cross product (only supports 3D vectors)
Array* cross(Array *arr1, Array *arr2) {
    if (arr1 == NULL || arr2 == NULL) {
        fprintf(stderr, "NULL array argument\n");
        return NULL;
    }

    if (arr1->ndim != 1 || arr2->ndim != 1) {
        fprintf(stderr, "Cross product requires 1D arrays\n");
        return NULL;
    }

    if (arr1->shape[0] != 3 || arr2->shape[0] != 3) {
        fprintf(stderr, "Cross product requires 3D vectors\n");
        return NULL;
    }

    if (arr1->dtype != arr2->dtype) {
        fprintf(stderr, "Array data types do not match\n");
        return NULL;
    }

    int shape[1] = {3};
    Array *result = create_array(shape, 1, arr1->dtype);
    if (result == NULL) {
        return NULL;
    }

    size_t element_size = dtype_size(arr1->dtype);
    char *data1 = (char*)arr1->data;
    char *data2 = (char*)arr2->data;
    char *res_data = (char*)result->data;

    switch (arr1->dtype) {
        case INT8:
        case INT16:
        case INT32:
        case INT64:
        case UINT8:
        case UINT16:
        case UINT32:
        case UINT64: {
            // for integer types, use int64_t for computation
            int64_t a1 = 0, a2 = 0, a3 = 0;
            int64_t b1 = 0, b2 = 0, b3 = 0;

            switch (arr1->dtype) {
                case INT8:
                    a1 = *(int8_t*)(data1 + 0 * element_size);
                    a2 = *(int8_t*)(data1 + 1 * element_size);
                    a3 = *(int8_t*)(data1 + 2 * element_size);
                    b1 = *(int8_t*)(data2 + 0 * element_size);
                    b2 = *(int8_t*)(data2 + 1 * element_size);
                    b3 = *(int8_t*)(data2 + 2 * element_size);
                    break;
                case INT16:
                    a1 = *(int16_t*)(data1 + 0 * element_size);
                    a2 = *(int16_t*)(data1 + 1 * element_size);
                    a3 = *(int16_t*)(data1 + 2 * element_size);
                    b1 = *(int16_t*)(data2 + 0 * element_size);
                    b2 = *(int16_t*)(data2 + 1 * element_size);
                    b3 = *(int16_t*)(data2 + 2 * element_size);
                    break;
                case INT32:
                    a1 = *(int32_t*)(data1 + 0 * element_size);
                    a2 = *(int32_t*)(data1 + 1 * element_size);
                    a3 = *(int32_t*)(data1 + 2 * element_size);
                    b1 = *(int32_t*)(data2 + 0 * element_size);
                    b2 = *(int32_t*)(data2 + 1 * element_size);
                    b3 = *(int32_t*)(data2 + 2 * element_size);
                    break;
                case INT64:
                    a1 = *(int64_t*)(data1 + 0 * element_size);
                    a2 = *(int64_t*)(data1 + 1 * element_size);
                    a3 = *(int64_t*)(data1 + 2 * element_size);
                    b1 = *(int64_t*)(data2 + 0 * element_size);
                    b2 = *(int64_t*)(data2 + 1 * element_size);
                    b3 = *(int64_t*)(data2 + 2 * element_size);
                    break;
                case UINT8:
                    a1 = *(uint8_t*)(data1 + 0 * element_size);
                    a2 = *(uint8_t*)(data1 + 1 * element_size);
                    a3 = *(uint8_t*)(data1 + 2 * element_size);
                    b1 = *(uint8_t*)(data2 + 0 * element_size);
                    b2 = *(uint8_t*)(data2 + 1 * element_size);
                    b3 = *(uint8_t*)(data2 + 2 * element_size);
                    break;
                case UINT16:
                    a1 = *(uint16_t*)(data1 + 0 * element_size);
                    a2 = *(uint16_t*)(data1 + 1 * element_size);
                    a3 = *(uint16_t*)(data1 + 2 * element_size);
                    b1 = *(uint16_t*)(data2 + 0 * element_size);
                    b2 = *(uint16_t*)(data2 + 1 * element_size);
                    b3 = *(uint16_t*)(data2 + 2 * element_size);
                    break;
                case UINT32:
                    a1 = *(uint32_t*)(data1 + 0 * element_size);
                    a2 = *(uint32_t*)(data1 + 1 * element_size);
                    a3 = *(uint32_t*)(data1 + 2 * element_size);
                    b1 = *(uint32_t*)(data2 + 0 * element_size);
                    b2 = *(uint32_t*)(data2 + 1 * element_size);
                    b3 = *(uint32_t*)(data2 + 2 * element_size);
                    break;
                case UINT64:
                    a1 = *(uint64_t*)(data1 + 0 * element_size);
                    a2 = *(uint64_t*)(data1 + 1 * element_size);
                    a3 = *(uint64_t*)(data1 + 2 * element_size);
                    b1 = *(uint64_t*)(data2 + 0 * element_size);
                    b2 = *(uint64_t*)(data2 + 1 * element_size);
                    b3 = *(uint64_t*)(data2 + 2 * element_size);
                    break;
                default:
                    break;
            }

            // compute cross product: a x b = (a2*b3 - a3*b2, a3*b1 - a1*b3, a1*b2 - a2*b1)
            int64_t c1 = a2 * b3 - a3 * b2;
            int64_t c2 = a3 * b1 - a1 * b3;
            int64_t c3 = a1 * b2 - a2 * b1;

            // convert back to original type
            switch (arr1->dtype) {
                case INT8:
                    *(int8_t*)(res_data + 0 * element_size) = (int8_t)c1;
                    *(int8_t*)(res_data + 1 * element_size) = (int8_t)c2;
                    *(int8_t*)(res_data + 2 * element_size) = (int8_t)c3;
                    break;
                case INT16:
                    *(int16_t*)(res_data + 0 * element_size) = (int16_t)c1;
                    *(int16_t*)(res_data + 1 * element_size) = (int16_t)c2;
                    *(int16_t*)(res_data + 2 * element_size) = (int16_t)c3;
                    break;
                case INT32:
                    *(int32_t*)(res_data + 0 * element_size) = (int32_t)c1;
                    *(int32_t*)(res_data + 1 * element_size) = (int32_t)c2;
                    *(int32_t*)(res_data + 2 * element_size) = (int32_t)c3;
                    break;
                case INT64:
                    *(int64_t*)(res_data + 0 * element_size) = c1;
                    *(int64_t*)(res_data + 1 * element_size) = c2;
                    *(int64_t*)(res_data + 2 * element_size) = c3;
                    break;
                case UINT8:
                    *(uint8_t*)(res_data + 0 * element_size) = (uint8_t)c1;
                    *(uint8_t*)(res_data + 1 * element_size) = (uint8_t)c2;
                    *(uint8_t*)(res_data + 2 * element_size) = (uint8_t)c3;
                    break;
                case UINT16:
                    *(uint16_t*)(res_data + 0 * element_size) = (uint16_t)c1;
                    *(uint16_t*)(res_data + 1 * element_size) = (uint16_t)c2;
                    *(uint16_t*)(res_data + 2 * element_size) = (uint16_t)c3;
                    break;
                case UINT32:
                    *(uint32_t*)(res_data + 0 * element_size) = (uint32_t)c1;
                    *(uint32_t*)(res_data + 1 * element_size) = (uint32_t)c2;
                    *(uint32_t*)(res_data + 2 * element_size) = (uint32_t)c3;
                    break;
                case UINT64:
                    *(uint64_t*)(res_data + 0 * element_size) = (uint64_t)c1;
                    *(uint64_t*)(res_data + 1 * element_size) = (uint64_t)c2;
                    *(uint64_t*)(res_data + 2 * element_size) = (uint64_t)c3;
                    break;
                default:
                    break;
            }
            break;
        }

        case FLOAT32: {
            float a1 = *(float*)(data1 + 0 * element_size);
            float a2 = *(float*)(data1 + 1 * element_size);
            float a3 = *(float*)(data1 + 2 * element_size);
            float b1 = *(float*)(data2 + 0 * element_size);
            float b2 = *(float*)(data2 + 1 * element_size);
            float b3 = *(float*)(data2 + 2 * element_size);

            // compute cross product
            *(float*)(res_data + 0 * element_size) = a2 * b3 - a3 * b2;
            *(float*)(res_data + 1 * element_size) = a3 * b1 - a1 * b3;
            *(float*)(res_data + 2 * element_size) = a1 * b2 - a2 * b1;
            break;
        }

        case FLOAT64: {
            double a1 = *(double*)(data1 + 0 * element_size);
            double a2 = *(double*)(data1 + 1 * element_size);
            double a3 = *(double*)(data1 + 2 * element_size);
            double b1 = *(double*)(data2 + 0 * element_size);
            double b2 = *(double*)(data2 + 1 * element_size);
            double b3 = *(double*)(data2 + 2 * element_size);

            // compute cross product
            *(double*)(res_data + 0 * element_size) = a2 * b3 - a3 * b2;
            *(double*)(res_data + 1 * element_size) = a3 * b1 - a1 * b3;
            *(double*)(res_data + 2 * element_size) = a1 * b2 - a2 * b1;
            break;
        }

        case COMPLEX64: {
            complex float a1 = *(complex float*)(data1 + 0 * element_size);
            complex float a2 = *(complex float*)(data1 + 1 * element_size);
            complex float a3 = *(complex float*)(data1 + 2 * element_size);
            complex float b1 = *(complex float*)(data2 + 0 * element_size);
            complex float b2 = *(complex float*)(data2 + 1 * element_size);
            complex float b3 = *(complex float*)(data2 + 2 * element_size);

            // compute cross product
            *(complex float*)(res_data + 0 * element_size) = a2 * b3 - a3 * b2;
            *(complex float*)(res_data + 1 * element_size) = a3 * b1 - a1 * b3;
            *(complex float*)(res_data + 2 * element_size) = a1 * b2 - a2 * b1;
            break;
        }

        case COMPLEX128: {
            complex double a1 = *(complex double*)(data1 + 0 * element_size);
            complex double a2 = *(complex double*)(data1 + 1 * element_size);
            complex double a3 = *(complex double*)(data1 + 2 * element_size);
            complex double b1 = *(complex double*)(data2 + 0 * element_size);
            complex double b2 = *(complex double*)(data2 + 1 * element_size);
            complex double b3 = *(complex double*)(data2 + 2 * element_size);

            // compute cross product
            *(complex double*)(res_data + 0 * element_size) = a2 * b3 - a3 * b2;
            *(complex double*)(res_data + 1 * element_size) = a3 * b1 - a1 * b3;
            *(complex double*)(res_data + 2 * element_size) = a1 * b2 - a2 * b1;
            break;
        }

        default:
            fprintf(stderr, "Unsupported data type for cross product\n");
            free_array(result);
            return NULL;
    }

    return result;
}