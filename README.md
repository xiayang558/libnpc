<p align="center">
  <img src="pic/logo.png" alt="libnpc" width="160"><br>
  <strong>libnpc: NumPy C Library</strong><br>
  <a href="README_zh.md">中文版</a>
</p>

A numerical operation basic library independently implemented based on C (similar to NumPy).

## Overview

| Category | Module | Functions |
|----------|--------|-----------|
| **Array Creation** | zeros, ones, full, arange, linspace, logspace, geomspace, eye, identity, diag, tri, vander, meshgrid | 30+ |
| **Core Math** | +, −, ×, ÷, sqrt, exp, log, log2, log10, sin/cos/tan, sinh/cosh/tanh, arcsin/arccos/arctan, arctan2, hypot, power, mod, mod_scalar, abs, sign, sinc, heaviside, floor, ceil, rint, trunc, radians, degrees | 46+ |
| **Statistics** | sum, mean, std, var, median, percentile, quantile, min, max, ptp, prod, cumsum, cumprod, cov, corrcoef, correlate, average, trapz | 26+ |
| **NaN-aware Stats** | nanmax, nanmin, nanmean, nanmedian, nanpercentile, nanquantile, nancumsum, nancumprod | 8 |
| **Interpolation** | interp | 1 |
| **Signal Processing** | convolve | 1 |
| **Linear Algebra** | matmul, dot, vdot, inner, outer, cross, kron, tensordot, trace, det, inv, pinv, svd, qr, lu, cholesky, eig, solve | 18 |
| **FFT** | fft, ifft, fft2, ifft2, fftn, ifftn, rfft, irfft, rfft2, irfft2, rfftn, irfftn, hfft, ihfft, fftfreq, rfftfreq, fftshift, ifftshift | 18 |
| **Random** | xoshiro256** PRNG, uniform, normal, randint, beta, gamma, chisquare, binomial, poisson, dirichlet, multivariate_normal, permutation, shuffle, choice + 30 more distributions | 44 |
| **Polynomial** | poly, roots, polyadd, polysub, polymul, polydiv, polyder, polyint, polyfit, polyval | 10 |
| **Window Functions** | hamming, hanning, bartlett, blackman, kaiser | 5 |
| **Shape Ops** | reshape, transpose, flatten, ravel, squeeze, expand_dims, tile, repeat, stack, hstack, vstack, dstack, split, hsplit, vsplit, flip, fliplr, flipud, roll, rot90, take, broadcast_to, atleast_1d/2d/3d | 25+ |
| **Logic & Bitwise** | logical_and/or/xor/not, bitwise_and/or/xor/not, invert, greater, equal, less, isclose, all, any, where, count_nonzero | 15+ |
| **Set & Search** | unique, intersect1d, setdiff1d, digitize, searchsorted, argmax, argmin, argsort, argpartition, argwhere, clip, choose, compress, delete, extract, insert, place, put, putmask, pad | 20+ |
| **I/O** | savez (NPZ), readtxt, genfromtxt, savetxt | 4 |
| **Datetime64** | datetime_create, datetime comparison | 6+ |
| **Utilities** | astype, copy, concatenate, append, frombuffer, conj_transpose, ix\_, fix, around, bincount, lcm, gcd, nan_to_num, unwrap, real_array, imag_array, trim_zeros, fabs, copysign, nextafter, spacing, frexp, modf | 25+ |

**Total: ~320 source files implementing ~300+ NumPy-style functions.**

## Data Types

| Category | Types |
|----------|-------|
| Boolean | `BOOL` |
| Signed Integers | `INT8`, `INT16`, `INT32`, `INT64` |
| Unsigned Integers | `UINT8`, `UINT16`, `UINT32`, `UINT64` |
| Floating Point | `FLOAT32`, `FLOAT64`, `FLOAT128` |
| Complex | `COMPLEX64`, `COMPLEX128`, `COMPLEX256` |
| Special | `DATETIME64`, `STRING` |

## Building

### Requirements
- C99 or later compiler (GCC / Clang)
- `libm` (math library, standard on all systems)
- `libzip` (for NPZ file I/O support — `brew install libzip` on macOS, `apt install libzip-dev` on Linux)

