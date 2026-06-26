# libnpc

<div align="center">
    <img src="pic/logo1.png" alt="Logo" style="width:400px; height:auto;">
</div>
<h1 align="center">NumPy C Library</h1>

- [libnpc](#libnpc)
  - [1. Introduction](#1-introduction)
  - [2. Build & Install](#2-build--install)
  - [3. Data Types](#3-data-types)
  - [4. Quick Start](#4-quick-start)
  - [5. Module Overview](#5-module-overview)
  - [6. API Reference](#6-api-reference)
  - [7. Architecture](#7-architecture)
  - [8. License](#8-license)

## 1. Introduction
A numerical operation basic library independently implemented based on C (similar to NumPy). Provides 300+ functions across 21 modules — array creation, math operations, linear algebra, FFT, random number generation, statistics, signal processing, and more — all working directly with C types and zero Python dependency.

***[点击查看中文文档](README_zh.md)***

***[Browse the API Documentation (HTML)](https://github.com/xiayang558/libnpc/blob/main/docs/index.html)***

## 2. Build & Install

### Requirements
- C99 or later compiler (GCC / Clang)
- `libm` (math library, standard on all systems)
- `libzip` (for NPZ file I/O — `brew install libzip` on macOS, `apt install libzip-dev` on Linux)

### Build
```shell
git clone https://github.com/xiayang558/libnpc.git
cd libnpc
make             # build static library  → lib/libnpc.a
make shared      # build shared library  → lib/libnpc.so / lib/libnpc.dylib
make test        # build + run all 58 test suites
make install     # install to /usr/local  (override with PREFIX=/custom/path)
```

### Link Into Your Project
**Static link** (self-contained binary):
```shell
gcc -Ipath/to/libnpc/include -c myfile.c
gcc myfile.o path/to/libnpc/lib/libnpc.a -lm -lzip -o myapp
```

**Shared link** (smaller binary, needs .so/.dylib at runtime):
```shell
gcc -Ipath/to/libnpc/include -c myfile.c
gcc myfile.o -Lpath/to/libnpc/lib -lnpc -lm -lzip \
    -Wl,-rpath,path/to/libnpc/lib -o myapp
```

If installed to `/usr/local`:
```shell
gcc myfile.o -lnpc -lm -lzip -o myapp
```



## 3. Data Types
| Enum | C Type | Size | NumPy Equivalent |
|:----:|:------:|:----:|:----------------:|
| `BOOL` | `uint8_t` | 1 B | `bool_` |
| `INT8` | `int8_t` | 1 B | `int8` |
| `INT16` | `int16_t` | 2 B | `int16` |
| `INT32` | `int32_t` | 4 B | `int32` |
| `INT64` | `int64_t` | 8 B | `int64` |
| `UINT8` | `uint8_t` | 1 B | `uint8` |
| `UINT16` | `uint16_t` | 2 B | `uint16` |
| `UINT32` | `uint32_t` | 4 B | `uint32` |
| `UINT64` | `uint64_t` | 8 B | `uint64` |
| `FLOAT32` | `float` | 4 B | `float32` |
| `FLOAT64` | `double` | 8 B | `float64` |
| `FLOAT128` | `long double` | 16 B | `float128` |
| `COMPLEX64` | `complex float` | 8 B | `complex64` |
| `COMPLEX128` | `complex double` | 16 B | `complex128` |
| `COMPLEX256` | `complex long double` | 32 B | `complex256` |
| `DATETIME64` | `int64_t` + unit | 8 B | `datetime64` |
| `STRING` | `char*` | — | `str_` |

Type promotion rules for binary operations:
- `INT + INT` → `INT64` (widest int)
- `INT + FLOAT` → `FLOAT64` (float dominates)
- `UINT + INT` → `INT64` (signed wins)
- `FLOAT32 + FLOAT64` → `FLOAT64` (widest float)
- `REAL + COMPLEX` → `COMPLEX` (complex dominates)
- `BOOL + ANY` → other type (bool → 0/1)

## 4. Quick Start

```c
#include "array.h"

int main() {
    // Create a 3×3 identity matrix
    Array *I = eye(3, 3, 0, FLOAT64);
    print_array(I);  // [[1.0, 0.0, 0.0], ...]

    // Element-wise arithmetic with broadcasting
    Array *a = arange(1, 10, 1, FLOAT64);   // [1..9]
    int shape[] = {3, 3};
    Array *A = reshape(a, shape, 2);          // 3×3 matrix
    Array *B = add(A, I);                     // A + I
    Array *C = matmul(A, B);                  // A @ (A + I)

    // SVD decomposition
    SVDResult svd_res = svd(A, 1);
    printf("Singular values:\n");
    print_array(svd_res.s);

    // FFT
    Array *spectrum = fft(a);

    // Random numbers (xoshiro256**)
    random_seed(42);
    int rshape[] = {1000};
    Array *samples = random_normal(0.0, 1.0, rshape, 1);

    // Polynomial fitting
    Array *coeffs = polyfit(a /* x */, a /* y */, 2);

    // Clean up
    free_array(I); free_array(a); free_array(A); free_array(B); free_array(C);
    free_array(svd_res.U); free_array(svd_res.s); free_array(svd_res.Vt);
    free_array(spectrum); free_array(samples); free_array(coeffs);
    return 0;
}
```

## 5. Module Overview
| Module | Key Functions | Count |
|:------:|:-------------|:-----:|
| Array Creation | zeros, ones, full, arange, linspace, logspace, geomspace, eye, diag, tri, vander, meshgrid | 30+ |
| Core Math | add, subtract, multiply, divide, sqrt, exp, log, sin/cos/tan, power, mod, abs, sign, floor, ceil | 46+ |
| Trigonometry | arcsin/arccos/arctan, hyperbolic functions, arctan2, deg2rad, radians, angle | 18 |
| Floating-Point | fabs, copysign, nextafter, spacing, frexp, modf, logaddexp | 12 |
| Statistics | sum, mean, std, var, median, percentile, quantile, min, max, ptp, cumsum, cov, corrcoef, trapz | 26+ |
| NaN Statistics | nanmax, nanmin, nanmean, nanmedian, nanpercentile, nancumsum, nancumprod | 8 |
| Linear Algebra | matmul, dot, vdot, inner, outer, cross, kron, tensordot, svd, qr, lu, cholesky, inv, pinv, det, trace, eig | 18 |
| FFT | fft, ifft, fft2, fftn, rfft/irfft family, hfft, ihfft, fftfreq, rfftfreq, fftshift, ifftshift | 18 |
| Random | xoshiro256** PRNG, 44 distributions (normal, gamma, beta, binomial, poisson, dirichlet, ...) | 44 |
| Polynomials | poly, roots, polyadd/polysub/polymul/polydiv, polyder, polyint, polyfit, polyval | 10 |
| Window Functions | hamming, hanning, bartlett, blackman, kaiser | 5 |
| Shape & Manipulation | reshape, transpose, flatten, ravel, squeeze, expand_dims, tile, repeat, stack/hstack/vstack, split, flip, roll, broadcast | 25+ |
| Comparison & Logic | greater, equal, less, isclose, logical_and/or/xor/not, all, any, where, count_nonzero | 15+ |
| Search & Set | unique, intersect1d, setdiff1d, digitize, searchsorted, argmax/min, argsort, argpartition | 20+ |
| Array Manipulation | clip, choose, compress, delete, extract, insert, place, put, putmask, pad, trim_zeros | 15+ |
| Bitwise Operations | bitwise_and/or/xor/not, invert, left_shift, right_shift | 7 |
| I/O | readtxt, genfromtxt, savetxt, savez (NPZ), frombuffer | 5 |
| Datetime64 | datetime_now, from_string, as_string, add/sub_days, comparison, is_busday | 10 |
| Interpolation | interp | 1 |
| Signal Processing | convolve, correlate | 2 |
| Utilities | astype, copy, nan_to_num, unwrap, real/imag, ix_, bincount, lcm, gcd, i0, piecewise | 15+ |

## 6. API Reference

### Array Creation
```c
Array* zeros(int *shape, int ndim, DataType dtype);
Array* ones(int *shape, int ndim, DataType dtype);
Array* full(int *shape, int ndim, DataType dtype, void *value);
Array* empty(int *shape, int ndim, DataType dtype);
Array* full_like(Array *arr, void *value, DataType dtype);
Array* arange(double start, double stop, double step, DataType dtype);
Array* linspace_float(float start, float stop, int num);
Array* logspace(double start, double stop, int num, double base, int endpoint);
Array* geomspace(double start, double stop, int num, int endpoint);
Array* eye(int N, int M, int k, DataType dtype);
Array* identity(int n, DataType dtype);
Array* diag(Array *v, int k);
Array* diagflat(Array *arr, int k);
Array* tri(int N, int M, int k, DataType dtype);
Array* vander(Array *x, int N, int increasing);
```

### Shape & Manipulation
```c
Array* reshape(Array *arr, int *new_shape, int new_ndim);
Array* resize(Array *arr, int *new_shape, int new_ndim);
Array* transpose(Array *arr);
Array* flatten(Array *arr);
Array* ravel(Array *arr, const char *order);
Array* squeeze(Array *arr, int axis);
Array* expand_dims(Array *arr, int axis);
Array* tile(Array *arr, int *reps, int nreps);
Array* repeat(Array *arr, Array *repeats, int axis);
Array* roll(Array *arr, int shift, int axis);
Array* rot90(Array *m, int k, int axis1, int axis2);
Array* flip(Array *arr, int axis);
Array* fliplr(Array *arr);
Array* flipud(Array *arr);
Array* broadcast_to(Array *arr, int *shape, int ndim);
Array* stack(Array **arrays, int num_arrays, int axis);
Array* hstack(Array **arrays, int num_arrays);
Array* vstack(Array **arrays, int num_arrays);
Array* dstack(Array **arrays, int num_arrays);
Array* column_stack(Array **arrays, int num_arrays);
Array* concatenate(Array *arr1, Array *arr2, int axis);
```

### Arithmetic & Math
```c
Array* add(Array *arr1, Array *arr2);
Array* subtract(Array *arr1, Array *arr2);
Array* multiply(Array *arr1, Array *arr2);
Array* divide(Array *arr1, Array *arr2);
Array* true_divide(Array *arr1, Array *arr2);
Array* power(Array *arr1, Array *arr2);
Array* mod(Array *arr1, Array *arr2);
Array* mod_scalar(Array *arr, void *scalar);
Array* ahypot(Array *arr1, Array *arr2);
Array* sqrt_array(Array *arr);
Array* exp_array(Array *arr);
Array* log_array(Array *arr);
Array* log2_array(Array *arr);
Array* log10_array(Array *arr);
Array* abs_array(Array *arr);
Array* sign(Array *arr);
Array* sinc(Array *arr);
Array* heaviside(Array *x, double h);
Array* floor_array(Array *arr);
Array* ceil_array(Array *arr);
Array* arint(Array *arr);
Array* atrunc(Array *arr);
Array* logaddexp(Array *arr1, Array *arr2);
Array* logaddexp2(Array *arr1, Array *arr2);
```

### Trigonometry
```c
Array* sin_array(Array *arr);
Array* cos_array(Array *arr);
Array* tan_array(Array *arr);
Array* arcsin(Array *arr);
Array* arccos(Array *arr);
Array* arctan(Array *arr);
Array* arctan2(Array *arr1, Array *arr2);
Array* sinh_array(Array *arr);
Array* cosh_array(Array *arr);
Array* tanh_array(Array *arr);
Array* arcsinh(Array *arr);
Array* arccosh(Array *arr);
Array* arctanh(Array *arr);
Array* deg2rad(Array *arr);
Array* rad2deg(Array *arr);
Array* degrees(Array *arr);
Array* radians(Array *arr);
Array* angle(Array *z, int deg);
```

### Floating-Point Ops
```c
Array* fabs_array(Array *arr);
Array* copysign_array(Array *x1, Array *x2);
Array* nextafter_array(Array *x1, Array *x2);
Array* spacing_array(Array *arr);
int frexp_array(Array *arr, Array **mant, Array **exp);
int modf_array(Array *arr, Array **frac, Array **integ);
```

### Statistics
```c
Array* sum(Array *arr, int axis, int keepdims);
Array* prod(Array *arr, int axis, int keepdims);
Array* mean(Array *arr, int axis, int keepdims);
Array* std(Array *arr, int axis, int keepdims);
Array* var(Array *a, int axis, int ddof, int keepdims);
Array* median(Array *a, int axis, int keepdims);
Array* min(Array *arr, int axis, int keepdims);
Array* max(Array *arr, int axis, int keepdims);
Array* percentile(Array *a, Array *q, int axis);
Array* quantile(Array *a, Array *q, int axis);
Array* ptp(Array *arr, int axis, int keepdims);
Array* cumsum(Array *arr, int axis);
Array* cumprod(Array *arr, int axis);
Array* cov(Array *m, int rowvar, int ddof, DataType dtype);
Array* corrcoef(Array *m, int rowvar);
Array* trapz(Array *y, Array *x, double dx, int axis);
Array* diff(Array *arr, int n, int axis);
Array* average(Array *arr, Array *weights, int axis, int keepdims);
```

### NaN Statistics
```c
Array* nanmax(Array *a, int axis);
Array* nanmin(Array *a, int axis);
Array* nanmean(Array *a, int axis);
Array* nanmedian(Array *a, int axis);
Array* nanpercentile(Array *a, Array *q, int axis);
Array* nanquantile(Array *a, Array *q, int axis);
Array* nancumsum(Array *a, int axis);
Array* nancumprod(Array *a, int axis);
Array* nan_to_num(Array *arr);
```

### Linear Algebra
```c
Array* matmul(Array *arr1, Array *arr2);
Array* dot(Array *arr1, Array *arr2);
Array* vdot(Array *arr1, Array *arr2);
Array* inner(Array *arr1, Array *arr2);
Array* outer(Array *arr1, Array *arr2);
Array* cross(Array *arr1, Array *arr2);
Array* kron(Array *a, Array *b);
Array* tensordot(Array *arr1, Array *arr2, Array *axes_a, Array *axes_b);

SVDResult svd(Array *a, int full_matrices);
QRResult  qr(Array *arr);
LUResult  lu(Array *arr);
Array*    cholesky(Array *arr);
Array*    inv(Array *arr);
Array*    pinv(Array *a, double rcond);
Array*    det(Array *arr);
Array*    trace(Array *arr);
Array*    conj_transpose(Array *arr);
Array*    tril(Array *m, int k);
Array*    triu(Array *m, int k);
```

### FFT (Fast Fourier Transform)
```c
Array* fft(Array *arr);
Array* ifft(Array *arr);
Array* fft2(Array *arr);
Array* ifft2(Array *arr);
Array* fftn(Array *arr);
Array* ifftn(Array *arr);
Array* rfft(Array *arr);
Array* irfft(Array *arr, int n);
Array* rfft2(Array *arr);
Array* irfft2(Array *arr, int s[2]);
Array* rfftn(Array *arr);
Array* irfftn(Array *arr, int *s);
Array* hfft(Array *arr, int n);
Array* ihfft(Array *arr, int n);
Array* fftfreq(int n, double d);
Array* rfftfreq(int n, double d);
Array* fftshift(Array *arr);
Array* ifftshift(Array *arr);
```

### Random Numbers
```c
void random_seed(uint64_t seed);
uint64_t random_uint64(void);
double random_double(void);

Array* random_sample(int *shape, int ndim);
Array* random_rand(int *shape, int ndim);
Array* random_randn(int *shape, int ndim);
Array* random_randint(int low, int high, int *shape, int ndim, DataType dtype);
Array* random_uniform(double low, double high, int *shape, int ndim);
Array* random_normal(double loc, double scale, int *shape, int ndim);
Array* random_exponential(double scale, int *shape, int ndim);
Array* random_beta(double a, double b, int *shape, int ndim);
Array* random_gamma(double shape, double scale, int *shape, int ndim);
Array* random_chisquare(double df, int *shape, int ndim);
Array* random_standard_t(double df, int *shape, int ndim);
Array* random_f(double dfn, double dfd, int *shape, int ndim);
Array* random_binomial(int n, double p, int *shape, int ndim);
Array* random_poisson(double lam, int *shape, int ndim);
Array* random_dirichlet(Array *alpha, int *shape, int ndim);
Array* random_multivariate_normal(Array *mean, Array *cov, int *shape, int ndim);
Array* random_permutation(Array *x);
void   random_shuffle(Array *x);
Array* random_choice(Array *a, int size, int replace, Array *p);
Array* random_multinomial(int n, Array *pvals, int *shape, int ndim);
Array* random_bytes(int n);
// ... and many more (laplace, logistic, lognormal, pareto, weibull, ...)
```

### Polynomials
```c
Array* poly(Array *roots);
Array* roots(Array *p);
Array* polyadd(Array *a, Array *b);
Array* polysub(Array *a, Array *b);
Array* polymul(Array *a, Array *b);
Array* polydiv(Array *u, Array *v, Array **r);
Array* polyder(Array *p, int m);
Array* polyint(Array *p, int m);
Array* polyfit(Array *x, Array *y, int deg);
Array* polyval(Array *p, Array *x);
```

### Window Functions
```c
Array* hamming(int m);
Array* hanning(int m);
Array* bartlett(int M);
Array* blackman(int M);
Array* kaiser(int M, double beta);
```

### Interpolation & Signal Processing
```c
Array* interp(Array *arr, Array *xp, Array *fp, Array *left, Array *right);
Array* convolve(Array *arr1, Array *arr2);
Array* correlate(Array *a, Array *v, const char *mode);
```

### Comparison & Logic
```c
Array* greater(Array *arr1, Array *arr2);
Array* greater_equal(Array *arr1, Array *arr2);
Array* equal(Array *arr1, Array *arr2);
Array* less(Array *x1, Array *x2);
Array* less_equal(Array *x1, Array *x2);
Array* isclose(Array *arr1, Array *arr2, double rtol, double atol, int equal_nan);
Array* logical_and(Array *arr1, Array *arr2);
Array* logical_or(Array *arr1, Array *arr2);
Array* logical_xor(Array *arr1, Array *arr2);
Array* logical_not(Array *arr);
Array* all(Array *arr, int axis, int keepdims);
Array* any(Array *arr, int axis, int keepdims);
Array* where(Array *condition, Array *x, Array *y);
Array* count_nonzero(Array *arr, int axis);
```

### Search & Set
```c
Array* argmax(Array *arr, int axis, int keepdims);
Array* argmin(Array *arr, int axis, int keepdims);
Array* argsort(Array *arr, int axis, const char *kind);
Array* argpartition(Array *arr, int kth, int axis);
Array* argwhere(Array *arr);
UniqueResult unique(Array *arr, int return_index, int return_inverse, int return_counts);
Array* intersect1d(Array *arr1, Array *arr2, int assume_unique);
Array* setdiff1d(Array *arr1, Array *arr2);
Array* searchsorted(Array *a, Array *v, const char *side);
Array* digitize(Array *arr, Array *bins, int right);
```

### Array Manipulation
```c
Array* clip(Array *arr, Array *min, Array *max);
Array* choose(Array *arr, int num_choices, Array **choices, int mode);
Array* compress(Array *condition, Array *a, int axis);
Array* delete(Array *arr, Array *obj, int axis);
Array* extract(Array *condition, Array *arr);
Array* insert(Array *arr, Array *obj, Array *values, int axis);
int place(Array *arr, Array *mask, Array *vals);
int put(Array *arr, Array *indices, Array *values, const char *mode);
void putmask(Array *arr, Array *mask, Array *values);
Array* pad(Array *arr, Array *pad_width, const char *mode, double constant_value);
Array* take(Array *arr, Array *indices, int axis);
Array* append(Array *arr, Array *values, int axis);
Array* trim_zeros(Array *arr, const char *trim);
Array* around(Array *arr, int decimals);
Array* fix(Array *arr);
```

### Bitwise Operations
```c
Array* bitwise_and(Array *arr1, Array *arr2);
Array* bitwise_or(Array *arr1, Array *arr2);
Array* bitwise_xor(Array *arr1, Array *arr2);
Array* bitwise_not(Array *arr);
Array* invert(Array *arr);
Array* left_shift(Array *x1, Array *x2);
Array* right_shift(Array *x1, Array *x2);
```

### I/O
```c
Array* readtxt(const char *fname, const char *delimiter, char comment,
               int skiplines, int max_rows, Array *usecols, int ndmin,
               int unpack, DataType dtype);
Array* genfromtxt(const char *filename, const char *delimiter, int skip_header);
int savetxt(const char *filename, Array *arr, const char *delimiter);
int savez(const char *filename, Array **arrays, const char **names, int num_arrays);
Array* frombuffer(const void *buffer, DataType dtype, int count, size_t offset);
```

### Datetime64
```c
DateTime64 datetime_now(void);
DateTime64 datetime_from_string(const char* str, DateTimeUnit unit);
char* datetime_as_string(const DateTime64* dt);
DateTime64 datetime_add_days(DateTime64 dt, int64_t days);
DateTime64 datetime_sub_days(DateTime64 dt, int64_t days);
bool datetime_eq(DateTime64 a, DateTime64 b);
bool datetime_lt(DateTime64 a, DateTime64 b);
bool datetime_gt(DateTime64 a, DateTime64 b);
int64_t datetime_to_ns(DateTime64 dt);
Array* is_busday(Array *dates, const char *weekmask, Array *holidays);
```

### Utilities
```c
Array* astype(Array *arr, DataType new_dtype);
Array* copy(Array *arr);
Array* nan_to_num(Array *arr);
Array* unwrap(Array *arr, double discont, int axis);
Array* real_array(Array *arr);
Array* imag_array(Array *arr);
int ix_(Array **arrays, int n, Array ***out);
Array* diff(Array *arr, int n, int axis);
Array* ediff1d(Array *y, Array *to_begin, Array *to_end);
Array* meshgrid(Array **arrays, int narrays, const char *indexing);
Array* bincount(Array *a, Array *weights, int minlength);
Array* lcm(Array *x1, Array *x2);
Array* gcd_array(Array *x1, Array *x2);
Array* i0(Array *arr);
double bessel_i0(double x);
```

## 7. Architecture

### Design
- **Broadcasting**: All binary element-wise operations support NumPy-style broadcasting with strides
- **Memory**: Arrays use contiguous row-major (C-order) storage. Views share data with the owning array
- **Result dtype**: Math operations promote types (e.g., INT32 + FLOAT64 → FLOAT64)
- **Error handling**: Functions return `NULL` on error with a message to `stderr`
- **No Python dependency**: Pure C — does not wrap CPython or link against NumPy

### Directory Layout
```
libnpc/
├── include/            # Public headers (16 files)
│   ├── array.h         # Core Array API (~300 declarations)
│   ├── def.h           # Constants (M_PI, INF, NAN, etc.)
│   ├── random.h        # Random module (52 declarations)
│   ├── shape.h         # Shape manipulation (30 declarations)
│   ├── utils.h         # Utilities (27 declarations)
│   └── ...             # window.h, datetime64.h, io.h, linalg.h, etc.
├── src/
│   ├── _core/          # Core implementation
│   │   ├── math/       # Element-wise math (45+ functions)
│   │   ├── linalg/     # Linear algebra (14 functions)
│   │   ├── func/       # Core binary ops
│   │   ├── fromnumeric/# arg*, all, any, clip, searchsorted
│   │   ├── shape_base/ # reshape, transpose, stack, split
│   │   ├── poly/       # Polynomial ops (10 functions)
│   │   ├── io/         # I/O functions
│   │   └── ...         # npzfile, numeric, towdim_base, etc.
│   ├── fft/            # FFT module (18 functions)
│   ├── random/         # Random module (44 functions)
│   ├── math/           # High-level math (statistics, aggregation)
│   ├── functional/     # Window, function_base, histogram
│   ├── datetime64/     # Datetime64 implementation
│   └── ...
└── test/               # Test files (58 test suites, all automated)
```

## 8. License

MIT License

***And PRs and Issues are welcome! Let's make libnpc better together.***
