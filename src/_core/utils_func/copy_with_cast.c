#include <string.h>
#include <complex.h>
#include <stdint.h>
#include "array.h"
void copy_with_cast(Array *src, Array *dst, int src_offset, int dst_offset, int count) {
    size_t src_elem = dtype_size(src->dtype);
    size_t dst_elem = dtype_size(dst->dtype);
    char *src_data = (char*)src->data + src_offset * src_elem;
    char *dst_data = (char*)dst->data + dst_offset * dst_elem;
    
    if (src->dtype == dst->dtype) {
        // same type, direct copy
        memcpy(dst_data, src_data, count * src_elem);
        return;
    }
    
    // different types, convert element by element
    for (int i = 0; i < count; i++) {
        void *s = src_data + i * src_elem;
        void *d = dst_data + i * dst_elem;
        
        // convert based on source type and destination type
        switch (src->dtype) {
            case INT8: {
                int8_t val = *(int8_t*)s;
                switch (dst->dtype) {
                    case INT16:   *(int16_t*)d = (int16_t)val; break;
                    case INT32:   *(int32_t*)d = (int32_t)val; break;
                    case INT64:   *(int64_t*)d = (int64_t)val; break;
                    case UINT8:   *(uint8_t*)d = (uint8_t)val; break;
                    case UINT16:  *(uint16_t*)d = (uint16_t)val; break;
                    case UINT32:  *(uint32_t*)d = (uint32_t)val; break;
                    case UINT64:  *(uint64_t*)d = (uint64_t)val; break;
                    case FLOAT32: *(float*)d = (float)val; break;
                    case FLOAT64: *(double*)d = (double)val; break;
                    case COMPLEX64: {
                        complex float *c = (complex float*)d;
                        *c = val + 0.0f*I;
                        break;
                    }
                    case COMPLEX128: {
                        complex double *c = (complex double*)d;
                        *c = val + 0.0*I;
                        break;
                    }
                    default: break;
                }
                break;
            }
            case INT16: {
                int16_t val = *(int16_t*)s;
                switch (dst->dtype) {
                    case INT8:    *(int8_t*)d = (int8_t)val; break;
                    case INT32:   *(int32_t*)d = (int32_t)val; break;
                    case INT64:   *(int64_t*)d = (int64_t)val; break;
                    case UINT8:   *(uint8_t*)d = (uint8_t)val; break;
                    case UINT16:  *(uint16_t*)d = (uint16_t)val; break;
                    case UINT32:  *(uint32_t*)d = (uint32_t)val; break;
                    case UINT64:  *(uint64_t*)d = (uint64_t)val; break;
                    case FLOAT32: *(float*)d = (float)val; break;
                    case FLOAT64: *(double*)d = (double)val; break;
                    case COMPLEX64: {
                        complex float *c = (complex float*)d;
                        *c = val + 0.0f*I;
                        break;
                    }
                    case COMPLEX128: {
                        complex double *c = (complex double*)d;
                        *c = val + 0.0*I;
                        break;
                    }
                    default: break;
                }
                break;
            }
            // other integer types are similar, omitted here for brevity; actual implementation needs to include all types
            // the actual code should contain conversions for INT32, INT64, UINT8, UINT16, UINT32, UINT64, FLOAT32, FLOAT64, COMPLEX64, COMPLEX128
            // below only FLOAT32 and COMPLEX64 are shown as examples
            case FLOAT32: {
                float val = *(float*)s;
                switch (dst->dtype) {
                    case INT8:    *(int8_t*)d = (int8_t)val; break;
                    case INT16:   *(int16_t*)d = (int16_t)val; break;
                    case INT32:   *(int32_t*)d = (int32_t)val; break;
                    case INT64:   *(int64_t*)d = (int64_t)val; break;
                    case UINT8:   *(uint8_t*)d = (uint8_t)val; break;
                    case UINT16:  *(uint16_t*)d = (uint16_t)val; break;
                    case UINT32:  *(uint32_t*)d = (uint32_t)val; break;
                    case UINT64:  *(uint64_t*)d = (uint64_t)val; break;
                    case FLOAT64: *(double*)d = (double)val; break;
                    case COMPLEX64: {
                        complex float *c = (complex float*)d;
                        *c = val + 0.0f*I;
                        break;
                    }
                    case COMPLEX128: {
                        complex double *c = (complex double*)d;
                        *c = val + 0.0*I;
                        break;
                    }
                    default: break;
                }
                break;
            }
            case COMPLEX64: {
                complex float val = *(complex float*)s;
                switch (dst->dtype) {
                    case FLOAT32: *(float*)d = crealf(val); break;
                    case FLOAT64: *(double*)d = (double)crealf(val); break;
                    case COMPLEX128: {
                        complex double *c = (complex double*)d;
                        *c = (complex double)val;
                        break;
                    }
                    default: break;
                }
                break;
            }
            // ... other types
            default:
                break;
        }
    }
}

// fast partition selection function
