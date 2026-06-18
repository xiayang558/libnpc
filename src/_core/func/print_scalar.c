#include "array.h"


// Printscalar
void print_scalar(Array *arr) {
    if (arr == NULL || arr->size != 1) {
        fprintf(stderr, "Invalid scalar array\n");
        return;
    }

    void *item = arr->data;

    switch (arr->dtype) {
        case BOOL:
            printf("%s", (*(uint8_t*)item) ? "True" : "False");
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
            printf("%.6f", *(double*)item);
            break;
        case COMPLEX64:
            printf("%.6f+%.6fi", crealf(*(complex float*)item), cimagf(*(complex float*)item));
            break;
        case COMPLEX128:
            printf("%.6f+%.6fi", creal(*(complex double*)item), cimag(*(complex double*)item));
            break;
        case COMPLEX256:
            printf("%.6Lf+%.6Lfi", creall(*(complex long double*)item), cimagl(*(complex long double*)item));
            break;
        default:
            printf("?");
            break;
    }
    printf("\n");
}