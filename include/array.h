#ifndef ARRAY_H
#define ARRAY_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <complex.h>
#include <math.h>
#include <stdint.h>
#include <stdbool.h>
#include "def.h"

#ifndef NP_UTILS_ERROR_LOG
#define NP_UTILS_ERROR_LOG(level, msg, ...) \
    fprintf(stderr, "[%s] %s: " msg "\n", level, __func__, ##__VA_ARGS__)
#endif

// Data type enumeration
typedef enum {
    BOOL,
    INT8,
    INT16,
    INT32,
    INT64,
    UINT8,
    UINT16,
    UINT32,
    UINT64,
    FLOAT32,
    FLOAT64,
    FLOAT128,
    COMPLEX64,
    COMPLEX128,
    COMPLEX256,
    DATETIME64,
    STRING,     // Corresponds to Python S/U string types
    UNKNOWN
} DataType;

// Parallel computation configuration struct
typedef struct {
    int num_threads;     // Number of threads
    int enable_parallel; // Whether to enable parallel computation
} ParallelConfig;

// Array structure
typedef struct {
    void *data;          // Data pointer
    DataType dtype;      // Data type
    int *shape;          // Shape array
    int ndim;            // Number of dimensions
    int size;            // Total number of elements
    int *strides;        // Stride array (for efficient indexing)
    int is_view;         // Whether it is a view (shares data)
} Array;

typedef struct {
    Array *U;
    Array *s;
    Array *Vt;
} SVDResult;

typedef struct {
    Array *Q;     // m×n, orthogonal matrix (Q^T Q = I)
    Array *R;     // n×n, upper triangular matrix
} QRResult;

typedef struct {
    Array *P;     // n×n, permutation matrix
    Array *L;     // n×n, unit lower triangular matrix (ones on diagonal)
    Array *U;     // n×n, upper triangular matrix
} LUResult;

typedef struct {
    Array *values;     // Sorted, deduplicated unique values
    Array *indices;    // First occurrence indices (may be NULL)
    Array *inverse;    // Inverse indices for reconstructing original array (may be NULL)
    Array *counts;     // Count of each unique value (may be NULL)
} UniqueResult;

/** Function pointer type for piecewise */
typedef double (*pf_func)(double);

// choose function modes
#define CHOOSE_RAISE 0
#define CHOOSE_WRAP  1
#define CHOOSE_CLIP  2

/**
 * @brief Get current buffer size (for I/O and internal operations)
 * @return Buffer size in bytes
 */
int getbufsize(void);

/**
 * @brief Set buffer size (for I/O and internal operations)
 * @param size New buffer size (must be > 0)
 */
void setbufsize(int size);


// Utils
void disp(const char* mesg, int add_newline);
const char* get_include(void);
Array* expand_dims(Array *arr, int axis);
int all_shapes_equal(Array **arrays, int n);
Array* to_2d_row(Array *arr);
Array* to_3d_for_dstack(Array *arr);
Array* geomspace(double start, double stop, int num, int endpoint);
Array* insert(Array *arr, Array *obj, Array *values, int axis);                 // Insert values along a given axis (like numpy.insert)

Array* bmat(Array **blocks, int rows, int cols);


// Basic array operations
Array* create_array(int *shape, int ndim, DataType dtype);
Array* empty(int *shape, int ndim, DataType dtype);             // Create an uninitialised array
void free_array(Array *arr);
int compute_offset(Array *arr, int *indices);
int compare_values(void *pa, void *pb, DataType dtype);
void* get_item(Array *arr, int *indices);
void set_item(Array *arr, int *indices, void *value);
void array_info(Array *arr);
void print_array(Array *arr);
void print_scalar(Array *arr);
bool compare_shapes(Array *arr1, Array *arr2);

// Array creation functions
Array* zeros(int *shape, int ndim, DataType dtype);
Array* ones(int *shape, int ndim, DataType dtype);
Array* full(int *shape, int ndim, DataType dtype, void *value);

// Type-specific array creation functions
Array* zeros_int(int *shape, int ndim);
Array* zeros_uint(int *shape, int ndim);
Array* ones_int(int *shape, int ndim);
Array* full_int(int *shape, int ndim, int value);

Array* zeros_float(int *shape, int ndim);
Array* ones_float(int *shape, int ndim);
Array* full_float(int *shape, int ndim, float value);

Array* zeros_complex(int *shape, int ndim);
Array* ones_complex(int *shape, int ndim);
Array* full_complex(int *shape, int ndim, complex float value);

Array* full_like(Array *arr, void *value, DataType dtype);

// Array generation functions
Array* arange_int(int start, int stop, int step);
Array* arange_float(float start, float stop, float step);
Array* arange(double start, double stop, double step, DataType dtype);
Array* linspace_float(float start, float stop, int num);

// Array copy functions
Array* copy_array(Array *arr);
Array* copy(Array *arr);

// Math operation functions
Array* add(Array *arr1, Array *arr2);
Array* subtract(Array *arr1, Array *arr2);
Array* multiply(Array *arr1, Array *arr2);
Array* divide(Array *arr1, Array *arr2);
Array* true_divide(Array *arr1, Array *arr2);   // Forced floating-point division (numpy.true_divide)
Array* add_scalar(Array *arr, void *scalar);
Array* multiply_scalar(Array *arr, void *scalar);
Array* dot(Array *arr1, Array *arr2);
Array* vdot(Array *arr1, Array *arr2);  // Conjugate inner product, flatten then dot (numpy.vdot)
Array* matmul(Array *arr1, Array *arr2);  // Matrix multiplication (supports higher dimensions)
Array* cross(Array *arr1, Array *arr2);   // Cross product
Array* outer(Array *arr1, Array *arr2);   // Outer product
Array* kron(Array *a, Array *b);            // Kronecker product (like numpy.kron)
Array* average(Array *arr, Array *weights, int axis, int keepdims); // Weighted average computation
Array* cov(Array *m, int rowvar, int ddof, DataType dtype);  // Compute covariance matrix (like numpy.cov)
Array* diag(Array *v, int k);  // Diagonal function: k=0 main diagonal, k>0 super-diagonal, k<0 sub-diagonal
Array* eye(int N, int M, int k, DataType dtype);  // Generate identity matrix — if M<=0 or M==N uses diag to build a square, else returns all-zero square
Array* tri(int N, int M, int k, DataType dtype);  // Lower triangular matrix, ones on and below diagonal (numpy.tri)
Array* vander(Array *x, int N, int increasing);    // Vandermonde matrix (numpy.vander)
Array* tril(Array *m, int k);                      // Lower triangle, rest set to zero (numpy.tril)
Array* tril_indices(int n, int k, int m);          // Lower triangular indices (numpy.tril_indices)
Array* tril_indices_from(Array *arr, int k);       // Get lower triangular indices from array (numpy.tril_indices_from)
Array* triu(Array *m, int k);                      // Upper triangle, rest set to zero (numpy.triu)
Array* triu_indices(int n, int k, int m);          // Upper triangular indices (numpy.triu_indices)
Array* triu_indices_from(Array *arr, int k);       // Get upper triangular indices from array (numpy.triu_indices_from)
Array* identity(int n, DataType dtype);                 // Create N×N identity matrix (like numpy.identity)
Array* diff(Array *arr, int n, int axis);  // Difference function
Array* digitize(Array *arr, Array *bins, int right);              // Compute bin index (1-indexed) for each value in arr; bins must be monotonically increasing 1-D array
void fill_diagonal(Array *arr, void *val);
Array* interp(Array *arr, Array *xp, Array *fp, Array *left, Array *right);           // 1-D linear interpolation (like numpy.interp)

Array* tensordot(Array *arr1, Array *arr2, Array *axes_a, Array *axes_b);
Array* inner(Array *arr1, Array *arr2);
Array* tensordot_scalar(Array *arr1, Array *arr2, int axes); 
// Differences between adjacent elements (like numpy.ediff1d)
Array* ediff1d(Array *y, Array *to_begin, Array *to_end);
// Set difference (like numpy.setdiff1d)
Array* setdiff1d(Array *arr1, Array *arr2);

// Element-wise math functions
Array* abs_array(Array *arr);
Array* sqrt_array(Array *arr);
Array* exp_array(Array *arr);
Array* log_array(Array *arr);
Array* log10_array(Array *arr);
Array* log2_array(Array *arr);
Array* power(Array *arr1, Array *arr2);   // Power (exponentiation)

/**
 * @brief Element-wise remainder (like numpy.mod)
 * @param arr1 Dividend array
 * @param arr2 Divisor array
 * @return Result array of type FLOAT64
 */
Array* mod(Array *arr1, Array *arr2);

Array* mod_scalar(Array *arr, void *scalar);  // Scalar modulo operation

/**
 * @brief Compute sqrt(arr1**2 + arr2**2) (like numpy.hypot)
 * @param arr1 First array (or scalar)
 * @param arr2 Second array (or scalar)
 * @return New array of type FLOAT64 with broadcast shape
 */
Array* ahypot(Array *arr1, Array *arr2);

/**
 * @brief Compute log(exp(arr1) + exp(arr2)) (avoids overflow)
 * @param arr1 First array (real)
 * @param arr2 Second array (real)
 * @return Result array of type FLOAT64
 */
Array* logaddexp(Array *arr1, Array *arr2);

/**
 * @brief Compute log2(2**arr1 + 2**arr2) (avoids overflow)
 * @param arr1 First array (real)
 * @param arr2 Second array (real)
 * @return Result array of type FLOAT64
 */
Array* logaddexp2(Array *arr1, Array *arr2);


// Trigonometric and inverse trigonometric functions
Array* sin_array(Array *arr);
Array* cos_array(Array *arr);
Array* tan_array(Array *arr);
Array* sinh_array(Array *arr);
Array* cosh_array(Array *arr);
Array* tanh_array(Array *arr);

Array* arccos(Array *arr);
Array* arccosh(Array *arr);
Array* arcsin(Array *arr);
Array* arcsinh(Array *arr);
Array* arctan(Array *arr);
Array* arctanh(Array *arr);

// Angle and radian conversion
Array* deg2rad(Array *arr);         // Degrees to radians
Array* rad2deg(Array *arr);         // Radians to degrees (same as degrees)
Array* degrees(Array *arr);         // Radians to degrees
Array* radians(Array *arr);         // Degrees to radians
Array* heaviside(Array *x, double h); // Heaviside step function (numpy.heaviside)
Array* floor_array(Array *arr);       // Floor (round down) (numpy.floor)
Array* ceil_array(Array *arr);        // Ceil (round up) (numpy.ceil)
Array* arint(Array *arr);            // Round to nearest integer, ties to even (numpy.rint)
Array* atrunc(Array *arr);           // Truncate to integer (numpy.trunc)
const char* np_typename(const char *typechar);  // Type character → readable name (numpy.typename)
Array* sign(Array *arr);             // Sign function (numpy.sign)
Array* sinc(Array *arr);             // sinc(x) = sin(πx)/(πx)（numpy.sinc）
int asize(Array *arr, int axis);      // Return total elements or axis length (numpy.size)

// Two-argument arctangent
Array* arctan2(Array *arr1, Array *arr2);

// Advanced math functions
Array* fft(Array *arr);                   // Fast Fourier Transform
Array* ifft(Array *arr);                  // Inverse Fast Fourier Transform
Array* rfft(Array *arr);                 // Real FFT, returns only non-negative frequencies (numpy.fft.rfft)
Array* rfft2(Array *arr);                // 2-D real FFT (numpy.fft.rfft2)
Array* rfftn(Array *arr);                // N-D real FFT (numpy.fft.rfftn)
Array* hfft(Array *arr, int n);           // Hermitian FFT, reconstruct real signal from half-spectrum (numpy.fft.hfft)
Array* ihfft(Array *arr, int n);          // Inverse Hermitian FFT, real signal → normalised half-spectrum (numpy.fft.ihfft)
Array* irfft(Array *arr, int n);          // Inverse real FFT, reconstruct real signal from half-spectrum (numpy.fft.irfft)
Array* irfft2(Array *arr, int s[2]);      // 2-D inverse real FFT (numpy.fft.irfft2)
Array* irfftn(Array *arr, int *s);        // N-D inverse real FFT (numpy.fft.irfftn)
Array* fft2(Array *arr);                 // 2-D FFT (numpy.fft.fft2)
Array* ifft2(Array *arr);                // 2-D inverse FFT (numpy.fft.ifft2)
Array* fftn(Array *arr);                 // N-D FFT (numpy.fft.fftn)
Array* ifftn(Array *arr);                // N-D inverse FFT (numpy.fft.ifftn)
Array* fftshift(Array *arr);             // Shift zero frequency to centre (numpy.fft.fftshift)
Array* ifftshift(Array *arr);            // Inverse of fftshift (numpy.fft.ifftshift)
Array* fftfreq(int n, double d);         // FFT sample frequencies (numpy.fft.fftfreq)
Array* rfftfreq(int n, double d);        // Real FFT sample frequencies (numpy.fft.rfftfreq)
Array* convolve(Array *arr1, Array *arr2); // Convolution

// Aggregation functions (unified API, multi-dimensional with axis support)

/**
 * @brief Compute sum of array elements (like numpy.sum)
 * @param arr Input array
 * @param axis Axis to reduce (-1 means all elements)
 * @param keepdims Keep reduced dims as size-1 if 1, else squeeze them
 * @return Result array; shape depends on axis/keepdims; dtype auto-promoted
 */
Array* sum(Array *arr, int axis, int keepdims);

/**
 * @brief Compute the arithmetic mean (like numpy.mean)
 * @param arr Input array
 * @param axis Axis to reduce (-1 means all elements)
 * @param keepdims Keep reduced dims as size-1 if 1, else squeeze them
 * @return Result array; shape depends on axis/keepdims; dtype auto-promoted
 */
Array* mean(Array *arr, int axis, int keepdims);

/**
 * @brief Compute the minimum (like numpy.min)
 * @param arr Input array
 * @param axis Axis to reduce (-1 means all elements)
 * @param keepdims Keep reduced dims as size-1 if 1, else squeeze them
 * @return Result array; shape depends on axis/keepdims; dtype auto-promoted
 */
Array* min(Array *arr, int axis, int keepdims);

/**
 * @brief Compute the maximum (like numpy.max)
 * @param arr Input array
 * @param axis Axis to reduce (-1 means all elements)
 * @param keepdims Keep reduced dims as size-1 if 1, else squeeze them
 * @return Result array; shape depends on axis/keepdims; dtype auto-promoted
 */
Array* max(Array *arr, int axis, int keepdims);

/**
 * @brief Compute the maximum, ignoring NaN (like numpy.nanmax)
 * @param a    Input array
 * @param axis Axis to reduce (-1 means all elements)
 * @return Result array (FLOAT64); NaN if all elements are NaN
 */
Array* nanmax(Array *a, int axis);

/**
 * @brief Compute the minimum, ignoring NaN (like numpy.nanmin)
 * @param a    Input array
 * @param axis Axis to reduce (-1 means all elements)
 * @return Result array (FLOAT64); NaN if all elements are NaN
 */
Array* nanmin(Array *a, int axis);

/**
 * @brief Compute the arithmetic mean, ignoring NaN (like numpy.nanmean)
 * @param a    Input array
 * @param axis Axis to reduce (-1 means all elements)
 * @return Result array (FLOAT64); NaN if all elements are NaN
 */
Array* nanmean(Array *a, int axis);

/**
 * @brief Compute the median, ignoring NaN (like numpy.nanmedian)
 * @param a    Input array
 * @param axis Axis to reduce (-1 means all elements)
 * @return Result array (FLOAT64); NaN if all elements are NaN
 */
Array* nanmedian(Array *a, int axis);

/**
 * @brief Pad an array (like numpy.pad)
 * @param arr            Input array
 * @param pad_width      1-D INT64 array of (ndim × 2) elements; each (before, after) pair for one dimension
 * @param mode           Padding mode: "constant" / "edge" / "reflect" / "symmetric" / "wrap"
 * @param constant_value Value for mode="constant" (ignored for other modes)
 * @return               Padded array, same dtype as input
 */
Array* pad(Array *arr, Array *pad_width, const char *mode, double constant_value);

/**
 * @brief Piecewise function evaluation (like numpy.piecewise)
 * @param x       Input array
 * @param condlist List of BOOL condition arrays
 * @param ncond   Number of conditions
 * @param funclist List of function pointers (one per condition)
 * @return        FLOAT64 result array, same shape as x
 */
Array* piecewise(Array *x, Array **condlist, int ncond, pf_func *funclist);

/**
 * @brief Compute the standard deviation (like numpy.std)
 * @param arr Input array
 * @param axis Axis to reduce (-1 means all elements)
 * @param keepdims Keep reduced dims as size-1 if 1, else squeeze them
 * @return Result array; shape depends on axis/keepdims; dtype auto-promoted
 */
Array* std(Array *arr, int axis, int keepdims);

/**
 * @brief Compute the product (like numpy.prod)
 * @param arr Input array
 * @param axis Axis to reduce (-1 means all elements)
 * @param keepdims Keep reduced dims as size-1 if 1, else squeeze them
 * @return Result array; shape depends on axis/keepdims; dtype auto-promoted
 */
Array* prod(Array *arr, int axis, int keepdims);

/**
 * @brief Compute the cumulative sum (like numpy.cumsum)
 * @param arr Input array
 * @param axis Axis (-1 means flatten first)
 * @return Result array; shape follows axis; dtype auto-promoted
 */
Array* cumsum(Array *arr, int axis);

/**
 * @brief Compute the cumulative product (like numpy.cumprod)
 * @param arr Input array
 * @param axis Axis (-1 means flatten first)
 * @return Result array; shape follows axis; dtype auto-promoted
 */
Array* cumprod(Array *arr, int axis);


/**
 * @brief Compute the median (like numpy.median)
 * @param a       Input array
 * @param axis    Axis (-1 means all elements)
 * @param keepdims Keep reduced dims as size-1 dimensions
 * @return Result array; shape depends on axis and keepdims
 */
Array* median(Array *a, int axis, int keepdims);

/**
 * @brief Compute the variance along a given axis (like numpy.var)
 * @param a       Input array
 * @param axis    Axis (-1 means all elements)
 * @param ddof    Delta degrees of freedom (default 0)
 * @param keepdims Keep reduced dims as size-1 dimensions
 * @return Variance array (FLOAT64)
 */
Array* var(Array *a, int axis, int ddof, int keepdims);

/**
 * @brief Compute the q-th percentile(s) (like numpy.percentile)
 * @param a    Input array
 * @param q    1-D FLOAT64 array, percentile value(s) in [0, 100]
 * @param axis Axis (-1 means flatten all elements)
 * @return FLOAT64 result array
 */
Array* percentile(Array *a, Array *q, int axis);

/**
 * @brief Compute the q-th quantile(s) (like numpy.quantile)
 * @param a    Input array
 * @param q    1-D FLOAT64 array, quantile value(s) in [0, 1]
 * @param axis Axis (-1 means flatten all elements)
 * @return FLOAT64 result array
 */
Array* quantile(Array *a, Array *q, int axis);
Array* nanpercentile(Array *a, Array *q, int axis);
Array* nanquantile(Array *a, Array *q, int axis);
Array* unwrap(Array *arr, double discont, int axis);  // Phase unwrapping (numpy.unwrap)
Array* real_array(Array *arr);     // Real part of complex numbers (numpy.real)
Array* imag_array(Array *arr);     // Imaginary part of complex numbers (numpy.imag)
Array* searchsorted(Array *a, Array *v, const char *side);  // Binary search insertion index (numpy.searchsorted)
Array* fabs_array(Array *arr);
Array* copysign_array(Array *x1, Array *x2);
Array* nextafter_array(Array *x1, Array *x2);
Array* spacing_array(Array *arr);
int frexp_array(Array *arr, Array **mant, Array **exp);
int modf_array(Array *arr, Array **frac, Array **integ);
Array* trim_zeros(Array *arr, const char *trim);
int ix_(Array **arrays, int n, Array ***out);   // Open grid indices (numpy.ix_)

/**
 * @brief Element-wise maximum (like numpy.maximum)
 * @param x1 First array
 * @param x2 Second array
 * @return Result array, same type as inputs
 */
Array* maximum(Array *x1, Array *x2);
Array* minimum(Array *x1, Array *x2);

// Simplified aggregation functions (using default arguments)
inline Array* cumproduct(Array *arr, int axis) { return cumprod(arr, axis); }

// Bitwise operations

Array* bitwise_and(Array *arr1, Array *arr2);       // Bitwise AND
Array* bitwise_not(Array *arr);                   // Bitwise NOT (invert)
Array* bitwise_or(Array *arr1, Array *arr2);        // Bitwise OR
Array* bitwise_xor(Array *arr1, Array *arr2);       // Bitwise XOR

Array* logical_and(Array *arr1, Array *arr2);
Array* logical_or(Array *arr1, Array *arr2);
Array* logical_xor(Array *arr1, Array *arr2);
Array* logical_not(Array *arr);


Array* invert(Array *arr);


// Comparison functions
Array* greater(Array *arr1, Array *arr2);
Array* equal(Array *arr1, Array *arr2);

Array* greater_equal(Array *arr1, Array *arr2);

Array* intersect1d(Array *arr1, Array *arr2, int assume_unique);          // Compute sorted unique intersection of two 1-D arrays
UniqueResult unique(Array *arr, int return_index, int return_inverse, int return_counts);  // Corresponds to numpy.unique

// Array concatenation functions
Array* concatenate(Array *arr1, Array *arr2, int axis);

// Helper functions
size_t dtype_size(DataType dtype);
const char* dtype_name(DataType dtype);
Array* astype(Array *arr, DataType new_dtype);
Array* create_scalar_array(void *value, DataType dtype);
void compute_strides(const int *shape, int ndim, int *strides);
void compute_elem_strides(const int *shape, int ndim, int *strides);
void compute_byte_strides(const int *shape, int ndim, size_t elem_size, size_t *strides);
int* broadcast_shapes(int n, Array **arrays, int *out_ndim);
int* broadcast_shapes_2(Array *arr1, Array *arr2, int *out_ndim);
int get_broadcast_index(const int *res_indices, int res_ndim,
                            Array *arr, const int *arr_strides);
int is_nonzero(void *ptr, DataType dtype);
int is_integer(DataType dtype);
DataType promote_type(DataType t1, DataType t2);
void copy_with_cast(Array *src, Array *dst, int src_offset, int dst_offset, int count);
int64_t find_index(double val, double *bins, int len, int right);       // Binary search helper
int* get_split_points(Array *indices, int axis_len, int *num_points);


Array* diagflat(Array *arr, int k);                                   // Flatten input then place on diagonal of a 2-D array (like numpy.diagflat)
Array** diag_indices(int n, int ndim, int *num_arrays);             // Generate diagonal index tuples (returns array of pointers; length = ndim; caller must free each + the outer array)
Array* diagonal(Array *arr, int offset, int axis1, int axis2);

// Parallel computation functions
void set_parallel_config(int num_threads, int enable_parallel);
ParallelConfig* get_parallel_config();

// Linear algebra functions

/**
 * @brief Compute the singular value decomposition (like numpy.linalg.svd)
 * @param a             Input matrix (2-D)
 * @param full_matrices If 0, economy SVD: U has min(m,n) columns, Vt has min(m,n) rows
 * @return SVDResult struct with U, s, Vt members
 */
SVDResult svd(Array *a, int full_matrices);                    // Singular value decomposition

/**
 * @brief Compute matrix inverse (like numpy.linalg.inv)
 * @param arr Input matrix (must be 2-D square)
 * @return Inverse matrix, or NULL on failure
 */
Array* inv(Array *arr);                    // Matrix inverse

/**
 * @brief Compute the Moore-Penrose pseudo-inverse (like numpy.linalg.pinv)
 * @param a     Input matrix (2-D)
 * @param rcond Singular value cutoff (default 1e-15; if <=0 uses max(shape)*eps*max_s)
 * @return Pseudo-inverse matrix
 */
Array* pinv(Array *a, double rcond);                   // Pseudo-inverse

Array* det(Array *arr);                    // Determinant
Array* trace(Array *arr);                  // Trace
Array* cholesky(Array *arr);               // Cholesky decomposition
/**
 * @brief Compute the QR decomposition (like numpy.linalg.qr)
 * @param arr Input matrix (2-D, m×n)
 * @return QRResult struct with Q (m×n) and R (n×n)
 */
QRResult qr(Array *arr);

/**
 * @brief Compute the LU decomposition with partial pivoting
 * @param arr Input matrix (2-D square)
 * @return LUResult struct with P, L, U, satisfying P * A = L * U
 */
LUResult lu(Array *arr);
// Correlation computation
Array* corrcoef(Array *m, int rowvar);      // Compute Pearson correlation coefficient matrix
Array* correlate(Array *a, Array *v, const char *mode);


int check_elementwise_op(Array *arr1, Array *arr2);
int check_null(const Array *arr, const char* func_name);
int check_axis(const Array *arr, int axis, const char* func_name);
void utils_error_log(const char* level, const char* func, const char* msg, ...);
double get_value_as_double(void *ptr, DataType dtype);
void quickselect(int64_t *indices, int left, int right, int k,
                 void *base, size_t stride, DataType dtype);
Array* take_along_axis(Array *arr, Array *indices, int axis);
int less_than(void *base, size_t stride, DataType dtype,
              int64_t idx_a, int64_t idx_b);
void merge(int64_t *indices, int left, int mid, int right,
           void *base, size_t stride, DataType dtype,
           int64_t *temp);
void merge_sort_indices(int64_t *indices, int left, int right,
                        void *base, size_t stride, DataType dtype,
                        int64_t *temp);

Array* count_nonzero(Array *arr, int axis);



Array* clip(Array *arr, Array *min, Array *max);
Array* choose(Array *arr, int num_choices, Array **choices, int mode);
Array* delete(Array *arr, Array *obj, int axis);
Array* angle(Array *z, int deg);
Array* any(Array *arr, int axis, int keepdims);
Array* all(Array *arr, int axis, int keepdims);
Array* append(Array *arr, Array *values, int axis);
Array* argpartition(Array *arr, int kth, int axis);
Array* partition(Array *arr, int kth, int axis);   // Corresponds to numpy.partition
int place(Array *arr, Array *mask, Array *vals);  // Corresponds to numpy.place
int put(Array *arr, Array *indices, Array *values, const char *mode);  // Corresponds to numpy.put
void putmask(Array *arr, Array *mask, Array *values);                // Corresponds to numpy.putmask
Array* poly(Array *roots);                         // Corresponds to numpy.poly
Array* roots(Array *p);                            // Corresponds to numpy.roots
Array* polyadd(Array *a, Array *b);                 // Corresponds to numpy.polyadd
Array* polysub(Array *a, Array *b);                 // Corresponds to numpy.polysub
Array* polymul(Array *a, Array *b);                 // Corresponds to numpy.polymul
Array* polydiv(Array *u, Array *v, Array **r);      // Corresponds to numpy.polydiv
Array* polyder(Array *p, int m);                    // Corresponds to numpy.polyder
Array* polyfit(Array *x, Array *y, int deg);        // Corresponds to numpy.polyfit
Array* polyint(Array *p, int m);                    // Corresponds to numpy.polyint
Array* polyval(Array *p, Array *x);                 // Corresponds to numpy.polyval
Array* ptp(Array *arr, int axis, int keepdims);      // Corresponds to numpy.ptp
Array* trapz(Array *y, Array *x, double dx, int axis); // Corresponds to numpy.trapz
Array* argsort(Array *arr, int axis, const char *kind);
Array* argwhere(Array *arr);
Array* where(Array *condition, Array *x, Array *y);  // Conditional selection (numpy.where)
Array* argmax(Array *arr, int axis, int keepdims);
Array* argmin(Array *arr, int axis, int keepdims);
Array* isclose(Array *arr1, Array *arr2, double rtol, double atol, int equal_nan);
Array* around(Array *arr, int decimals);
Array* fix(Array *arr);
Array* compress(Array *condition, Array *a, int axis);

/**
 * @brief Sort along the first axis (like numpy.msort)
 * @param a Input array
 * @return New sorted array
 */
Array* msort(Array *a);

/**
 * @brief Cumulative sum along an axis, skipping NaN (like numpy.nancumsum)
 * @param a    Input array
 * @param axis Axis (-1 means flatten)
 * @return Cumulative sum array, same shape as input
 */
Array* nancumsum(Array *a, int axis);

/**
 * @brief Cumulative product along an axis, skipping NaN (like numpy.nancumprod)
 * @param a    Input array
 * @param axis Axis (-1 means flatten)
 * @return Cumulative product array, same shape as input; dtype FLOAT64 or complex
 */
Array* nancumprod(Array *a, int axis);

// Array* broadcast_shapes_(int **shapes, int *ndims, int num_shapes);                         // Broadcast shape computation
Array* broadcast_to(Array *arr, int *shape, int ndim);                                      // Broadcast array to a given shape
Array** broadcast_arrays(Array **arrays, int num_arrays, int *out_ndim, int **out_shape);   // Broadcast multiple arrays to common shape

// Complex number operation functions
Array* conj_transpose(Array *arr);


Array* frombuffer(const void *buffer, DataType dtype, int count, size_t offset);
Array* fromfunction(double (*func)(int*, int), int *shape, int ndim, DataType dtype);
Array* genfromtxt(const char *filename, const char *delimiter, int skip_header);
int gradient(Array *arr, Array *spacing, Array ***out, int *num_out);

// Higher-order operators
double bessel_i0(double arr);
Array* i0(Array *arr);      // Modified Bessel function of the first kind, order 0


#endif // ARRAY_H