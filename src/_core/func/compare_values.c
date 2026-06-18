#include "array.h"


int compare_values(void *pa, void *pb, DataType dtype) {
    switch (dtype) {
        case INT8: {
            int8_t a = *(int8_t*)pa, b = *(int8_t*)pb;
            return (a > b) - (a < b);
        }
        case INT16: {
            int16_t a = *(int16_t*)pa, b = *(int16_t*)pb;
            return (a > b) - (a < b);
        }
        case INT32: {
            int32_t a = *(int32_t*)pa, b = *(int32_t*)pb;
            return (a > b) - (a < b);
        }
        case INT64: {
            int64_t a = *(int64_t*)pa, b = *(int64_t*)pb;
            return (a > b) - (a < b);
        }
        case UINT8: {
            uint8_t a = *(uint8_t*)pa, b = *(uint8_t*)pb;
            return (a > b) - (a < b);
        }
        case UINT16: {
            uint16_t a = *(uint16_t*)pa, b = *(uint16_t*)pb;
            return (a > b) - (a < b);
        }
        case UINT32: {
            uint32_t a = *(uint32_t*)pa, b = *(uint32_t*)pb;
            return (a > b) - (a < b);
        }
        case UINT64: {
            uint64_t a = *(uint64_t*)pa, b = *(uint64_t*)pb;
            return (a > b) - (a < b);
        }
        case FLOAT32: {
            float a = *(float*)pa, b = *(float*)pb;
            if (isnan(a) && isnan(b)) return 0;
            if (isnan(a)) return -1;   // NaN is less than any number
            if (isnan(b)) return 1;
            return (a > b) - (a < b);
        }
        case FLOAT64: {
            double a = *(double*)pa, b = *(double*)pb;
            if (isnan(a) && isnan(b)) return 0;
            if (isnan(a)) return -1;
            if (isnan(b)) return 1;
            return (a > b) - (a < b);
        }
        case FLOAT128: {
            long double a = *(long double*)pa, b = *(long double*)pb;
            if (isnan(a) && isnan(b)) return 0;
            if (isnan(a)) return -1;
            if (isnan(b)) return 1;
            return (a > b) - (a < b);
        }
        case COMPLEX64: {
            complex float a = *(complex float*)pa, b = *(complex float*)pb;
            float abs_a = cabsf(a), abs_b = cabsf(b);
            return (abs_a > abs_b) - (abs_a < abs_b);
        }
        case COMPLEX128: {
            complex double a = *(complex double*)pa, b = *(complex double*)pb;
            double abs_a = cabs(a), abs_b = cabs(b);
            return (abs_a > abs_b) - (abs_a < abs_b);
        }
        case COMPLEX256: {
            complex long double a = *(complex long double*)pa, b = *(complex long double*)pb;
            long double abs_a = cabsl(a), abs_b = cabsl(b);
            return (abs_a > abs_b) - (abs_a < abs_b);
        }
        default:
            return 0;
    }
}