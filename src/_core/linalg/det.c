#include "array.h"
#include "linalg.h"

// matrix determinant (simplified version, only supports floating-point square matrices)
Array* det(Array *arr) {
    if (arr == NULL || arr->ndim != 2) {
        fprintf(stderr, "Determinant requires 2D array\n");
        return NULL;
    }

    if (arr->shape[0] != arr->shape[1]) {
        fprintf(stderr, "Determinant requires square matrix\n");
        return NULL;
    }

    if (arr->dtype != FLOAT32 && arr->dtype != FLOAT64) {
        fprintf(stderr, "Determinant only supported for float32 and float64\n");
        return NULL;
    }

    int n = arr->shape[0];
    
    // create a copy of the matrix
    Array *copy = copy_array(arr);
    if (copy == NULL) {
        return NULL;
    }

    size_t element_size = dtype_size(arr->dtype);
    char *data = (char*)copy->data;

    int sign = 1;  // determinant sign

    // Gauss elimination to convert to upper triangular matrix
    for (int i = 0; i < n; i++) {
        // find pivot
        int pivot_row = i;
        float max_val = 0.0f;

        for (int j = i; j < n; j++) {
            float val;
            if (arr->dtype == FLOAT32) {
                val = fabsf(*(float*)(data + j * n * element_size + i * element_size));
            } else {
                val = fabs(*(double*)(data + j * n * element_size + i * element_size));
            }

            if (val > max_val) {
                max_val = val;
                pivot_row = j;
            }
        }

        // if pivot is 0, determinant is 0
        if (max_val < 1e-10f) {
            free_array(copy);
            int shape[1] = {1};
            Array *result = create_array(shape, 1, arr->dtype);
            if (result == NULL) {
                return NULL;
            }
            
            if (arr->dtype == FLOAT32) {
                float zero = 0.0f;
                memcpy(result->data, &zero, sizeof(float));
            } else {
                double zero = 0.0;
                memcpy(result->data, &zero, sizeof(double));
            }
            return result;
            // }
        }

        // swap current row and pivot row
        if (pivot_row != i) {
            swap_rows(copy, i, pivot_row);
            sign *= -1;  // row swap changes determinant sign
        }

        // eliminate elements below
        for (int j = i + 1; j < n; j++) {
            if (arr->dtype == FLOAT32) {
                float factor = *(float*)(data + j * n * element_size + i * element_size) /
                              *(float*)(data + i * n * element_size + i * element_size);
                for (int k = i; k < n; k++) {
                    *(float*)(data + j * n * element_size + k * element_size) -=
                        factor * *(float*)(data + i * n * element_size + k * element_size);
                }
            } else {
                double factor = *(double*)(data + j * n * element_size + i * element_size) /
                               *(double*)(data + i * n * element_size + i * element_size);
                for (int k = i; k < n; k++) {
                    *(double*)(data + j * n * element_size + k * element_size) -=
                        factor * *(double*)(data + i * n * element_size + k * element_size);
                }
            }
        }
    }

    // compute product of diagonal elements of upper triangular matrix
    int shape[1] = {1};
    Array *result = create_array(shape, 1, arr->dtype);
    if (result == NULL) {
        free_array(copy);
        return NULL;
    }
    
    if (arr->dtype == FLOAT32) {
        float det_value = sign;
        for (int i = 0; i < n; i++) {
            det_value *= *(float*)(data + i * n * element_size + i * element_size);
        }
        memcpy(result->data, &det_value, sizeof(float));
    } else {
        double det_value = sign;
        for (int i = 0; i < n; i++) {
            det_value *= *(double*)(data + i * n * element_size + i * element_size);
        }
        memcpy(result->data, &det_value, sizeof(double));
    }

    free_array(copy);
    return result;
}