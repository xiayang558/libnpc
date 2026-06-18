#include "array.h"


// recursive print array helper function (static function, only internal use)
static void print_array_recursive(Array *arr, int current_dim, int *indices, int indent) {
    // print indent, control format aesthetics
    for (int i = 0; i < indent; i++) {
        printf(" ");
    }

    // recursive termination condition: reached the last dimension (leaf node), print concrete element
    if (current_dim == arr->ndim - 1) {
        printf("[");
        for (int i = 0; i < arr->shape[current_dim]; i++) {
            indices[current_dim] = i;
            void *item = get_item(arr, indices);

            // complex use original data based on type print logic
            switch (arr->dtype) {
                case BOOL:
                    printf("%s", (*(char*)item) ? "True" : "False");
                    break;
                case INT8:
                    printf("%d", *(int8_t*)item);
                    break;
                case INT16:
                    printf("%d", *(int16_t*)item);
                    break;
                case INT32:
                    printf("%d", *(int32_t*)item);
                    break;
                case INT64:
                    printf("%lld", *(int64_t*)item);
                    break;
                case UINT8:
                    printf("%u", *(uint8_t*)item);
                    break;
                case UINT16:
                    printf("%u", *(uint16_t*)item);
                    break;
                case UINT32:
                    printf("%u", *(uint32_t*)item);
                    break;
                case UINT64:
                    printf("%llu", *(uint64_t*)item);
                    break;
                case FLOAT32:
                    printf("%.6f", *(float*)item);
                    break;
                case FLOAT64:
                    printf("%.9f", *(double*)item);
                    break;
                case COMPLEX64: {
                    float real = crealf(*(complex float*)item);
                    float imag = cimagf(*(complex float*)item);
                    // based on imaginary part sign determine print + or -
                    if (signbit(imag))
                        printf("%.8f-%.8fi", real, -imag);
                    else
                        printf("%.8f+%.8fi", real, imag);
                    break;
                }
                case COMPLEX128: {
                    double real = creal(*(complex double*)item);
                    double imag = cimag(*(complex double*)item);
                    if (signbit(imag))
                        printf("%.8f-%.8fi", real, -imag);
                    else
                        printf("%.8f+%.8fi", real, imag);
                    break;
                }
                case COMPLEX256: {
                    long double real = creall(*(complex long double*)item);
                    long double imag = cimagl(*(complex long double*)item);
                    if (signbit(imag))
                        printf("%.8Lf-%.8Lfi", real, -imag);
                    else
                        printf("%.8Lf+%.8Lfi", real, imag);
                    break;
                }
                default:
                    printf("?");
                    break;
            }

            // delimiter between elements
            if (i < arr->shape[current_dim] - 1) {
                printf(", ");
            }
        }
        printf("]");
        return;
    }

    // not last dimension: recursive print one level deeper
    printf("[\n");
    for (int i = 0; i < arr->shape[current_dim]; i++) {
        indices[current_dim] = i;
        // recursive call, ndim+1, indent+4 (maintain nested format)
        print_array_recursive(arr, current_dim + 1, indices, indent + 4);
        // comma separator between elements (don't add after last element)
        if (i < arr->shape[current_dim] - 1) {
            printf(",");
        }
        printf("\n");
    }
    // close preceding dimension brackets, keep indent correctly aligned
    for (int i = 0; i < indent; i++) {
        printf(" ");
    }
    printf("]");
}

// print array contents (support any ndim)
void print_array(Array *arr) {
    if (arr == NULL) {
        fprintf(stderr, "NULL array argument\n");
        return;
    }

    // allocate index array, use for recursive traversal per ndim
    int *indices = (int*)malloc(arr->ndim * sizeof(int));
    if (indices == NULL) {
        fprintf(stderr, "Memory allocation failed for indices\n");
        return;
    }
    // initialize index to 0
    memset(indices, 0, arr->ndim * sizeof(int));

    // extra handling for 0-dim scalar: directly print scalar value
    if (arr->ndim == 0 && arr->size == 1) {
        print_scalar(arr);
    } else {
        // call recursive function: starting from 0-th dim, initial indent 2 spaces (consistent with original format)
        print_array_recursive(arr, 0, indices, 2);
        // final newline, finish printing
        printf("\n");
    }

    // free index array memory
    free(indices);
}