### Build Commands

Use the provided Makefile:

```bash
make             # build static library  → lib/libnpc.a
make shared      # build shared library  → lib/libnpc.so (Linux) / lib/libnpc.dylib (macOS)
make test        # build + run all tests (static link)
make test-shared # build + run all tests (shared link)
make install     # install to /usr/local  (override with PREFIX=/custom/path)
make clean       # remove build artifacts
```

### Linking

**Static link** (self-contained binary, no runtime dependency on the library file):

```bash
gcc -Ipath/to/libnpc/include -c myfile.c
gcc myfile.o path/to/libnpc/lib/libnpc.a -lm -lzip -o myapp
```

**Shared link** (smaller binary, needs `.so`/`.dylib` at runtime):

```bash
gcc -Ipath/to/libnpc/include -c myfile.c
gcc myfile.o -Lpath/to/libnpc/lib -lnpc -lm -lzip \
    -Wl,-rpath,path/to/libnpc/lib -o myapp
```

If installed to `/usr/local`:

```bash
gcc myfile.o -lnpc -lm -lzip -o myapp  # auto-finds libnpc in standard paths
```

## Quick Start

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

## API Reference

### Array Creation

```c
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

Array* zeros(int *shape, int ndim, DataType dtype);
Array* ones(int *shape, int ndim, DataType dtype);
Array* full(int *shape, int ndim, DataType dtype, void *value);
Array* full_like(Array *arr, void *value, DataType dtype);
Array* empty(int *shape, int ndim, DataType dtype);
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

// Trigonometry
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

// Floating-point ops
Array* fabs_array(Array *arr);
Array* copysign_array(Array *x1, Array *x2);
Array* nextafter_array(Array *x1, Array *x2);
Array* spacing_array(Array *arr);
int frexp_array(Array *arr, Array **mant, Array **exp);
int modf_array(Array *arr, Array **frac, Array **integ);
Array* logaddexp(Array *arr1, Array *arr2);
Array* logaddexp2(Array *arr1, Array *arr2);
```

### Statistics & Aggregation

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

// NaN-aware variants
Array* nanmax(Array *a, int axis);
Array* nanmin(Array *a, int axis);
Array* nanmean(Array *a, int axis);
Array* nanmedian(Array *a, int axis);
Array* nanpercentile(Array *a, Array *q, int axis);
Array* nanquantile(Array *a, Array *q, int axis);
Array* nancumsum(Array *a, int axis);
Array* nancumprod(Array *a, int axis);
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

// Decompositions & solvers
SVDResult svd(Array *a, int full_matrices);
QRResult  qr(Array *arr);
LUResult  lu(Array *arr);
Array*    cholesky(Array *arr);
Array*    inv(Array *arr);
Array*    pinv(Array *a, double rcond);
Array*    det(Array *arr);
Array*    trace(Array *arr);
Array*    conj_transpose(Array *arr);

// Triangular matrices
Array* tril(Array *m, int k);
Array* triu(Array *m, int k);
Array* tril_indices(int n, int k, int m);
Array* triu_indices(int n, int k, int m);
Array* tril_indices_from(Array *arr, int k);
Array* triu_indices_from(Array *arr, int k);
```

### FFT (Fast Fourier Transform)

Cooley-Tukey radix-2 DIT FFT with full real-input and Hermitian variants.

```c
Array* fft(Array *arr);              // 1D FFT
Array* ifft(Array *arr);             // 1D inverse FFT
Array* fft2(Array *arr);             // 2D FFT
Array* ifft2(Array *arr);            // 2D inverse FFT
Array* fftn(Array *arr);             // N-D FFT
Array* ifftn(Array *arr);            // N-D inverse FFT

Array* rfft(Array *arr);             // Real 1D FFT (half spectrum)
Array* irfft(Array *arr, int n);     // Inverse real FFT
Array* rfft2(Array *arr);            // Real 2D FFT
Array* irfft2(Array *arr, int s[2]); // Inverse real 2D FFT
Array* rfftn(Array *arr);            // Real N-D FFT
Array* irfftn(Array *arr, int *s);   // Inverse real N-D FFT

Array* hfft(Array *arr, int n);      // Hermitian FFT
Array* ihfft(Array *arr, int n);     // Inverse Hermitian FFT

