#include "def.h"
#include "array.h"

static int is_float32_nan(float x) { return isnan(x); }
static int is_float64_nan(double x) { return isnan(x); }
static int is_float128_nan(long double x) { return isnan(x); }
static int is_complex64_nan(complex float z) { return isnan(crealf(z)) || isnan(cimagf(z)); }
static int is_complex128_nan(complex double z) { return isnan(creal(z)) || isnan(cimag(z)); }
static int is_complex256_nan(complex long double z) { return isnan(creall(z)) || isnan(cimagl(z)); }

static int float_close(float a, float b, double rtol, double atol) {
    if (is_float32_nan(a) || is_float32_nan(b)) return 0;
    float diff = fabsf(a - b);
    if (diff <= atol) return 1;
    float max_val = fmaxf(fabsf(a), fabsf(b));
    return diff <= rtol * max_val;
}
static int double_close(double a, double b, double rtol, double atol) {
    if (is_float64_nan(a) || is_float64_nan(b)) return 0;
    double diff = fabs(a - b);
    if (diff <= atol) return 1;
    double max_val = fmax(fabs(a), fabs(b));
    return diff <= rtol * max_val;
}
static int long_double_close(long double a, long double b, double rtol, double atol) {
    if (is_float128_nan(a) || is_float128_nan(b)) return 0;
    long double diff = fabsl(a - b);
    if (diff <= atol) return 1;
    long double max_val = fabsl(a) > fabsl(b) ? fabsl(a) : fabsl(b);
    return diff <= rtol * max_val;
}
static int complex64_close(complex float a, complex float b, double rtol, double atol) {
    int a_nan = is_complex64_nan(a), b_nan = is_complex64_nan(b);
    if (a_nan || b_nan) return 0;
    float real_diff = fabsf(crealf(a) - crealf(b)), imag_diff = fabsf(cimagf(a) - cimagf(b));
    float real_max = fmaxf(fabsf(crealf(a)), fabsf(crealf(b))), imag_max = fmaxf(fabsf(cimagf(a)), fabsf(cimagf(b)));
    return (real_diff <= atol || real_diff <= rtol * real_max) && (imag_diff <= atol || imag_diff <= rtol * imag_max);
}
static int complex128_close(complex double a, complex double b, double rtol, double atol) {
    int a_nan = is_complex128_nan(a), b_nan = is_complex128_nan(b);
    if (a_nan || b_nan) return 0;
    double real_diff = fabs(creal(a) - creal(b)), imag_diff = fabs(cimag(a) - cimag(b));
    double real_max = fmax(fabs(creal(a)), fabs(creal(b))), imag_max = fmax(fabs(cimag(a)), fabs(cimag(b)));
    return (real_diff <= atol || real_diff <= rtol * real_max) && (imag_diff <= atol || imag_diff <= rtol * imag_max);
}
static int complex256_close(complex long double a, complex long double b, double rtol, double atol) {
    int a_nan = is_complex256_nan(a), b_nan = is_complex256_nan(b);
    if (a_nan || b_nan) return 0;
    long double real_diff = fabsl(creall(a) - creall(b)), imag_diff = fabsl(cimagl(a) - cimagl(b));
    long double real_max = fabsl(creall(a)) > fabsl(creall(b)) ? fabsl(creall(a)) : fabsl(creall(b));
    long double imag_max = fabsl(cimagl(a)) > fabsl(cimagl(b)) ? fabsl(cimagl(a)) : fabsl(cimagl(b));
    return (real_diff <= atol || real_diff <= rtol * real_max) && (imag_diff <= atol || imag_diff <= rtol * imag_max);
}

