# libnpc

<div align="center">
    <img src="pic/logo1.png" alt="Logo" style="width:400px; height:auto;">
</div>
<h1 align="center">NumPy C 库</h1>

- [libnpc](#libnpc)
  - [1. 项目简介](#1-项目简介)
  - [2. 编译安装](#2-编译安装)
  - [3. 数据类型](#3-数据类型)
  - [4. 快速入门](#4-快速入门)
  - [5. 模块总览](#5-模块总览)
  - [6. API 参考](#6-api-参考)
  - [7. 架构说明](#7-架构说明)
  - [8. 许可证](#8-许可证)

## 1. 项目简介
基于C独立实现的数值运算基础库（类似NumPy）。提供 300+ 个函数，覆盖 21 个模块——数组创建、数学运算、线性代数、FFT、随机数生成、统计、信号处理等——纯 C 类型直接操作，零 Python 依赖。

***[Click to view English Documentation](README.md)***

***[Browse the API Documentation (HTML)](https://github.com/xiayang558/libnpc/blob/main/docs/index.html)***

## 2. 编译安装

### 系统要求
- C99 或更高版本编译器（GCC / Clang）
- `libm`（数学库，所有系统自带）
- `libzip`（NPZ 文件读写 — macOS 执行 `brew install libzip`，Linux 执行 `apt install libzip-dev`）

### 编译
```shell
git clone https://github.com/xiayang558/libnpc.git
cd libnpc
make             # 编译静态库 → lib/libnpc.a
make shared      # 编译共享库 → lib/libnpc.so / lib/libnpc.dylib
make test        # 编译并运行全部 58 个测试套件
make install     # 安装到 /usr/local（可通过 PREFIX=/custom/path 覆盖）
```

### 链接到你的项目
**静态链接**（独立二进制文件）：
```shell
gcc -Ipath/to/libnpc/include -c myfile.c
gcc myfile.o path/to/libnpc/lib/libnpc.a -lm -lzip -o myapp
```

**共享链接**（需要 .so/.dylib 在运行时可用）：
```shell
gcc -Ipath/to/libnpc/include -c myfile.c
gcc myfile.o -Lpath/to/libnpc/lib -lnpc -lm -lzip \
    -Wl,-rpath,path/to/libnpc/lib -o myapp
```

如果已安装到 `/usr/local`：
```shell
gcc myfile.o -lnpc -lm -lzip -o myapp
```



## 3. 数据类型
| 枚举 | C 类型 | 大小 | NumPy 对应 |
|:----:|:------:|:----:|:----------:|
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

二元运算符的类型提升规则：
- `INT + INT` → `INT64`（取最宽整数）
- `INT + FLOAT` → `FLOAT64`（浮点优先）
- `UINT + INT` → `INT64`（有符号优先）
- `FLOAT32 + FLOAT64` → `FLOAT64`（取最宽浮点）
- `REAL + COMPLEX` → `COMPLEX`（复数优先）
- `BOOL + ANY` → 其他类型（布尔转为0/1）

## 4. 快速入门

```c
#include "array.h"

int main() {
    // 创建 3×3 单位矩阵
    Array *I = eye(3, 3, 0, FLOAT64);
    print_array(I);  // [[1.0, 0.0, 0.0], ...]

    // 逐元素算术运算（支持广播）
    Array *a = arange(1, 10, 1, FLOAT64);   // [1..9]
    int shape[] = {3, 3};
    Array *A = reshape(a, shape, 2);          // 3×3 矩阵
    Array *B = add(A, I);                     // A + I
    Array *C = matmul(A, B);                  // A @ (A + I)

    // SVD 分解
    SVDResult svd_res = svd(A, 1);
    printf("Singular values:\n");
    print_array(svd_res.s);

    // FFT
    Array *spectrum = fft(a);

    // 随机数（xoshiro256** 算法）
    random_seed(42);
    int rshape[] = {1000};
    Array *samples = random_normal(0.0, 1.0, rshape, 1);

    // 多项式拟合
    Array *coeffs = polyfit(a /* x */, a /* y */, 2);

    // 释放内存
    free_array(I); free_array(a); free_array(A); free_array(B); free_array(C);
    free_array(svd_res.U); free_array(svd_res.s); free_array(svd_res.Vt);
    free_array(spectrum); free_array(samples); free_array(coeffs);
    return 0;
}
```

## 5. 模块总览
| 模块 | 关键函数 | 数量 |
|:----:|:--------|:----:|
| 数组创建 | zeros, ones, full, arange, linspace, logspace, geomspace, eye, diag, tri, vander, meshgrid | 30+ |
| 核心数学 | add, subtract, multiply, divide, sqrt, exp, log, sin/cos/tan, power, mod, abs, sign, floor, ceil | 46+ |
| 三角函数 | arcsin/arccos/arctan, 双曲函数, arctan2, deg2rad, radians, angle | 18 |
| 浮点运算 | fabs, copysign, nextafter, spacing, frexp, modf, logaddexp | 12 |
| 统计 | sum, mean, std, var, median, percentile, quantile, min, max, ptp, cumsum, cov, corrcoef, trapz | 26+ |
| NaN 统计 | nanmax, nanmin, nanmean, nanmedian, nanpercentile, nancumsum, nancumprod | 8 |
| 线性代数 | matmul, dot, vdot, inner, outer, cross, kron, tensordot, svd, qr, lu, cholesky, inv, pinv, det, trace, eig | 18 |
| FFT 傅里叶变换 | fft, ifft, fft2, fftn, rfft/irfft 系列, hfft, ihfft, fftfreq, rfftfreq, fftshift, ifftshift | 18 |
| 随机数 | xoshiro256** PRNG, 44 种分布（normal, gamma, beta, binomial, poisson, dirichlet, ...） | 44 |
| 多项式 | poly, roots, polyadd/polysub/polymul/polydiv, polyder, polyint, polyfit, polyval | 10 |
| 窗函数 | hamming, hanning, bartlett, blackman, kaiser | 5 |
| 形状操作 | reshape, transpose, flatten, ravel, squeeze, expand_dims, tile, repeat, stack/hstack/vstack, split, flip, roll, broadcast | 25+ |
| 比较与逻辑 | greater, equal, less, isclose, logical_and/or/xor/not, all, any, where, count_nonzero | 15+ |
| 搜索与集合 | unique, intersect1d, setdiff1d, digitize, searchsorted, argmax/min, argsort, argpartition | 20+ |
| 数组操作 | clip, choose, compress, delete, extract, insert, place, put, putmask, pad, trim_zeros | 15+ |
| 位运算 | bitwise_and/or/xor/not, invert, left_shift, right_shift | 7 |
| 输入输出 | readtxt, genfromtxt, savetxt, savez (NPZ), frombuffer | 5 |
| 日期时间 | datetime_now, from_string, as_string, add/sub_days, 比较, is_busday | 10 |
| 插值 | interp | 1 |
| 信号处理 | convolve, correlate | 2 |
| 工具函数 | astype, copy, nan_to_num, unwrap, real/imag, ix_, bincount, lcm, gcd, i0, piecewise | 15+ |

## 6. API 参考

### 数组创建
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

### 形状操作
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

### 算术与数学
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

### 三角函数
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

### 浮点运算
```c
Array* fabs_array(Array *arr);
Array* copysign_array(Array *x1, Array *x2);
Array* nextafter_array(Array *x1, Array *x2);
Array* spacing_array(Array *arr);
int frexp_array(Array *arr, Array **mant, Array **exp);
int modf_array(Array *arr, Array **frac, Array **integ);
```

### 统计
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

### NaN 统计
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

### 线性代数
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

### FFT（快速傅里叶变换）
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

### 随机数
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
// ... 以及更多（laplace, logistic, lognormal, pareto, weibull, ...）
```

### 多项式
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

### 窗函数
```c
Array* hamming(int m);
Array* hanning(int m);
Array* bartlett(int M);
Array* blackman(int M);
Array* kaiser(int M, double beta);
```

### 插值与信号处理
```c
Array* interp(Array *arr, Array *xp, Array *fp, Array *left, Array *right);
Array* convolve(Array *arr1, Array *arr2);
Array* correlate(Array *a, Array *v, const char *mode);
```

### 比较与逻辑
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

### 搜索与集合
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

### 数组操作
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

### 位运算
```c
Array* bitwise_and(Array *arr1, Array *arr2);
Array* bitwise_or(Array *arr1, Array *arr2);
Array* bitwise_xor(Array *arr1, Array *arr2);
Array* bitwise_not(Array *arr);
Array* invert(Array *arr);
Array* left_shift(Array *x1, Array *x2);
Array* right_shift(Array *x1, Array *x2);
```

### 输入输出
```c
Array* readtxt(const char *fname, const char *delimiter, char comment,
               int skiplines, int max_rows, Array *usecols, int ndmin,
               int unpack, DataType dtype);
Array* genfromtxt(const char *filename, const char *delimiter, int skip_header);
int savetxt(const char *filename, Array *arr, const char *delimiter);
int savez(const char *filename, Array **arrays, const char **names, int num_arrays);
Array* frombuffer(const void *buffer, DataType dtype, int count, size_t offset);
```

### 日期时间
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

### 工具函数
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

## 7. 架构说明

### 设计要点
- **广播**：所有二元逐元素操作支持 NumPy 风格的跨步广播
- **内存**：数组使用连续行优先（C 序）存储，视图与属主数组共享数据
- **结果类型**：数学运算自动提升类型（如 INT32 + FLOAT64 → FLOAT64）
- **错误处理**：函数出错时返回 `NULL` 并向 `stderr` 输出错误消息
- **零 Python 依赖**：纯 C 实现，不包装 CPython，不链接 NumPy

### 目录结构
```
libnpc/
├── include/            # 公共头文件（16 个）
│   ├── array.h         # 核心数组 API（约300个声明）
│   ├── def.h           # 常量（M_PI、INF、NAN 等）
│   ├── random.h        # 随机数模块（52个声明）
│   ├── shape.h         # 形状操作（30个声明）
│   ├── utils.h         # 工具函数（27个声明）
│   └── ...             # window.h, datetime64.h, io.h, linalg.h 等
├── src/
│   ├── _core/          # 核心实现
│   │   ├── math/       # 逐元素数学运算（45+ 函数）
│   │   ├── linalg/     # 线性代数（14 函数）
│   │   ├── func/       # 核心二元运算
│   │   ├── fromnumeric/# arg*、all、any、clip、searchsorted
│   │   ├── shape_base/ # reshape、transpose、stack、split
│   │   ├── poly/       # 多项式运算（10 函数）
│   │   ├── io/         # 输入输出函数
│   │   └── ...         # npzfile、numeric、towdim_base 等
│   ├── fft/            # FFT 模块（18 函数）
│   ├── random/         # 随机数模块（44 函数）
│   ├── math/           # 高级数学（统计、聚合）
│   ├── functional/     # 窗函数、函数式操作、直方图
│   ├── datetime64/     # Datetime64 实现
│   └── ...
└── test/               # 测试文件（58 个测试套件，全部自动化）
```

## 8. 许可证

MIT License

***欢迎提交 PR 和 Issues！一起让 libnpc 变得更好。***