Array* fftfreq(int n, double d);     // FFT sample frequencies
Array* rfftfreq(int n, double d);    // Real FFT sample frequencies
Array* fftshift(Array *arr);         // Shift zero frequency to center
Array* ifftshift(Array *arr);        // Inverse of fftshift
```

### Random Number Generation

Uses xoshiro256** algorithm (period 2^256−1) with splitmix64 seeding.

```c
void random_seed(uint64_t seed);
uint64_t random_uint64(void);
double random_double(void);      // Uniform [0, 1)

// Generators
Array* random_sample(int *shape, int ndim);        // Uniform [0, 1)
Array* random_rand(int *shape, int ndim);          // Alias for random_sample
Array* random_randn(int *shape, int ndim);         // Standard normal N(0,1)
Array* random_randint(int low, int high, int *shape, int ndim, DataType dtype);
Array* random_uniform(double low, double high, int *shape, int ndim);
Array* random_normal(double loc, double scale, int *shape, int ndim);
Array* random_exponential(double scale, int *shape, int ndim);

// Distributions (44 total)
Array* random_beta(double a, double b, int *shape, int ndim);
Array* random_gamma(double shape, double scale, int *shape, int ndim);
Array* random_chisquare(double df, int *shape, int ndim);
Array* random_standard_t(double df, int *shape, int ndim);
Array* random_f(double dfn, double dfd, int *shape, int ndim);
Array* random_binomial(int n, double p, int *shape, int ndim);
Array* random_poisson(double lam, int *shape, int ndim);
Array* random_dirichlet(Array *alpha, int *shape, int ndim);
Array* random_multivariate_normal(Array *mean, Array *cov, int *shape, int ndim);
// ... and many more (laplace, logistic, lognormal, pareto, weibull, ...)

// Utilities
Array* random_permutation(Array *x);
void   random_shuffle(Array *x);
Array* random_choice(Array *a, int size, int replace, Array *p);
Array* random_multinomial(int n, Array *pvals, int *shape, int ndim);
Array* random_bytes(int n);
```

### Polynomial Operations

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

### Interpolation

```c
Array* interp(Array *arr, Array *xp, Array *fp, Array *left, Array *right);
```

### Signal Processing

```c
Array* convolve(Array *arr1, Array *arr2);  // 1D linear convolution (mode="full")
Array* correlate(Array *a, Array *v, const char *mode);  // 1D cross-correlation
```

### Comparison, Logic & Search

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

Array* bitwise_and(Array *arr1, Array *arr2);
Array* bitwise_or(Array *arr1, Array *arr2);
Array* bitwise_xor(Array *arr1, Array *arr2);
Array* bitwise_not(Array *arr);
Array* invert(Array *arr);

Array* all(Array *arr, int axis, int keepdims);
Array* any(Array *arr, int axis, int keepdims);
Array* where(Array *condition, Array *x, Array *y);
Array* count_nonzero(Array *arr, int axis);

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
```

### I/O

```c
Array* readtxt(const char *fname, const char *delimiter, char comment,
               int skiplines, int max_rows, Array *usecols, int ndmin,
               int unpack, DataType dtype);
Array* genfromtxt(const char *filename, const char *delimiter, int skip_header);
int savetxt(const char *filename, Array *arr, const char *delimiter);
Array* frombuffer(const void *buffer, DataType dtype, int count, size_t offset);

// NPZ file I/O (requires libzip)
int savez(const char *filename, Array **arrays, const char **names, int num_arrays);
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
Array* around(Array *arr, int decimals);
Array* fix(Array *arr);
Array* diff(Array *arr, int n, int axis);
Array* ediff1d(Array *y, Array *to_begin, Array *to_end);
Array* meshgrid(Array **arrays, int narrays, const char *indexing);
Array* trapz(Array *y, Array *x, double dx, int axis);
Array* bincount(Array *a, Array *weights, int minlength);
Array* lcm(Array *x1, Array *x2);
Array* gcd_array(Array *x1, Array *x2);
```

## Data Types Reference