Array* isclose(Array *arr1, Array *arr2, double rtol, double atol, int equal_nan) {
    if (arr1 == NULL || arr2 == NULL) { fprintf(stderr, "[Error] isclose: NULL array argument\n"); return NULL; }
    int out_ndim;
    int *out_shape = NULL;
    if (arr1->ndim == 0 && arr2->ndim == 0) { out_ndim = 0; out_shape = NULL; }
    else if (arr1->ndim == 0) { out_ndim = arr2->ndim; out_shape = malloc(out_ndim*sizeof(int)); memcpy(out_shape, arr2->shape, out_ndim*sizeof(int)); }
    else if (arr2->ndim == 0) { out_ndim = arr1->ndim; out_shape = malloc(out_ndim*sizeof(int)); memcpy(out_shape, arr1->shape, out_ndim*sizeof(int)); }
    else {
        if (arr1->ndim != arr2->ndim) { fprintf(stderr, "[Error] isclose: Dimension mismatch\n"); return NULL; }
        for (int i = 0; i < arr1->ndim; i++) { if (arr1->shape[i] != arr2->shape[i]) { fprintf(stderr, "[Error] isclose: Shape mismatch at dim %d\n", i); return NULL; } }
        out_ndim = arr1->ndim; out_shape = malloc(out_ndim*sizeof(int)); memcpy(out_shape, arr1->shape, out_ndim*sizeof(int));
    }
    Array *result;
    if (out_ndim == 0) { int v=FALSE; result=create_scalar_array(&v,BOOL); if(!result){fprintf(stderr,"[Error] equal: Failed to create result scalar\n");return NULL;} }
    else { result=create_array(out_shape,out_ndim,BOOL); if(out_shape)free(out_shape); if(!result){fprintf(stderr,"[Error] equal: Failed to create result array\n");return NULL;} }
    uint8_t *res_data = (uint8_t*)result->data;
    size_t elem_size_a=dtype_size(arr1->dtype), elem_size_b=dtype_size(arr2->dtype);
    char *data_a=(char*)arr1->data, *data_b=(char*)arr2->data;

    if (arr1->ndim == 0 && arr2->ndim == 0) {
        int close = 0; double va=0,vb=0;
        switch (arr1->dtype) {
            case INT8:va=*(int8_t*)data_a;break; case INT16:va=*(int16_t*)data_a;break; case INT32:va=*(int32_t*)data_a;break; case INT64:va=*(int64_t*)data_a;break;
            case UINT8:va=*(uint8_t*)data_a;break; case UINT16:va=*(uint16_t*)data_a;break; case UINT32:va=*(uint32_t*)data_a;break; case UINT64:va=*(uint64_t*)data_a;break;
            case FLOAT32:va=*(float*)data_a;break; case FLOAT64:va=*(double*)data_a;break; case FLOAT128:va=(double)*(long double*)data_a;break;
            case COMPLEX64:{complex float z=*(complex float*)data_a,zb=*(complex float*)data_b;close=complex64_close(z,zb,rtol,atol);if(equal_nan&&is_complex64_nan(z)&&is_complex64_nan(zb))close=TRUE;res_data[0]=close?TRUE:FALSE;free_array(result);return result;}
            case COMPLEX128:{complex double z=*(complex double*)data_a,zb=*(complex double*)data_b;close=complex128_close(z,zb,rtol,atol);if(equal_nan&&is_complex128_nan(z)&&is_complex128_nan(zb))close=TRUE;res_data[0]=close?TRUE:FALSE;free_array(result);return result;}
            case COMPLEX256:{complex long double z=*(complex long double*)data_a,zb=*(complex long double*)data_b;close=complex256_close(z,zb,rtol,atol);if(equal_nan&&is_complex256_nan(z)&&is_complex256_nan(zb))close=TRUE;res_data[0]=close?TRUE:FALSE;free_array(result);return result;}
            default:break;
        }
        switch(arr2->dtype){case INT8:vb=*(int8_t*)data_b;break; case INT16:vb=*(int16_t*)data_b;break; case INT32:vb=*(int32_t*)data_b;break; case INT64:vb=*(int64_t*)data_b;break; case UINT8:vb=*(uint8_t*)data_b;break; case UINT16:vb=*(uint16_t*)data_b;break; case UINT32:vb=*(uint32_t*)data_b;break; case UINT64:vb=*(uint64_t*)data_b;break; case FLOAT32:vb=*(float*)data_b;break; case FLOAT64:vb=*(double*)data_b;break; case FLOAT128:vb=(double)*(long double*)data_b;break; default:break;}
        close=double_close(va,vb,rtol,atol);
        if(equal_nan){int8_t an=FALSE,bn=FALSE;
            switch(arr1->dtype){case INT8:case INT16:case INT32:case INT64:case UINT8:case UINT16:case UINT32:case UINT64:an=FALSE;break; case FLOAT32:an=is_float32_nan(*(float*)data_a);break; case FLOAT64:an=is_float64_nan(*(double*)data_a);break; case FLOAT128:an=is_float128_nan(*(long double*)data_a);break; case COMPLEX64:an=is_complex64_nan(*(complex float*)data_a);break; case COMPLEX128:an=is_complex128_nan(*(complex double*)data_a);break; case COMPLEX256:an=is_complex256_nan(*(complex long double*)data_a);break; default:free_array(result);return NULL;}
            switch(arr2->dtype){case INT8:case INT16:case INT32:case INT64:case UINT8:case UINT16:case UINT32:case UINT64:bn=FALSE;break; case FLOAT32:bn=is_float32_nan(*(float*)data_b);break; case FLOAT64:bn=is_float64_nan(*(double*)data_b);break; case FLOAT128:bn=is_float128_nan(*(long double*)data_b);break; case COMPLEX64:bn=is_complex64_nan(*(complex float*)data_b);break; case COMPLEX128:bn=is_complex128_nan(*(complex double*)data_b);break; case COMPLEX256:bn=is_complex256_nan(*(complex long double*)data_b);break; default:free_array(result);return NULL;}
            if(an&&bn)close=TRUE; else if(an||bn)close=FALSE;}
        res_data[0]=close?TRUE:FALSE;
    } else if (arr1->ndim == 0) {
        double sv=0; int sc=0; complex float scf; complex double scd; complex long double scld;
        switch(arr1->dtype){case INT8:sv=*(int8_t*)data_a;break; case INT16:sv=*(int16_t*)data_a;break; case INT32:sv=*(int32_t*)data_a;break; case INT64:sv=*(int64_t*)data_a;break; case UINT8:sv=*(uint8_t*)data_a;break; case UINT16:sv=*(uint16_t*)data_a;break; case UINT32:sv=*(uint32_t*)data_a;break; case UINT64:sv=*(uint64_t*)data_a;break; case FLOAT32:sv=*(float*)data_a;break; case FLOAT64:sv=*(double*)data_a;break; case FLOAT128:sv=(double)*(long double*)data_a;break; case COMPLEX64:scf=*(complex float*)data_a;sc=1;break; case COMPLEX128:scd=*(complex double*)data_a;sc=2;break; case COMPLEX256:scld=*(complex long double*)data_a;sc=3;break; default:break;}
        for(int i=0;i<arr2->size;i++){void*p=data_b+i*elem_size_b;int close=FALSE;
            if(sc){if(arr2->dtype!=arr1->dtype){fprintf(stderr,"[Error] isclose: Complex type mismatch\n");free_array(result);return NULL;}
                if(sc==1){complex float bv=*(complex float*)p;close=complex64_close(scf,bv,rtol,atol);if(equal_nan&&is_complex64_nan(scf)&&is_complex64_nan(bv))close=TRUE;}
                else if(sc==2){complex double bv=*(complex double*)p;close=complex128_close(scd,bv,rtol,atol);if(equal_nan&&is_complex128_nan(scd)&&is_complex128_nan(bv))close=TRUE;}
                else{complex long double bv=*(complex long double*)p;close=complex256_close(scld,bv,rtol,atol);if(equal_nan&&is_complex256_nan(scld)&&is_complex256_nan(bv))close=TRUE;}}
            else{double bv=0;switch(arr2->dtype){case INT8:bv=*(int8_t*)p;break;case INT16:bv=*(int16_t*)p;break;case INT32:bv=*(int32_t*)p;break;case INT64:bv=*(int64_t*)p;break;case UINT8:bv=*(uint8_t*)p;break;case UINT16:bv=*(uint16_t*)p;break;case UINT32:bv=*(uint32_t*)p;break;case UINT64:bv=*(uint64_t*)p;break;case FLOAT32:bv=*(float*)p;break;case FLOAT64:bv=*(double*)p;break;case FLOAT128:bv=(double)*(long double*)p;break;default:break;}
                close=double_close(sv,bv,rtol,atol);}
            res_data[i]=close?TRUE:FALSE;}
    } else if (arr2->ndim == 0) {
        double sv=0; int sc=0; complex float scf; complex double scd; complex long double scld;
        switch(arr2->dtype){case INT8:sv=*(int8_t*)data_b;break; case INT16:sv=*(int16_t*)data_b;break; case INT32:sv=*(int32_t*)data_b;break; case INT64:sv=*(int64_t*)data_b;break; case UINT8:sv=*(uint8_t*)data_b;break; case UINT16:sv=*(uint16_t*)data_b;break; case UINT32:sv=*(uint32_t*)data_b;break; case UINT64:sv=*(uint64_t*)data_b;break; case FLOAT32:sv=*(float*)data_b;break; case FLOAT64:sv=*(double*)data_b;break; case FLOAT128:sv=(double)*(long double*)data_b;break; case COMPLEX64:scf=*(complex float*)data_b;sc=1;break; case COMPLEX128:scd=*(complex double*)data_b;sc=2;break; case COMPLEX256:scld=*(complex long double*)data_b;sc=3;break; default:break;}
        for(int i=0;i<arr1->size;i++){void*p=data_a+i*elem_size_a;int close=FALSE;
            if(sc){if(arr1->dtype!=arr2->dtype){fprintf(stderr,"[Error] isclose: Complex type mismatch\n");free_array(result);return NULL;}
                if(sc==1){complex float av=*(complex float*)p;close=complex64_close(av,scf,rtol,atol);if(equal_nan&&is_complex64_nan(av)&&is_complex64_nan(scf))close=TRUE;}
                else if(sc==2){complex double av=*(complex double*)p;close=complex128_close(av,scd,rtol,atol);if(equal_nan&&is_complex128_nan(av)&&is_complex128_nan(scd))close=TRUE;}
                else{complex long double av=*(complex long double*)p;close=complex256_close(av,scld,rtol,atol);if(equal_nan&&is_complex256_nan(av)&&is_complex256_nan(scld))close=TRUE;}}
            else{double av=0;switch(arr1->dtype){case INT8:av=*(int8_t*)p;break;case INT16:av=*(int16_t*)p;break;case INT32:av=*(int32_t*)p;break;case INT64:av=*(int64_t*)p;break;case UINT8:av=*(uint8_t*)p;break;case UINT16:av=*(uint16_t*)p;break;case UINT32:av=*(uint32_t*)p;break;case UINT64:av=*(uint64_t*)p;break;case FLOAT32:av=*(float*)p;break;case FLOAT64:av=*(double*)p;break;case FLOAT128:av=(double)*(long double*)p;break;default:break;}
                close=double_close(av,sv,rtol,atol);}
            res_data[i]=close?TRUE:FALSE;}
    } else {
        for(int i=0;i<arr1->size;i++){void*pa=data_a+i*elem_size_a,*pb=data_b+i*elem_size_b;int close=0;
            if(arr1->dtype!=arr2->dtype){double va=0,vb=0;
                switch(arr1->dtype){case INT8:va=*(int8_t*)pa;break;case INT16:va=*(int16_t*)pa;break;case INT32:va=*(int32_t*)pa;break;case INT64:va=*(int64_t*)pa;break;case UINT8:va=*(uint8_t*)pa;break;case UINT16:va=*(uint16_t*)pa;break;case UINT32:va=*(uint32_t*)pa;break;case UINT64:va=*(uint64_t*)pa;break;case FLOAT32:va=*(float*)pa;break;case FLOAT64:va=*(double*)pa;break;case FLOAT128:va=(double)*(long double*)pa;break;default:break;}
                switch(arr2->dtype){case INT8:vb=*(int8_t*)pb;break;case INT16:vb=*(int16_t*)pb;break;case INT32:vb=*(int32_t*)pb;break;case INT64:vb=*(int64_t*)pb;break;case UINT8:vb=*(uint8_t*)pb;break;case UINT16:vb=*(uint16_t*)pb;break;case UINT32:vb=*(uint32_t*)pb;break;case UINT64:vb=*(uint64_t*)pb;break;case FLOAT32:vb=*(float*)pb;break;case FLOAT64:vb=*(double*)pb;break;case FLOAT128:vb=(double)*(long double*)pb;break;default:break;}
                close=double_close(va,vb,rtol,atol);}
            else{double va=0,vb=0;switch(arr1->dtype){case INT8:va=*(int8_t*)pa;vb=*(int8_t*)pb;break;case INT16:va=*(int16_t*)pa;vb=*(int16_t*)pb;break;case INT32:va=*(int32_t*)pa;vb=*(int32_t*)pb;break;case INT64:va=*(int64_t*)pa;vb=*(int64_t*)pb;break;case UINT8:va=*(uint8_t*)pa;vb=*(uint8_t*)pb;break;case UINT16:va=*(uint16_t*)pa;vb=*(uint16_t*)pb;break;case UINT32:va=*(uint32_t*)pa;vb=*(uint32_t*)pb;break;case UINT64:va=*(uint64_t*)pa;vb=*(uint64_t*)pb;break;case FLOAT32:va=*(float*)pa;vb=*(float*)pb;break;case FLOAT64:va=*(double*)pa;vb=*(double*)pb;break;case FLOAT128:va=(double)*(long double*)pa;vb=(double)*(long double*)pb;break;default:break;}
                close=double_close(va,vb,rtol,atol);}
            if(equal_nan){int an=FALSE,bn=FALSE;
                switch(arr1->dtype){case INT8:case INT16:case INT32:case INT64:case UINT8:case UINT16:case UINT32:case UINT64:an=FALSE;break;case FLOAT32:an=is_float32_nan(*(float*)pa);break;case FLOAT64:an=is_float64_nan(*(double*)pa);break;case FLOAT128:an=is_float128_nan(*(long double*)pa);break;case COMPLEX64:an=is_complex64_nan(*(complex float*)pa);break;case COMPLEX128:an=is_complex128_nan(*(complex double*)pa);break;case COMPLEX256:an=is_complex256_nan(*(complex long double*)pa);break;default:free_array(result);return NULL;}
                switch(arr2->dtype){case INT8:case INT16:case INT32:case INT64:case UINT8:case UINT16:case UINT32:case UINT64:bn=FALSE;break;case FLOAT32:bn=is_float32_nan(*(float*)pb);break;case FLOAT64:bn=is_float64_nan(*(double*)pb);break;case FLOAT128:bn=is_float128_nan(*(long double*)pb);break;case COMPLEX64:bn=is_complex64_nan(*(complex float*)pb);break;case COMPLEX128:bn=is_complex128_nan(*(complex double*)pb);break;case COMPLEX256:bn=is_complex256_nan(*(complex long double*)pb);break;default:free_array(result);return NULL;}
                if(an&&bn)close=TRUE;else if(an||bn)close=FALSE;}
            res_data[i]=close?TRUE:FALSE;}
    }
    return result;
}