| Enum | C Type | Size (bytes) | NumPy Equivalent |
|------|--------|-------------|------------------|
| `BOOL` | `uint8_t` | 1 | `bool_` |
| `INT8` | `int8_t` | 1 | `int8` |
| `INT16` | `int16_t` | 2 | `int16` |
| `INT32` | `int32_t` | 4 | `int32` |
| `INT64` | `int64_t` | 8 | `int64` |
| `UINT8` | `uint8_t` | 1 | `uint8` |
| `UINT16` | `uint16_t` | 2 | `uint16` |
| `UINT32` | `uint32_t` | 4 | `uint32` |
| `UINT64` | `uint64_t` | 8 | `uint64` |
| `FLOAT32` | `float` | 4 | `float32` |
| `FLOAT64` | `double` | 8 | `float64` |
| `FLOAT128` | `long double` | 16 | `float128` |
| `COMPLEX64` | `complex float` | 8 | `complex64` |
| `COMPLEX128` | `complex double` | 16 | `complex128` |
| `COMPLEX256` | `complex long double` | 32 | `complex256` |
| `DATETIME64` | `int64_t` (+ unit) | 8 | `datetime64` |
| `STRING` | `char*` | — | `str_` |

## Architecture Notes

- **Broadcasting**: All binary element-wise operations support NumPy-style broadcasting with strides
- **Memory**: Arrays use contiguous row-major (C-order) storage. Views share data with the owning array
- **Result dtype**: Math operations promote types (e.g., INT32 + FLOAT64 → FLOAT64)
- **Error handling**: Functions return `NULL` on error with a message to `stderr`
- **No Python dependency**: This is pure C, it does not wrap CPython or link against NumPy

## Directory Layout

```
numpy/
├── include/            # Public headers
│   ├── array.h         # Core Array API (~300 declarations)
│   ├── def.h           # Constants (M_PI, INF, NAN, etc.)
│   ├── random.h        # Random module
│   ├── shape.h         # Shape manipulation functions
│   ├── function.h      # Utility functions (lcm, gcd, extract, etc.)
│   ├── window.h        # Window functions (hamming, hanning, etc.)
│   ├── io.h            # I/O functions
│   ├── linalg.h        # Linear algebra (eig)
│   ├── histogram.h     # Histogram functions
│   ├── datetime64.h    # Datetime64 type support
│   ├── npzfile.h       # NPZ file I/O
│   ├── einsum.h        # Einstein summation
│   ├── np_math_macros.h # Math macro constants
│   └── _dt_utils.h     # Datetime utility macros
├── src/
│   ├── _core/
│   │   ├── math/       # Element-wise math (45+ functions)
│   │   ├── linalg/     # Linear algebra (14 functions)
│   │   ├── func/       # Core binary ops (hypot, logaddexp, mod)
│   │   ├── fromnumeric/ # arg*, all, any, clip, searchsorted (13 functions)
│   │   ├── shape_base/  # reshape, transpose, stack, split (24+ functions)
│   │   ├── function_basic/ # append, invert, bitwise, flip, etc. (14 functions)
│   │   ├── poly/        # Polynomial ops (10 functions)
│   │   ├── io/          # I/O (genfromtxt, readtxt, savetxt, savez)
│   │   ├── numeric/     # Numeric utilities
│   │   ├── npzfile/     # NPZ archive handling
│   │   ├── stride_tricks/ # Advanced stride operations
│   │   ├── towdim_base/ # eyed, identity, diag, tril, triu, etc.
│   │   ├── utils_func/  # Broadcasting, shape helpers
│   │   └── multiarray/  # Multi-array utilities
│   ├── fft/             # FFT module (18 functions)
│   ├── random/          # Random module (44 functions)
│   ├── math/            # High-level math (statistics, aggregation)
│   ├── functional/
│   │   ├── window/      # Window functions (5 functions)
│   │   ├── function_base/ # delete, extract, insert, put, putmask
│   │   └── histogram/   # Histogram functions
│   ├── shape/           # Shape operations
│   ├── _utils/          # Sorting, comparison, utilities
│   ├── matrixlib/       # Matrix utilities (bmat)
│   ├── features/        # Feature extraction
│   └── datetime64/      # Datetime64 implementation
└── test/                # Test files (58 test suites, all automated)
```

## License

MIT License
