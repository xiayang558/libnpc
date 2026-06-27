const FNS = {

// ═════════ Array Creation ═════════
arange: { m:'create', sig:'Array* arange(double start, double stop, double step, DataType dtype)', desc:'Return evenly spaced values within [start, stop). Like numpy.arange.', params:[
  ['start','double','Start of interval (inclusive)'],['stop','double','End of interval (exclusive)'],['step','double','Spacing between values (must be non-zero)'],['dtype','DataType','Output data type — use FLOAT64 for floating-point']
], ret:'1D Array of requested dtype. Length = ceil((stop-start)/step).', notes:'Step must not be zero. If step does not evenly divide the interval, the last value may be less than stop. Always uses double arithmetic internally.', example:`Array *a = arange(1, 10, 2, FLOAT64);   // [1, 3, 5, 7, 9]
Array *b = arange(0, 5, 1, INT32);       // [0, 1, 2, 3, 4]
Array *c = arange(0.0, 1.0, 0.25, FLOAT64); // [0, 0.25, 0.5, 0.75]`, rel:['linspace','logspace','geomspace'] },

linspace: { m:'create', sig:'Array* linspace(float start, float stop, int num)', desc:'Return num evenly spaced samples over [start, stop]. Like numpy.linspace.', params:[
  ['start','float','Start of interval (inclusive)'],['stop','float','End of interval (inclusive)'],['num','int','Number of samples (must be ≥ 2 for proper spacing)']
], ret:'1D FLOAT32 array of length num.', notes:'If num < 2, returns a single-element array containing stop. Dtype is always FLOAT32 — cast with astype() if needed.', example:`Array *x = linspace(0.0f, 1.0f, 5);  // [0, 0.25, 0.5, 0.75, 1.0]
Array *y = linspace(-1.0f, 1.0f, 3); // [-1, 0, 1]`, rel:['arange','logspace','geomspace','astype'] },

logspace: { m:'create', sig:'Array* logspace(double start, double stop, int num, double base, int endpoint)', desc:'Return numbers evenly spaced on a log scale. Like numpy.logspace.', params:[
  ['start','double','Exponent of the first value: base^start'],['stop','double','Exponent of the last value: base^stop'],['num','int','Number of samples'],['base','double','Logarithm base (10.0 for base-10)'],['endpoint','int','If non-zero, include stop; if 0, exclude stop']
], ret:'1D FLOAT64 array of length num.', notes:'For base=10 and start=0,stop=2,num=3: [1, 10, 100]. Values are base^linspace(start,stop,num) internally.', example:`Array *ls = logspace(0, 3, 4, 10.0, 1); // [1, 10, 100, 1000]
Array *ls2 = logspace(0, 2, 3, 2.0, 1);   // [1, 2, 4]`, rel:['linspace','geomspace','arange'] },

geomspace: { m:'create', sig:'Array* geomspace(double start, double stop, int num, int endpoint)', desc:'Return numbers evenly spaced on a geometric progression. Like numpy.geomspace.', params:[
  ['start','double','First value of the sequence'],['stop','double','Last value (if endpoint=1)'],['num','int','Number of samples'],['endpoint','int','If non-zero, include stop; if 0, exclude']
], ret:'1D FLOAT64 array of length num.', notes:'Generates values as exp(linspace(log(start), log(stop), num)). Both start and stop must have the same sign.', example:`Array *gs = geomspace(1, 1000, 4, 1);  // [1, 10, 100, 1000]
Array *gs2 = geomspace(1, 256, 5, 1);    // [1, 4, 16, 64, 256]`, rel:['logspace','linspace'] },

zeros: { m:'create', sig:'Array* zeros(int *shape, int ndim, DataType dtype)', desc:'Return a new array of given shape and dtype, filled with zeros.', params:[
  ['shape','int*','Array of dimension sizes (e.g. (int[]){3,4})'],['ndim','int','Number of dimensions (length of shape array)'],['dtype','DataType','Data type for elements — integer, float, complex, or BOOL']
], ret:'New Array with calloc\'d data (all bytes zero).', notes:'Uses calloc for zero-initialization. For complex, both real and imag are 0. BOOL zeros are false.', example:`int s[] = {3, 4};
Array *z = zeros(s, 2, FLOAT64);   // 3×4 of 0.0
Array *zi = zeros(s, 2, INT32);    // 3×4 of 0`, rel:['ones','full','empty','full_like'] },

ones: { m:'create', sig:'Array* ones(int *shape, int ndim, DataType dtype)', desc:'Return a new array of given shape and dtype, filled with ones.', params:[
  ['shape','int*','Dimension sizes'],['ndim','int','Number of dimensions'],['dtype','DataType','Data type — integer ones = 1, float ones = 1.0, complex ones = 1+0i, BOOL ones = true']
], ret:'New Array with every element set to 1 (or 1.0 / 1+0i / true).', notes:'For complex types, imag part is zero. BOOL ones are true (1).', example:`int s[] = {5};
Array *o = ones(s, 1, FLOAT64);  // [1.0, 1.0, 1.0, 1.0, 1.0]
Array *oi = ones(s, 1, INT32);   // [1, 1, 1, 1, 1]`, rel:['zeros','full','empty','full_like'] },

full: { m:'create', sig:'Array* full(int *shape, int ndim, DataType dtype, void *value)', desc:'Return a new array of given shape and dtype, filled with a scalar value.', params:[
  ['shape','int*','Dimension sizes'],['ndim','int','Number of dimensions'],['dtype','DataType','Data type — must match the type of *value'],['value','void*','Pointer to the fill value — cast to appropriate type (e.g. &myDouble)']
], ret:'New Array with every element == *value.', notes:'The value pointer must point to a variable of the correct C type for the given dtype. For struct types, memcpy is used.', example:`double v = 3.14;
Array *f = full((int[]){2,3}, 2, FLOAT64, &v); // 2×3 of 3.14
int iv = 42;
Array *fi = full((int[]){4}, 1, INT32, &iv);   // [42,42,42,42]`, rel:['zeros','ones','full_like','empty'] },

full_like: { m:'create', sig:'Array* full_like(Array *arr, void *value, DataType dtype)', desc:'Return an array with the same shape as arr, filled with value.', params:[
  ['arr','Array*','Template array — shape and ndim are copied'],['value','void*','Pointer to fill value'],['dtype','DataType','Output data type (may differ from arr->dtype)']
], ret:'New Array, same shape as arr, dtype as specified.', example:`double v = -1.0;
Array *f = full_like(existingArray, &v, FLOAT64);`, rel:['full','zeros','ones','copy'] },

empty: { m:'create', sig:'Array* empty(int *shape, int ndim, DataType dtype)', desc:'Return an uninitialized array. Contents are undefined. Like numpy.empty.', params:[
  ['shape','int*','Dimension sizes'],['ndim','int','Number of dimensions'],['dtype','DataType','Data type']
], ret:'New Array with malloc\'d but uninitialized data.', notes:'Faster than zeros() for large arrays since no zero-fill is done. Only use when you will overwrite every element.', example:`int s[] = {1000, 1000};
Array *buf = empty(s, 2, FLOAT64);  // allocate, no init
for (int i=0; i<buf->size; i++) ((double*)buf->data)[i] = i;`, rel:['zeros','ones','full'] },

eye: { m:'create', sig:'Array* eye(int N, int M, int k, DataType dtype)', desc:'Return a 2-D array with ones on the k-th diagonal and zeros elsewhere.', params:[
  ['N','int','Number of rows'],['M','int','Number of columns'],['k','int','Diagonal offset — 0 = main diagonal, >0 = above main, <0 = below main'],['dtype','DataType','Data type for the array']
], ret:'N×M Array.', notes:'When k=0 this is the identity-like matrix (square ones on diagonal). For non-square matrices, the diagonal is truncated at the edges.', example:`Array *I = eye(3, 3, 0, FLOAT64);     // 3×3 identity
Array *D = eye(4, 4, 1, FLOAT64);     // super-diagonal of ones
Array *L = eye(4, 4, -1, FLOAT64);    // sub-diagonal of ones`, rel:['identity','diag','diagflat','tri'] },

identity: { m:'create', sig:'Array* identity(int n, DataType dtype)', desc:'Return the n×n identity matrix (ones on main diagonal, zeros elsewhere).', params:[
  ['n','int','Size of the square matrix'],['dtype','DataType','Data type']
], ret:'n×n Array.', notes:'Shorthand for eye(n, n, 0, dtype).', example:`Array *I = identity(4, FLOAT64);  // 4×4 identity matrix`, rel:['eye','diag'] },

diag: { m:'create', sig:'Array* diag(Array *v, int k)', desc:'Extract a diagonal or construct a diagonal array. Like numpy.diag.', params:[
  ['v','Array*','Input — if 1-D builds a diagonal matrix; if 2-D extracts the k-th diagonal'],['k','int','Diagonal offset — 0 = main, >0 = above, <0 = below']
], ret:'If v is 1-D → square 2-D diagonal matrix. If v is 2-D → 1-D diagonal vector.', notes:'If v is 2-D, the extracted diagonal length is min(rows, cols − |k|). Diagonals beyond matrix bounds yield empty results.', example:`Array *v = arange(1, 4, 1, FLOAT64);
Array *D = diag(v, 0);    // [[1,0,0],[0,2,0],[0,0,3]]
Array *d = diag(D, 1);   // [0, 0] — super-diagonal of D`, rel:['diagflat','diagonal','eye','fill_diagonal'] },

diagflat: { m:'create', sig:'Array* diagflat(Array *arr, int k)', desc:'Create a 2-D array with the flattened input on the k-th diagonal.', params:[
  ['arr','Array*','Input array — flattened first, then placed on diagonal'],['k','int','Diagonal offset']
], ret:'Square 2-D Array of size = |k| + len(flattened arr).', example:`Array *a = arange(1, 4, 1, FLOAT64);
Array *D = diagflat(a, 0);  // [[1,0,0],[0,2,0],[0,0,3]]`, rel:['diag','eye'] },

tri: { m:'create', sig:'Array* tri(int N, int M, int k, DataType dtype)', desc:'Return an array with ones at and below the k-th diagonal, zeros above.', params:[
  ['N','int','Number of rows'],['M','int','Number of columns'],['k','int','Diagonal offset (default 0 for main diagonal and below)'],['dtype','DataType','Data type']
], ret:'N×M Array.', example:`Array *L = tri(3, 3, 0, FLOAT64);  // [[1,0,0],[1,1,0],[1,1,1]]
Array *U = tri(3, 3, -1, FLOAT64); // [[0,0,0],[1,0,0],[1,1,0]]`, rel:['tril','triu','eye'] },

vander: { m:'create', sig:'Array* vander(Array *x, int N, int increasing)', desc:'Generate a Vandermonde matrix. Columns are powers of input x.', params:[
  ['x','Array*','1-D input values'],['N','int','Number of columns; if 0 or > len(x), uses len(x)'],['increasing','int','0 = decreasing powers (x^(N-1) … x^0, numpy default); 1 = increasing (x^0 … x^(N-1))']
], ret:'len(x) × N FLOAT64 matrix.', notes:'Column j (increasing=0): x[i]^(N-1-j). Column j (increasing=1): x[i]^j.', example:`Array *x = arange(1, 4, 1, FLOAT64);
Array *V = vander(x, 3, 0);  // [[1,1,1],[4,2,1],[9,3,1]]
Array *Vi = vander(x, 3, 1); // [[1,1,1],[1,2,4],[1,3,9]]`, rel:['poly','polyfit','polyval'] },

meshgrid: { m:'create', sig:'Array* meshgrid(Array **arrays, int narrays, const char *indexing)', desc:'Return coordinate matrices from 1-D coordinate vectors.', params:[
  ['arrays','Array**','Array of narrays 1-D input arrays (modified in place to become N-D grids)'],['narrays','int','Number of input arrays'],['indexing','const char*','"xy" = Cartesian (first dim varies slowest); "ij" = matrix (first dim varies fastest)']
], ret:'Returns first array pointer; all inputs are reshaped in place to broadcastable N-D form.', notes:'Unlike numpy which returns a list, this modifies the input arrays in place. After calling, each arrays[i] is expanded with newaxis to form an N-D grid.', example:`Array *x = arange(1,4,1,FLOAT64), *y = arange(1,5,1,FLOAT64);
Array *in[2] = {x, y};
meshgrid(in, 2, "xy");  // x→3×4, y→3×4 coordinate grids`, rel:['ix_','broadcast_arrays'] },

copy: { m:'create', sig:'Array* copy(Array *arr)', desc:'Return a deep copy of the array (new memory allocation).', params:[
  ['arr','Array*','Input array']
], ret:'New Array with identical shape, dtype, and data.', notes:'Copies all data even if arr is a view. Strides are NOT copied — the result is always contiguous.', example:`Array *b = copy(a);  // independent copy; freeing a does not affect b`, rel:['astype','flatten','reshape'] },

astype: { m:'create', sig:'Array* astype(Array *arr, DataType new_dtype)', desc:'Cast array elements to a new data type. Like numpy.ndarray.astype.', params:[
  ['arr','Array*','Source array'],['new_dtype','DataType','Target data type — any valid DataType enum value']
], ret:'New Array with new_dtype, same shape.', notes:'Complex→real takes the real part only. Real→complex sets imag=0. Integer→float is exact within range. Float→integer truncates (toward zero). BOOL→anything: false=0, true=1.', example:`Array *d = astype(intArr, FLOAT64);     // INT32→FLOAT64
Array *r = astype(complexArr, FLOAT64);  // COMPLEX128→FLOAT64 (real part)
Array *c = astype(realArr, COMPLEX128); // FLOAT64→COMPLEX128 (imag=0)`, rel:['copy','dtype_name','dtype_size'] },

frombuffer: { m:'create', sig:'Array* frombuffer(const void *buffer, DataType dtype, int count, size_t offset)', desc:'Interpret a raw memory buffer as an array (view, no copy).', params:[
  ['buffer','const void*','Pointer to raw memory'],['dtype','DataType','Interpretation type for the bytes'],['count','int','Number of elements to interpret'],['offset','size_t','Byte offset into buffer before first element']
], ret:'Array VIEW (is_view=1) sharing the buffer memory.', notes:'The returned array is a VIEW. The caller must keep the original buffer alive. No bounds checking is performed on the buffer.', example:`double raw[] = {1,2,3,4,5,6};
Array *a = frombuffer(raw, FLOAT64, 6, 0); // interprets raw as 6 doubles
// CAUTION: do NOT free raw until done with a`, rel:['fromfunction','copy'] },

fromfunction: { m:'create', sig:'Array* fromfunction(double (*func)(int*, int), int *shape, int ndim, DataType dtype)', desc:'Construct an array by evaluating a function over each coordinate.', params:[
  ['func','double(*)(int*,int)','Callback: receives indices array and ndim, returns value for that coordinate'],['shape','int*','Output array shape'],['ndim','int','Number of dimensions'],['dtype','DataType','Output data type']
], ret:'Array filled by calling func at every coordinate.', example:`double myfn(int *idx, int ndim) { return idx[0] + idx[1]*10.0; }
Array *a = fromfunction(myfn, (int[]){3,4}, 2, FLOAT64);
// [[0,10,20,30],[1,11,21,31],[2,12,22,32]]`, rel:['frombuffer','arange'] },

// ── Shape & Manipulation ──
reshape: { m:'shape', sig:'Array* reshape(Array *arr, int *new_shape, int new_ndim)', desc:'Give a new shape to an array without changing its data.', params:[
  ['arr','Array*','Input array'],['new_shape','int*','Array of new dimension sizes'],['new_ndim','int','New number of dimensions']
], ret:'VIEW with new shape (same data pointer). Returns NULL if total size mismatch.', notes:'Returns a VIEW — modifying the result modifies the original. Total elements must equal arr->size exactly.', example:`int ns[] = {4, 3};
Array *r = reshape(arange(1,13,1,FLOAT64), ns, 2);
// 1×12 → 4×3`, rel:['resize','flatten','ravel','transpose','squeeze'] },

resize: { m:'shape', sig:'Array* resize(Array *arr, int *new_shape, int new_ndim)', desc:'Return a new array with the specified shape, repeating or truncating data.', params:[
  ['arr','Array*','Input (flattened before resizing)'],['new_shape','int*','New dimension sizes'],['new_ndim','int','New number of dimensions']
], ret:'NEW array (always a copy) of requested shape.', notes:'If new size > original size, elements are repeated cyclically. If smaller, elements are truncated. Unlike reshape, this always allocates new memory.', example:`Array *a = arange(1,5,1,FLOAT64);
Array *b = resize(a, (int[]){2,4}, 2);  // 2×4, repeats [1,2,3,4,1,2,3,4]
Array *c = resize(a, (int[]){2}, 1);    // [1,2]`, rel:['reshape','tile','repeat'] },

transpose: { m:'shape', sig:'Array* transpose(Array *arr)', desc:'Reverse the axes of an array. For 2-D this is the standard matrix transpose.', params:[
  ['arr','Array*','Input array of any dimension']
], ret:'VIEW with all axes reversed (shape[i] → output.shape[ndim-1-i]).', notes:'Returns a VIEW — data is shared with original. For conjugate transpose of complex matrices, use conj_transpose().', example:`int s[] = {3, 4};
Array *A = ones(s, 2, FLOAT64);
Array *T = transpose(A);  // T is 4×3, same data`, rel:['conj_transpose','reshape','rot90'] },

flatten: { m:'shape', sig:'Array* flatten(Array *arr)', desc:'Return a 1-D copy of the array (always allocates new memory).', params:[
  ['arr','Array*','Input array of any shape']
], ret:'1-D Array (copy) of same dtype, length = arr->size.', notes:'Unlike ravel which returns a view when possible, flatten always copies. Safe to modify result without affecting original.', example:`Array *flat = flatten(A);  // A(3×4) → flat(12)`, rel:['ravel','reshape','copy'] },

ravel: { m:'shape', sig:'Array* ravel(Array *arr, const char *order)', desc:'Return a 1-D array. "C" = row-major (default), "F" = column-major.', params:[
  ['arr','Array*','Input array'],['order','const char*','"C" (row-major, default) or "F" (column-major)']
], ret:'1-D Array.', notes:'For C-order on contiguous arrays, returns a view. For F-order, allocates a copy.', example:`Array *r = ravel(A, "C"); // row-major flatten`, rel:['flatten','reshape'] },

squeeze: { m:'shape', sig:'Array* squeeze(Array *arr, int axis)', desc:'Remove length-1 dimensions from an array.', params:[
  ['arr','Array*','Input array'],['axis','int','Specific axis to squeeze, or -1 to remove ALL length-1 dimensions']
], ret:'VIEW with length-1 dims removed.', notes:'If no dimension has length 1, returns the original array as-is (view). Cannot squeeze a dimension whose size ≠ 1.', example:`int s[] = {1, 3, 1, 4};
Array *a = zeros(s, 4, FLOAT64);
Array *b = squeeze(a, -1);  // shape (3, 4)`, rel:['expand_dims','reshape'] },

expand_dims: { m:'shape', sig:'Array* expand_dims(Array *arr, int axis)', desc:'Insert a new axis of length 1 at the given position.', params:[
  ['arr','Array*','Input array'],['axis','int','Position for the new axis (0..ndim, where ndim appends at end)']
], ret:'VIEW with one extra dimension of size 1.', example:`int s[] = {3, 4};
Array *a = ones(s, 2, FLOAT64);
Array *b = expand_dims(a, 0);  // shape (1, 3, 4)
Array *c = expand_dims(a, -1); // shape (3, 4, 1)`, rel:['squeeze','reshape'] },

tile: { m:'shape', sig:'Array* tile(Array *arr, int *reps, int nreps)', desc:'Construct an array by repeating arr reps[i] times along each dimension.', params:[
  ['arr','Array*','Input array'],['reps','int*','Repetition factors, one per dimension of arr'],['nreps','int','Length of reps (must equal arr->ndim)']
], ret:'New array with shape arr->shape[i] * reps[i].', example:`int s[] = {2, 2}, reps[] = {2, 3};
Array *a = arange(1,5,1,FLOAT64); a = reshape(a, s, 2);
Array *t = tile(a, reps, 2); // 4×6`, rel:['repeat','broadcast_to','resize'] },

repeat: { m:'shape', sig:'Array* repeat(Array *arr, Array *repeats, int axis)', desc:'Repeat each element of the array along a given axis.', params:[
  ['arr','Array*','Input array'],['repeats','Array*','INT64 array of repetition counts — must be broadcastable to arr.shape[axis]'],['axis','int','Axis along which to repeat values; -1 = flatten first']
], ret:'New array with repeated elements along axis.', example:`int s[] = {3};
Array *a = arange(1,4,1,FLOAT64);
Array *rpt = full((int[]){3},1,INT64,&(int64_t){2});
Array *rp = repeat(a, rpt, 0);  // [1,1, 2,2, 3,3]`, rel:['tile','take','broadcast_to'] },

stack: { m:'shape', sig:'Array* stack(Array **arrays, int num_arrays, int axis)', desc:'Join a sequence of arrays along a new axis. All must have identical shape.', params:[
  ['arrays','Array**','Array of pointers to arrays to stack'],['num_arrays','int','Number of arrays to stack'],['axis','int','Position of the new axis in the result']
], ret:'Array with one extra dimension compared to inputs.', notes:'Input shape (s0,s1,…) → output shape with axis inserted: (s0,…,s(axis-1), num_arrays, s(axis),…).', example:`Array *a = arange(1,4,1,FLOAT64), *b = arange(4,7,1,FLOAT64);
Array *arrs[] = {a, b};
Array *s = stack(arrs, 2, 0);  // shape (2, 3)`, rel:['hstack','vstack','dstack','concatenate'] },

hstack: { m:'shape', sig:'Array* hstack(Array **arrays, int num_arrays)', desc:'Stack arrays horizontally (column-wise). 1-D → stack along axis 0; 2-D+ → stack along axis 1.', params:[
  ['arrays','Array**','Array of pointers to arrays'],['num_arrays','int','Number of arrays to stack']
], example:`Array *a = arange(1,3,1,FLOAT64), *b = arange(3,5,1,FLOAT64);
Array *hs = hstack((Array*[]){a,b}, 2); // [1,2,3,4]`, rel:['vstack','dstack','stack','column_stack'] },

vstack: { m:'shape', params:[['arrays','Array**','Arrays to stack'],['num_arrays','int','Count']], sig:'Array* vstack(Array **arrays, int num_arrays)', desc:'Stack arrays vertically (row-wise). 1-D → reshape to (1,N) then stack; 2-D+ → concatenate axis 0.', rel:['hstack','dstack','stack'] , example:'Array *vs = vstack((Array*[]){a,b}, 2);',},

dstack: { m:'shape', params:[['arrays','Array**','Arrays to stack'],['num_arrays','int','Count']], sig:'Array* dstack(Array **arrays, int num_arrays)', desc:'Stack arrays depth-wise (along third axis).', rel:['hstack','vstack','stack'] , example:'Array *ds = dstack((Array*[]){a,b}, 2);',},

column_stack: { m:'shape', sig:'Array* column_stack(Array **arrays, int num_arrays)', desc:'Stack 1-D arrays as columns into a 2-D array.', params:[['arrays','Array**','1-D arrays to stack'],['num_arrays','int','Count']], example:`Array *a = arange(1,4,1,FLOAT64), *b = arange(4,7,1,FLOAT64);
Array *cs = column_stack((Array*[]){a,b}, 2); // 3×2`, rel:['hstack','stack'] },

concatenate: { m:'shape', sig:'Array* concatenate(Array *arr1, Array *arr2, int axis)', desc:'Join two arrays along an existing axis.', params:[
  ['arr1','Array*','First array'],['arr2','Array*','Second array (must match arr1 on all dims except axis)'],['axis','int','Existing axis to concatenate along']
], ret:'New array with combined axis.', example:`int s[]={3,4};
Array *a = ones(s,2,FLOAT64), *b = zeros(s,2,FLOAT64);
Array *c = concatenate(a, b, 0);  // shape (6, 4)`, rel:['stack','hstack','vstack','append'] },

split: { m:'shape', sig:'Array** split(Array *arr, int sections, int axis, int *nout)', desc:'Split an array into sections sub-arrays along axis.', params:[
  ['arr','Array*','Input array'],['sections','int','Number of equal-sized sections'],['axis','int','Axis to split along'],['nout','int*','Output: number of result arrays']
], ret:'Array of section Array* pointers. Caller must free each + the outer array.', notes:'Array must be evenly divisible along axis.', rel:['hsplit','vsplit','dsplit'] , example:'int n; Array **parts = split(A, 3, 0, &n);',},

hsplit: { m:'shape', params:[['arr','Array*','Input'],['sections','int','Number'],['nout','int*','Output count']], sig:'Array** hsplit(Array *arr, int sections, int *nout)', desc:'Split horizontally (along axis=1).', rel:['vsplit','split'] , example:'int n; Array **parts = hsplit(A, 2, &n);',},
vsplit: { m:'shape', params:[['arr','Array*','Input'],['sections','int','Number'],['nout','int*','Output count']], sig:'Array** vsplit(Array *arr, int sections, int *nout)', desc:'Split vertically (along axis=0).', rel:['hsplit','split'] , example:'int n; Array **parts = vsplit(A, 2, &n);',},
dsplit: { m:'shape', params:[['arr','Array*','Input (>=3-D)'],['sections','int','Number'],['nout','int*','Output count']], sig:'Array** dsplit(Array *arr, int sections, int *nout)', desc:'Split depth-wise (along axis=2).', rel:['hsplit','vsplit'] , example:'int n; Array **parts = dsplit(volume, 2, &n);',},

flip: { m:'shape', sig:'Array* flip(Array *arr, int axis)', desc:'Reverse the order of elements along a given axis.', params:[
  ['arr','Array*','Input array'],['axis','int','Axis to reverse']
], example:`Array *a = arange(1,7,1,FLOAT64); a = reshape(a, (int[]){2,3}, 2);
Array *f = flip(a, 0);  // rows reversed`, rel:['fliplr','flipud','roll'] },

fliplr: { m:'shape', sig:'Array* fliplr(Array *arr)', desc:'Reverse array left-right (flip along axis=1).', params:[['arr','Array*','Input (>=2-D)']], example:`Array *lr = fliplr(A);  // columns reversed`, rel:['flip','flipud'] },
flipud: { m:'shape', sig:'Array* flipud(Array *arr)', desc:'Reverse array up-down (flip along axis=0).', params:[['arr','Array*','Input (>=2-D)']], rel:['flip','fliplr'] , example:'Array *ud = flipud(A);'},

roll: { m:'shape', sig:'Array* roll(Array *arr, int shift, int axis)', desc:'Roll array elements along a given axis.', params:[
  ['arr','Array*','Input array'],['shift','int','Number of positions to shift (positive=right/down, negative=left/up)'],['axis','int','Axis to roll along; -1 = flatten first, roll, then reshape']
], example:`Array *a = arange(1,6,1,FLOAT64);
Array *r = roll(a, 2, 0);  // [4,5,1,2,3]`, rel:['flip','rot90'] },

rot90: { m:'shape', sig:'Array* rot90(Array *m, int k, int axis1, int axis2)', desc:'Rotate array by 90 degrees counter-clockwise k times.', params:[
  ['m','Array*','Input (at least 2-D)'],['k','int','Number of 90° rotations (1..3 typical; 4 = identity)'],['axis1','int','First axis of the rotation plane (default 0)'],['axis2','int','Second axis of the rotation plane (default 1)']
], example:`Array *a = arange(1,7,1,FLOAT64); a = reshape(a, (int[]){2,3}, 2);
Array *r = rot90(a, 1, 0, 1);  // 3×2 rotated CCW`, rel:['transpose','flip'] },

take: { m:'shape', sig:'Array* take(Array *arr, Array *indices, int axis)', desc:'Take elements from an array along an axis using an index array.', params:[
  ['arr','Array*','Source array'],['indices','Array*','INT64 index array (any shape; flattened first)'],['axis','int','Axis to index into']
], ret:'Array with indices->shape along axis, arr->shape elsewhere.', example:`int s[]={3,4}; Array *A = arange(1,13,1,FLOAT64); A = reshape(A,s,2);
Array *idx = create_array((int[]){2},1,INT64); ((int64_t*)idx->data)[0]=0; ((int64_t*)idx->data)[1]=2;
Array *t = take(A, idx, 0);  // rows 0 and 2 → shape (2,4)`, rel:['compress','extract','choose','put'] },

broadcast_to: { m:'shape', sig:'Array* broadcast_to(Array *arr, int *shape, int ndim)', desc:'Broadcast an array to a new shape without copying data.', params:[
  ['arr','Array*','Input array'],['shape','int*','Target shape (must be broadcastable)'],['ndim','int','Number of target dimensions']
], ret:'VIEW with target shape. New dims are prepended; existing dims must be 1 or match.', notes:'Returns a VIEW — data is NOT copied. Use copy() if you need independent memory.', example:`int s[]={3,1};
Array *a = arange(1,4,1,FLOAT64); a = reshape(a, s, 2);
Array *b = broadcast_to(a, (int[]){3,5}, 2); // a broadcast from (3,1) to (3,5)`, rel:['broadcast_arrays','tile','expand_dims'] },

broadcast_arrays: { m:'shape', sig:'Array** broadcast_arrays(Array **arrays, int num_arrays, int *out_ndim, int **out_shape)', desc:'Broadcast multiple arrays to a common shape.', params:[['arrays','Array**','Input arrays'],['num_arrays','int','Count'],['out_ndim','int*','Output: common ndim'],['out_shape','int**','Output: common shape']], rel:['broadcast_to'] , example:'int nd; int *sh; Array** bc = broadcast_arrays(arrs, 3, &nd, &sh);',},

atleast_1d: { m:'shape', sig:'Array* atleast_1d(Array *a)', desc:'View inputs as arrays with at least one dimension. Scalars → 1-element 1-D.', params:[['a','Array*','Input']], ret:'Array with ndim ≥ 1.', example:`Array *s = create_scalar_array(&(double){5.0}, FLOAT64); // ndim=0
Array *v = atleast_1d(s);  // ndim=1, shape [1]`, rel:['atleast_2d','atleast_3d','squeeze'] },
atleast_2d: { m:'shape', sig:'Array* atleast_2d(Array *a)', desc:'View inputs as arrays with at least two dimensions.', params:[['a','Array*','Input']], rel:['atleast_1d','atleast_3d'] , example:'Array *m = atleast_2d(vector);'},
atleast_3d: { m:'shape', sig:'Array* atleast_3d(Array *a)', desc:'View inputs as arrays with at least three dimensions.', params:[['a','Array*','Input']], rel:['atleast_1d','atleast_2d'] , example:'Array *vol = atleast_3d(mat);'},

block: { m:'shape', sig:'Array* block(Array **blocks, int rows, int cols)', desc:'Assemble an array from a grid of blocks.', params:[
  ['blocks','Array**','Row-major list of rows×cols arrays'],['rows','int','Number of block rows'],['cols','int','Number of block columns']
], example:`Array *a = ones((int[]){2,2},2,FLOAT64), *b = zeros((int[]){2,2},2,FLOAT64);
Array *blk = block((Array*[]){a,b,b,a}, 2, 2); // 4×4 checkerboard`, rel:['stack','concatenate','bmat'] },

kron: { m:'shape', sig:'Array* kron(Array *a, Array *b)', desc:'Kronecker product — each element of a is multiplied by the entire array b.', params:[
  ['a','Array*','First factor'],['b','Array*','Second factor']
], ret:'Array with shape = a.shape × b.shape (element-wise product of dims).', example:`Array *a = eye(2,2,0,FLOAT64);
Array *b = ones((int[]){3,2},2,FLOAT64);
Array *k = kron(a, b); // 6×4 block-diagonal`, rel:['outer','tensordot'] },

pad: { m:'shape', sig:'Array* pad(Array *arr, Array *pad_width, const char *mode, double constant_value)', desc:'Pad an array along each dimension.', params:[
  ['arr','Array*','Input array'],['pad_width','Array*','1D INT64 array: (before_0, after_0, before_1, after_1, …) — length 2*ndim'],['mode','const char*','"constant" (fill value), "edge" (repeat edge), "reflect", "symmetric", "wrap"'],['constant_value','double','Fill value for "constant" mode (ignored for other modes)']
], ret:'Padded array, same dtype as arr.', example:`int s[]={3,4}; Array *a = ones(s,2,FLOAT64);
int64_t pw[4]={1,1,2,2};
Array *pp = create_array((int[]){4},1,INT64); memcpy(pp->data,pw,4*sizeof(int64_t));
Array *padded = pad(a, pp, "constant", 0.0); // pad 1 row top/bottom, 2 cols left/right`, rel:['tile','broadcast_to'] },

// ── Arithmetic ──
add: { m:'arith', sig:'Array* add(Array *x1, Array *x2)', desc:'Element-wise addition with broadcasting. Like numpy.add.', params:[
  ['x1','Array*','First operand'],['x2','Array*','Second operand']
], ret:'Array with broadcast shape; dtype is the promoted type of x1 and x2.', notes:'Integer overflow triggers a warning via NP_MUL_OVERFLOW macros. Complex numbers are supported.', example:`Array *a = arange(1,5,1,FLOAT64);
Array *b = ones((int[]){4},1,FLOAT64);
Array *c = add(a, b);  // [2,3,4,5]
Array *d = add(a, a);  // [2,4,6,8]`, rel:['subtract','multiply','divide','add_scalar'] },

subtract: { m:'arith', sig:'Array* subtract(Array *x1, Array *x2)', desc:'Element-wise subtraction with broadcasting.', params:[['x1','Array*','Minuend'],['x2','Array*','Subtrahend']], ret:'Array with broadcast shape, promoted dtype.', example:`Array *c = subtract(a, b);  // a - b element-wise`, rel:['add','multiply','divide'] },

multiply: { m:'arith', sig:'Array* multiply(Array *x1, Array *x2)', desc:'Element-wise multiplication with broadcasting.', params:[['x1','Array*','First factor'],['x2','Array*','Second factor']], ret:'Array with broadcast shape, promoted dtype.', example:`Array *c = multiply(a, b);  // a[i] * b[i]`, rel:['add','divide','multiply_scalar'] },

divide: { m:'arith', sig:'Array* divide(Array *x1, Array *x2)', desc:'Element-wise division. Integer division truncates toward zero.', params:[['x1','Array*','Dividend'],['x2','Array*','Divisor']], ret:'Array with broadcast shape. For integers, same dtype; for floats, same dtype.', notes:'Division by zero → returns NULL with error to stderr. Use true_divide for guaranteed floating-point output.', example:`Array *q = divide(A, B);  // A / B element-wise`, rel:['true_divide','multiply','mod'] },

true_divide: { m:'arith', sig:'Array* true_divide(Array *x1, Array *x2)', desc:'Floating-point division. Always returns FLOAT64 regardless of input types.', params:[['x1','Array*','Dividend'],['x2','Array*','Divisor']], ret:'FLOAT64 array of broadcast shape.', example:`Array *q = true_divide(intA, intB); // FLOAT64 output even for int inputs`, rel:['divide','multiply'] },

power: { m:'arith', sig:'Array* power(Array *base, Array *exp)', desc:'Element-wise base raised to exp power, with broadcasting.', params:[
  ['base','Array*','Base values (converted to double)'],['exp','Array*','Exponent values (converted to double)']
], ret:'FLOAT64 array of broadcast shape.', notes:'Both arrays converted to double. Negative base with non-integer exponent → NaN. 0^0 → 1.0.', example:`Array *a = arange(1,4,1,FLOAT64); // [1,2,3]
Array *b = full((int[]){3},1,FLOAT64,&(double){2}); // [2,2,2]
Array *p = power(a, b);  // [1,4,9]`, rel:['exp_array','log_array','sqrt_array'] },

mod: { m:'arith', sig:'Array* mod(Array *arr1, Array *arr2)', desc:'Element-wise modulo. Result sign matches divisor (Python/NumPy behavior).', params:[
  ['arr1','Array*','Dividend (real only)'],['arr2','Array*','Divisor (real only)']
], ret:'FLOAT64 array of broadcast shape.', notes:'Complex not supported. Divide-by-zero → result element is 0 (no error). Uses fmod with sign correction.', example:`Array *a = arange(1,6,1,FLOAT64);
Array *r = mod(a, full((int[]){5},1,FLOAT64,&(double){3})); // [1,2,0,1,2]
Array *rn = mod(arange(-3,3,1,FLOAT64), full((int[]){6},1,FLOAT64,&(double){3})); // [0,1,2,0,1,2]`, rel:['mod_scalar','divide','true_divide'] },

mod_scalar: { m:'arith', sig:'Array* mod_scalar(Array *arr, void *scalar)', desc:'Element-wise modulo with a scalar divisor. Result always FLOAT64.', params:[
  ['arr','Array*','Dividend array (real, any shape)'],['scalar','void*','Pointer to divisor value (type must match arr->dtype)']
], ret:'FLOAT64 array, same shape as arr.', notes:'divisor=0 → result filled with 0. Complex not supported. Sign of result matches divisor.', example:`double d = 3.0;
Array *r = mod_scalar(arr, &d);  // arr[i] % 3.0`, rel:['mod','divide'] },

ahypot: { m:'arith', sig:'Array* ahypot(Array *x1, Array *x2)', desc:'sqrt(x1² + x2²) element-wise. Always returns FLOAT64.', params:[
  ['x1','Array*','First array (real)'],['x2','Array*','Second array (real)']
], ret:'FLOAT64 array of broadcast shape.', notes:'Uses libm hypot() for numerical stability (avoids overflow in intermediate squares). Complex not supported.', example:`Array *h = ahypot(x, y);  // sqrt(x² + y²) — hypotenuse`, rel:['sqrt_array','abs_array','add','multiply'] },

sqrt_array: { m:'arith', sig:'Array* sqrt_array(Array *arr)', desc:'Element-wise square root, √x.', params:[['arr','Array*','Input (any numeric dtype)']], ret:'Integer→FLOAT32. Float→same float type. Complex→same complex type.', notes:'Negative real input → returns NULL + error to stderr. Negative complex → produces complex result.', example:`Array *s = sqrt_array(arr);  // sqrt of each element`, rel:['exp_array','log_array','power'] },

exp_array: { m:'arith', sig:'Array* exp_array(Array *arr)', desc:'Element-wise exponential, e^x.', params:[['arr','Array*','Input']], ret:'Integer→FLOAT32. Float/Complex→same type.', example:`Array *e = exp_array(arr);  // e^x for each element`, rel:['log_array','log2_array','log10_array','power'] },

log_array: { m:'arith', sig:'Array* log_array(Array *arr)', desc:'Element-wise natural logarithm, ln(x).', params:[['arr','Array*','Input (real or complex)']], ret:'Integer→FLOAT64. Float→FLOAT64. Complex→COMPLEX128.', notes:'Non-positive real input → returns NULL with error to stderr.', example:`Array *l = log_array(arr);  // ln(x) for each element`, rel:['exp_array','log2_array','log10_array'] },

log2_array: { m:'arith', sig:'Array* log2_array(Array *arr)', desc:'Element-wise base-2 logarithm.', params:[['arr','Array*','Input (real or complex)']], example:`Array *l = log2_array(arr);`, rel:['log_array','log10_array'] },
log10_array: { m:'arith', sig:'Array* log10_array(Array *arr)', desc:'Element-wise base-10 logarithm.', rel:['log_array','log2_array'] , example:'Array *l10 = log10_array(x);'},

abs_array: { m:'arith', sig:'Array* abs_array(Array *arr)', desc:'Element-wise absolute value (real) or magnitude (complex).', params:[['arr','Array*','Input (any numeric dtype)']], ret:'Real→same type. Complex→real (FLOAT32 for COMPLEX64, FLOAT64 for COMPLEX128).', example:`Array *a = abs_array(arr);  // |x| or magnitude for complex`, rel:['fabs_array','sign'] },

sign: { m:'arith', sig:'Array* sign(Array *arr)', desc:'Element-wise sign indicator: -1, 0, or +1.', params:[['arr','Array*','Input (BOOL, int, uint, or float; complex not supported)']], ret:'Same dtype as input.', notes:'Unsigned types: only 0 or 1. NaN stays NaN for float types.', example:`Array *s = sign(a);  // [-1.0, 0.0, 1.0, 1.0, -1.0]`, rel:['abs_array','heaviside'] },

sinc: { m:'arith', sig:'Array* sinc(Array *arr)', desc:'sinc(x) = sin(πx) / (πx), with sinc(0) = 1.', params:[['arr','Array*','Input (real)']], ret:'FLOAT64 for most types, FLOAT32 for FLOAT32 input.', example:`Array *s = sinc(x);`, rel:['sin_array'] },

heaviside: { m:'arith', sig:'Array* heaviside(Array *x, double h)', desc:'Heaviside step function. 0 for x<0, h for x=0, 1 for x>0.', params:[
  ['x','Array*','Input (real, any dtype)'],['h','double','Value to return when x == 0 (commonly 0, 0.5, or 1)']
], ret:'FLOAT64 array, same shape as x.', example:`Array *h = heaviside(x, 0.5);  // step with midpoint`, rel:['sign','where','clip'] },

floor_array: { m:'arith', sig:'Array* floor_array(Array *arr)', desc:'Floor — round down to nearest integer.', params:[['arr','Array*','Input (real)']], ret:'Integer→FLOAT64. Float→same float type.', example:`Array *f = floor_array(arr);`, rel:['ceil_array','arint','atrunc','around'] },
ceil_array: { m:'arith', sig:'Array* ceil_array(Array *arr)', desc:'Ceiling — round up to nearest integer.', params:[['arr','Array*','Input (real)']], example:`Array *c = ceil_array(arr);`, rel:['floor_array','arint','around'] },
arint: { m:'arith', sig:'Array* arint(Array *arr)', desc:'Round to nearest integer, ties to even (banker\'s rounding). Like numpy.rint.', params:[['arr','Array*','Input (real)']], example:`Array *r = arint(arr);`, rel:['floor_array','ceil_array','atrunc','around'] },
atrunc: { m:'arith', sig:'Array* atrunc(Array *arr)', desc:'Truncate toward zero. Like numpy.trunc.', params:[['arr','Array*','Input (real)']], example:`Array *t = atrunc(arr);`, rel:['floor_array','fix','around'] },

logaddexp: { m:'arith', sig:'Array* logaddexp(Array *x1, Array *x2)', desc:'log(exp(x1) + exp(x2)) — numerically stable using log1p.', params:[
  ['x1','Array*','First array (real)'],['x2','Array*','Second array (real)']
], ret:'FLOAT64 array of broadcast shape.', notes:'Handles -∞ correctly: logaddexp(-∞, x) = x.', example:`Array *r = logaddexp(a, b);  // log(e^a + e^b)`, rel:['logaddexp2','exp_array','log_array'] },

logaddexp2: { m:'arith', sig:'Array* logaddexp2(Array *x1, Array *x2)', desc:'log2(2^x1 + 2^x2) — numerically stable.', params:[['x1','Array*','First'],['x2','Array*','Second']], ret:'FLOAT64 array.', rel:['logaddexp','exp_array'] , example:'Array *r = logaddexp2(a, b);',},

// ── Trigonometry ──
sin_array: { m:'trig', sig:'Array* sin_array(Array *arr)', desc:'Element-wise sine, sin(x).', params:[['arr','Array*','Input (radians)']], ret:'Integer→FLOAT64. Float→same float. Complex→same complex.', example:`Array *s = sin_array(angles);  // sin of each angle`, rel:['cos_array','tan_array','arcsin'] },
cos_array: { m:'trig', sig:'Array* cos_array(Array *arr)', desc:'Element-wise cosine, cos(x).', params:[['arr','Array*','Input (radians)']], rel:['sin_array','tan_array','arccos'] , example:'Array *c = cos_array(angles);'},
tan_array: { m:'trig', sig:'Array* tan_array(Array *arr)', desc:'Element-wise tangent, tan(x).', params:[['arr','Array*','Input (radians)']], rel:['sin_array','cos_array','arctan'] , example:'Array *t = tan_array(angles);'},
sinh_array: { m:'trig', sig:'Array* sinh_array(Array *arr)', desc:'Element-wise hyperbolic sine, sinh(x).', params:[['arr','Array*','Input']], rel:['cosh_array','tanh_array','arcsinh'] , example:'Array *sh = sinh_array(x);'},
cosh_array: { m:'trig', sig:'Array* cosh_array(Array *arr)', desc:'Element-wise hyperbolic cosine, cosh(x).', params:[['arr','Array*','Input']], rel:['sinh_array','tanh_array','arccosh'] , example:'Array *ch = cosh_array(x);'},
tanh_array: { m:'trig', sig:'Array* tanh_array(Array *arr)', desc:'Element-wise hyperbolic tangent, tanh(x).', params:[['arr','Array*','Input']], rel:['sinh_array','cosh_array','arctanh'] , example:'Array *th = tanh_array(x);'},

arcsin: { m:'trig', sig:'Array* arcsin(Array *arr)', desc:'Inverse sine, asin(x). Domain [-1,1] → [-π/2, π/2].', params:[['arr','Array*','Input']], ret:'Integer→FLOAT64. Float→same float.', notes:'Values outside [-1,1] produce NaN.', example:`Array *a = arcsin(x);  // NaN for x outside [-1,1]`, rel:['arccos','arctan','sin_array'] },
arccos: { m:'trig', sig:'Array* arccos(Array *arr)', desc:'Inverse cosine, acos(x). Domain [-1,1] → [0, π].', notes:'Values outside [-1,1] produce NaN.', rel:['arcsin','arctan','cos_array'] , params:[['arr','Array*','Input (any numeric)']], example:'Array *ac = arccos(x);  // NaN for |x|>1'},
arctan: { m:'trig', sig:'Array* arctan(Array *arr)', desc:'Inverse tangent, atan(x). Range (-π/2, π/2).', rel:['arcsin','arccos','arctan2'] , params:[['arr','Array*','Input']], example:'Array *at = arctan(x);'},
arctan2: { m:'trig', sig:'Array* arctan2(Array *y, Array *x)', desc:'atan2(y, x) with quadrant handling. Like numpy.arctan2.', params:[['y','Array*','y-coordinates'],['x','Array*','x-coordinates']], ret:'FLOAT64 array of broadcast shape. Range (-π, π].', notes:'atan2(0,0) → 0 (matching IEEE). Handles x=0 correctly for quadrants.', example:`Array *theta = arctan2(y, x);  // polar angle from (0,0)`, rel:['arctan','angle'] },
arcsinh: { m:'trig', sig:'Array* arcsinh(Array *arr)', desc:'Inverse hyperbolic sine, asinh(x).', rel:['arccosh','arctanh','sinh_array'] , params:[['arr','Array*','Input']], example:'Array *ash = arcsinh(x);'},
arccosh: { m:'trig', sig:'Array* arccosh(Array *arr)', desc:'Inverse hyperbolic cosine, acosh(x). Domain [1, ∞).', notes:'x < 1 produces NaN.', rel:['arcsinh','arctanh','cosh_array'] , params:[['arr','Array*','Input']], example:'Array *ach = arccosh(x);  // NaN for x<1'},
arctanh: { m:'trig', sig:'Array* arctanh(Array *arr)', desc:'Inverse hyperbolic tangent, atanh(x). Domain (-1, 1).', notes:'|x| ≥ 1 produces NaN.', rel:['arcsinh','arccosh','tanh_array'] , params:[['arr','Array*','Input']], example:'Array *ath = arctanh(x);  // NaN for |x|>=1'},
deg2rad: { m:'trig', sig:'Array* deg2rad(Array *arr)', desc:'Convert degrees to radians: x * π / 180.', params:[['arr','Array*','Input (degrees)']], example:`Array *rad = deg2rad(deg);`, rel:['rad2deg','radians','degrees'] },
rad2deg: { m:'trig', sig:'Array* rad2deg(Array *arr)', desc:'Convert radians to degrees: x * 180 / π.', params:[['arr','Array*','Input (radians)']], rel:['deg2rad','degrees','radians'] , example:'Array *deg = rad2deg(rad);'},
degrees: { m:'trig', sig:'Array* degrees(Array *arr)', desc:'Alias for rad2deg. Convert radians to degrees.', params:[['arr','Array*','Input (radians)']], rel:['rad2deg','radians'] , example:'Array *deg = degrees(rad);'},
radians: { m:'trig', sig:'Array* radians(Array *arr)', desc:'Alias for deg2rad. Convert degrees to radians.', params:[['arr','Array*','Input (degrees)']], rel:['deg2rad','degrees'] , example:'Array *rad = radians(deg);'},
angle: { m:'trig', sig:'Array* angle(Array *z, int deg)', desc:'Phase angle (argument) of a complex number.', params:[['z','Array*','Complex array'],['deg','int','0→radians, 1→degrees']], ret:'FLOAT64 array, same shape. Range (-π,π] or (-180,180].', example:`Array *phase = angle(complexArr, 0);  // in radians`, rel:['arctan2','real_array','imag_array'] },

// ── Floating-Point Ops ──
fabs_array: { m:'fp', sig:'Array* fabs_array(Array *arr)', desc:'Absolute value for floating-point dtypes. Like numpy.fabs.', params:[['arr','Array*','Floating-point input']], ret:'Same dtype as input.', example:`Array *a = fabs_array(arr);`, rel:['abs_array','copysign_array'] },
copysign_array: { m:'fp', sig:'Array* copysign_array(Array *x1, Array *x2)', desc:'Copy sign of x2 to magnitude of x1.', params:[['x1','Array*','Magnitude source'],['x2','Array*','Sign source']], ret:'FLOAT64 array of broadcast shape.', example:`Array *r = copysign_array(a, b);  // |a| * sign(b)`, rel:['fabs_array','sign','nextafter_array'] },
nextafter_array: { m:'fp', sig:'Array* nextafter_array(Array *x1, Array *x2)', desc:'Next representable value after x1 in the direction of x2.', params:[['x1','Array*','Starting value'],['x2','Array*','Direction']], ret:'FLOAT64 array of broadcast shape.', example:`Array *nxt = nextafter_array(x, y);`, rel:['spacing_array'] },
spacing_array: { m:'fp', sig:'Array* spacing_array(Array *arr)', desc:'Distance to the nearest representable number. Like numpy.spacing.', params:[['arr','Array*','Floating-point input']], example:`Array *eps = spacing_array(arr);`, rel:['nextafter_array'] },
frexp_array: { m:'fp', sig:'int frexp_array(Array *arr, Array **mantissa, Array **exponent)', desc:'Decompose into mantissa (0.5 ≤ |m| < 1) and exponent (integer).', params:[['arr','Array*','Input'],['mantissa','Array**','Output mantissa (FLOAT64)'],['exponent','Array**','Output exponent (INT32)']], ret:'0 on success.', example:`Array *m=NULL, *e=NULL; frexp_array(arr, &m, &e);`, rel:['modf_array'] },
modf_array: { m:'fp', sig:'int modf_array(Array *arr, Array **frac, Array **integ)', desc:'Decompose into fractional and integral parts.', params:[['arr','Array*','Input'],['frac','Array**','Fractional part (FLOAT64)'],['integ','Array**','Integral part (FLOAT64)']], ret:'0 on success.', example:`Array *f=NULL,*i=NULL; modf_array(arr, &f, &i);`, rel:['frexp_array','floor_array','atrunc'] },

// ── Statistics ──
sum: { m:'stats', sig:'Array* sum(Array *arr, int axis, int keepdims)', desc:'Sum of array elements along a given axis.', params:[
  ['arr','Array*','Input array'],['axis','int','Axis to reduce (-1 = flatten all elements)'],['keepdims','int','If non-zero, reduced axis kept as size 1']
], ret:'Array with reduced axis. Dtype promoted: integers → INT64, float/complex keep their type.', example:`Array *t = sum(A, -1, 0);  // total sum (scalar)
Array *cs = sum(A, 0, 1);   // column sums, shape (1,N)`, rel:['prod','mean','cumsum'] },

prod: { m:'stats', sig:'Array* prod(Array *arr, int axis, int keepdims)', desc:'Product of array elements along a given axis.', params:[['arr','Array*','Input'],['axis','int','Axis (-1=all)'],['keepdims','int','Keep reduced dims']], ret:'Array with reduced axis. Integers→INT64.', example:`Array *p = prod(A, -1, 0);  // total product`, rel:['sum','cumprod'] },

mean: { m:'stats', sig:'Array* mean(Array *arr, int axis, int keepdims)', desc:'Arithmetic mean along a given axis. Result always FLOAT64.', params:[['arr','Array*','Input'],['axis','int','Axis (-1=flat)'],['keepdims','int','Keep dims']], ret:'FLOAT64 array.', notes:'Mean = sum / count. Integer inputs are converted to double before averaging.', example:`Array *m = mean(A, 0, 1);  // (1,N) row vector of column means
Array *avg = mean(A, -1, 0);  // scalar`, rel:['std','var','median','sum'] },

std: { m:'stats', sig:'Array* std(Array *arr, int axis, int keepdims)', desc:'Standard deviation (ddof=0) along a given axis.', params:[['arr','Array*','Input'],['axis','int','Axis (-1=flat)'],['keepdims','int','Keep dims']], ret:'FLOAT64 array.', notes:'Uses a two-pass algorithm: mean first, then squared deviations. For ddof>0 use var() with ddof parameter.', example:`Array *s = std(A, -1, 0);  // population std of all elements`, rel:['var','mean'] },

var: { m:'stats', sig:'Array* var(Array *a, int axis, int ddof, int keepdims)', desc:'Variance along a given axis.', params:[
  ['a','Array*','Input'],['axis','int','Axis (-1=flat)'],['ddof','int','Delta degrees of freedom: 0 = population variance (1/N), 1 = sample variance (1/(N-1))'],['keepdims','int','Keep reduced dims']
], ret:'FLOAT64 array.', example:`Array *pv = var(A, -1, 0, 0);  // population variance
Array *sv = var(A, -1, 1, 0);  // sample variance`, rel:['std','mean','cov'] },

median: { m:'stats', sig:'Array* median(Array *a, int axis, int keepdims)', desc:'Median along a given axis.', params:[['a','Array*','Input (real)'],['axis','int','Axis (-1=flat)'],['keepdims','int','Keep dims']], ret:'FLOAT64 array.', notes:'Sorts a copy of the data. For even-length axis, returns mean of two middle values.', example:`Array *m = median(A, -1, 0);`, rel:['mean','percentile','quantile'] },

percentile: { m:'stats', sig:'Array* percentile(Array *a, Array *q, int axis)', desc:'q-th percentile(s), q ∈ [0, 100]. Uses linear interpolation.', params:[
  ['a','Array*','Input (real)'],['q','Array*','1D FLOAT64 array of percentiles (0 to 100)'],['axis','int','Axis (-1=flat)']
], ret:'FLOAT64 array. If q is scalar → shape = a.shape without axis; if q is vector → first dim = len(q).', notes:'Interpolation: let N = axis_len, p = percentile/100. Index = (N-1)*p. If non-integer, linear interpolation between floor and ceil.', example:`Array *q = create_array((int[]){3},1,FLOAT64);
((double*)q->data)[0]=25; ((double*)q->data)[1]=50; ((double*)q->data)[2]=75;
Array *p = percentile(A, q, -1);  // [Q1, median, Q3]`, rel:['quantile','median','nanpercentile'] },

quantile: { m:'stats', sig:'Array* quantile(Array *a, Array *q, int axis)', desc:'q-th quantile(s), q ∈ [0, 1]. Same algorithm as percentile with q values 0–1.', params:[
  ['a','Array*','Input (real)'],['q','Array*','1D FLOAT64 array of quantiles (0 to 1)'],['axis','int','Axis (-1=flat)']
], example:`Array *q = create_array((int[]){2},1,FLOAT64);
((double*)q->data)[0]=0.25; ((double*)q->data)[1]=0.75;
Array *qu = quantile(A, q, -1);  // lower and upper quartiles`, rel:['percentile','median','nanquantile'] },

min: { m:'stats', sig:'Array* min(Array *arr, int axis, int keepdims)', desc:'Minimum along a given axis.', params:[['arr','Array*','Input'],['axis','int','Axis (-1=flat)'],['keepdims','int','Keep dims']], ret:'Array with reduced axis, same dtype.', example:`Array *mn = min(A, 0, 0);  // min of each column`, rel:['max','minimum','nanmin'] },

max: { m:'stats', sig:'Array* max(Array *arr, int axis, int keepdims)', desc:'Maximum along a given axis.', params:[['arr','Array*','Input'],['axis','int','Axis (-1=flat)'],['keepdims','int','Keep dims']], example:`Array *mx = max(A, -1, 0);  // global max`, rel:['min','maximum','nanmax'] },

ptp: { m:'stats', sig:'Array* ptp(Array *arr, int axis, int keepdims)', desc:'Peak-to-peak = max − min along axis.', params:[['arr','Array*','Input'],['axis','int','Axis'],['keepdims','int','Keep dims']], example:`Array *r = ptp(A, -1, 0);  // range`, rel:['max','min'] },

maximum: { m:'stats', sig:'Array* maximum(Array *x1, Array *x2)', desc:'Element-wise maximum of two arrays (with broadcasting).', params:[['x1','Array*','First'],['x2','Array*','Second']], ret:'Same dtype as inputs (promoted), broadcast shape.', example:`Array *m = maximum(a, b);  // max(a[i], b[i])`, rel:['minimum','max','clip'] },

minimum: { m:'stats', sig:'Array* minimum(Array *x1, Array *x2)', desc:'Element-wise minimum of two arrays (with broadcasting).', params:[['x1','Array*','First'],['x2','Array*','Second']], example:`Array *m = minimum(a, b);`, rel:['maximum','min','clip'] },

cumsum: { m:'stats', sig:'Array* cumsum(Array *arr, int axis)', desc:'Cumulative sum along a given axis.', params:[['arr','Array*','Input'],['axis','int','Axis (-1=flat first)']], ret:'Same shape as input.', example:`Array *cs = cumsum(A, 0);  // cumulative sum down rows`, rel:['cumprod','sum','nancumsum'] },

cumprod: { m:'stats', sig:'Array* cumprod(Array *arr, int axis)', desc:'Cumulative product along a given axis.', params:[['arr','Array*','Input'],['axis','int','Axis (-1=flat)']], example:`Array *cp = cumprod(A, -1);  // cumulative product flat`, rel:['cumsum','prod','nancumprod'] },

cov: { m:'stats', sig:'Array* cov(Array *m, int rowvar, int ddof, DataType dtype)', desc:'Estimate a covariance matrix.', params:[
  ['m','Array*','Input (1-D = single variable; 2-D = multiple variables)'],['rowvar','int','If non-zero, rows = variables; if 0, columns = variables'],['ddof','int','Delta degrees of freedom (0=biased, 1=unbiased)'],['dtype','DataType','Output data type']
], ret:'FLOAT64 covariance matrix (square, size = number of variables).', example:`Array *C = cov(data, 0, 0, FLOAT64);  // columns=variables, biased`, rel:['corrcoef','var'] },

corrcoef: { m:'stats', sig:'Array* corrcoef(Array *m, int rowvar)', desc:'Pearson product-moment correlation coefficients.', params:[['m','Array*','Input'],['rowvar','int','If 1, rows=variables; if 0, columns=variables']], ret:'FLOAT64 correlation matrix (1 on diagonal, −1..1 off-diagonal).', example:`Array *R = corrcoef(data, 0);`, rel:['cov'] },

average: { m:'stats', sig:'Array* average(Array *arr, Array *weights, int axis, int keepdims)', desc:'Compute the weighted average along a given axis.', params:[['arr','Array*','Input'],['weights','Array*','Weights (must match along axis; NULL = uniform)'],['axis','int','Axis (-1=flat)'],['keepdims','int','Keep dims']], ret:'FLOAT64 array.', example:`Array *w = full((int[]){n},1,FLOAT64,&(double){0.5});
Array *wa = average(A, w, 0, 0);  // weighted column average`, rel:['mean','sum'] },

diff: { m:'stats', sig:'Array* diff(Array *arr, int n, int axis)', desc:'n-th discrete difference along axis.', params:[['arr','Array*','Input'],['n','int','Order (1=first difference, 2=second, etc.)'],['axis','int','Axis']], ret:'Array with axis reduced by n.', example:`Array *d1 = diff(A, 1, 0);   // A[1:,:]-A[:-1,:]
Array *d2 = diff(A, 2, -1);  // second difference, flat`, rel:['cumsum','ediff1d'] },

trapz: { m:'stats', sig:'Array* trapz(Array *y, Array *x, double dx, int axis)', desc:'Integrate along axis using the trapezoidal rule.', params:[
  ['y','Array*','Integrand values'],['x','Array*','Sample positions (NULL = use uniform dx)'],['dx','double','Uniform sample spacing (used when x is NULL)'],['axis','int','Axis to integrate along']
], ret:'FLOAT64 array, shape = y.shape with axis removed.', example:`Array *area = trapz(y, NULL, 0.1, -1);  // uniform dx=0.1
Array *area2 = trapz(y, x, 0.0, 0);    // x gives variable spacing`, rel:['sum','diff','cumsum'] },

// ── NaN Stats ──
nanmax: { m:'nanstats', sig:'Array* nanmax(Array *a, int axis)', desc:'Maximum ignoring NaN values.', params:[['a','Array*','Input (real or complex)'],['axis','int','Axis (-1=flat)']], ret:'FLOAT64 array. All-NaN slices → NaN.', example:`Array *m = nanmax(A, -1);  // max ignoring NaN`, rel:['nanmin','max','nanmean'] },
nanmin: { m:'nanstats', sig:'Array* nanmin(Array *a, int axis)', desc:'Minimum ignoring NaN.', params:[['a','Array*','Input'],['axis','int','Axis (-1=flat)']], rel:['nanmax','min'] , example:'Array *mn = nanmin(A, -1);'},
nanmean: { m:'nanstats', sig:'Array* nanmean(Array *a, int axis)', desc:'Mean ignoring NaN. Sum over non-NaN count.', params:[['a','Array*','Input'],['axis','int','Axis (-1=flat)']], example:`Array *m = nanmean(A, -1);`, rel:['nanmedian','mean'] },
nanmedian: { m:'nanstats', sig:'Array* nanmedian(Array *a, int axis)', desc:'Median ignoring NaN. Strips NaN before sorting.', params:[['a','Array*','Input'],['axis','int','Axis (-1=flat)']], example:`Array *m = nanmedian(A, -1);`, rel:['nanmean','median'] },
nanpercentile: { m:'nanstats', sig:'Array* nanpercentile(Array *a, Array *q, int axis)', desc:'Percentile ignoring NaN.', params:[['a','Array*','Input'],['q','Array*','1D FLOAT64, 0–100'],['axis','int','Axis']], ret:'FLOAT64 array.', rel:['nanquantile','percentile'] , example:'Array *p = nanpercentile(A, q, -1);',},
nanquantile: { m:'nanstats', sig:'Array* nanquantile(Array *a, Array *q, int axis)', desc:'Quantile ignoring NaN.', params:[['a','Array*','Input'],['q','Array*','1D FLOAT64 (0-1)'],['axis','int','Axis']], rel:['nanpercentile','quantile'] , example:'Array *q = nanquantile(A, qvals, -1);'},
nancumsum: { m:'nanstats', sig:'Array* nancumsum(Array *a, int axis)', desc:'Cumulative sum, treating NaN as zero (skip).', params:[['a','Array*','Input'],['axis','int','Axis (-1=flat)']], ret:'Same shape as input.', example:`Array *cs = nancumsum(A, 0);`, rel:['nancumprod','cumsum'] },
nancumprod: { m:'nanstats', sig:'Array* nancumprod(Array *a, int axis)', desc:'Cumulative product, treating NaN as one (skip).', params:[['a','Array*','Input'],['axis','int','Axis (-1=flat)']], example:`Array *cp = nancumprod(A, -1);`, rel:['nancumsum','cumprod'] },

// ── Linear Algebra ──
matmul: { m:'linalg', sig:'Array* matmul(Array *arr1, Array *arr2)', desc:'Matrix product with full NumPy broadcasting semantics.', params:[
  ['arr1','Array*','First operand (any ndim)'],['arr2','Array*','Second operand (any ndim)']
], ret:'Array. Rules: both 1-D → scalar (dot product); 1-D+2-D → 1-D result; 2-D+2-D → 2-D; >2-D → batched matmul.', notes:'The last axis of arr1 is contracted with the second-to-last axis of arr2. Batch dimensions are broadcast.', example:`Array *C = matmul(A, B);          // 2-D matrix multiply
Array *v = matmul(M, vector);      // matrix × vector
Array *B = matmul(batch, weights); // batched matmul`, rel:['dot','vdot','inner','tensordot'] },

dot: { m:'linalg', sig:'Array* dot(Array *arr1, Array *arr2)', desc:'Dot product of two arrays. For 2-D = matmul; 1-D = inner product; N-D = sum product over last axis.', params:[['arr1','Array*','First'],['arr2','Array*','Second']], ret:'Scalars from 1-D dot; matrix from 2-D; tensor from N-D.', notes:'Difference from matmul: for >2-D, dot contracts last of a with 2nd-to-last of b (not batched).', example:`double *dp = dot(v, w)->data;  // scalar: Σ v[i]*w[i]`, rel:['matmul','vdot','inner'] },

vdot: { m:'linalg', sig:'Array* vdot(Array *arr1, Array *arr2)', desc:'Conjugate dot product. Flattens both inputs, then Σ conj(a[i]) * b[i].', params:[['arr1','Array*','First (flattened)'],['arr2','Array*','Second (flattened)']], ret:'Scalar (ndim=0) FLOAT64 or COMPLEX128.', notes:'For real inputs: same as dot after flattening. For complex: conjugate of first argument.', example:`double r = *(double*)vdot(x, y)->data;  // real dot
complex double c = *(complex double*)vdot(cx, cy)->data;  // conj(cx)·cy`, rel:['dot','inner'] },

inner: { m:'linalg', sig:'Array* inner(Array *arr1, Array *arr2)', desc:'Inner product — contracts the last dimension of both arrays.', params:[['arr1','Array*','First'],['arr2','Array*','Second']], example:`Array *ip = inner(a, b);`, rel:['dot','outer'] },

outer: { m:'linalg', sig:'Array* outer(Array *arr1, Array *arr2)', desc:'Outer product. out[i,j] = a[i] * b[j] after flattening both inputs.', params:[['arr1','Array*','First (flattened)'],['arr2','Array*','Second (flattened)']], ret:'2-D Array, shape (len(a), len(b)).', example:`Array *o = outer(u, v);  // u[i]*v[j] for all i,j`, rel:['inner','kron','tensordot'] },

cross: { m:'linalg', sig:'Array* cross(Array *arr1, Array *arr2)', desc:'Cross product of two 3-element vectors.', params:[['arr1','Array*','First (must be 1-D, size 3)'],['arr2','Array*','Second (must be 1-D, size 3)']], ret:'1-D Array of length 3. c[0]=a[1]*b[2]−a[2]*b[1], c[1]=a[2]*b[0]−a[0]*b[2], c[2]=a[0]*b[1]−a[1]*b[0].', example:`Array *c = cross(u, v);  // u × v`, rel:['dot','outer'] },

tensordot: { m:'linalg', sig:'Array* tensordot(Array *a, Array *b, Array *axes_a, Array *axes_b)', desc:'Tensor dot product — contract specified axes of two arrays.', params:[['a','Array*','First tensor'],['b','Array*','Second tensor'],['axes_a','Array*','1D INT64 axes of a to contract'],['axes_b','Array*','1D INT64 axes of b to contract (same length as axes_a)']], ret:'Contracted tensor.', example:`Array *td = tensordot(A, B, axes_a, axes_b);`, rel:['dot','matmul','kron'] },

svd: { m:'linalg', sig:'SVDResult svd(Array *a, int full_matrices)', desc:'Singular Value Decomposition. A = U @ diag(s) @ Vt.', params:[
  ['a','Array*','m×n real matrix (2-D, FLOAT64)'],['full_matrices','int','1 = full U (m×m); 0 = economy U (m×k where k=min(m,n))']
], ret:'SVDResult {Array *U, Array *s, Array *Vt}. s: 1-D singular values in descending order.', notes:'Pure-C Golub-Reinsch bidiagonalization + implicit-shift QR. U·diag(s)·Vt ≈ A. For large matrices compile with -DUSE_LAPACK.', example:`SVDResult r = svd(A, 1);
printf("Condition number: %.2e\\n", *(double*)r.s->data / *(double*)(r.s->data+(r.s->size-1)*sizeof(double)));
// Cleanup: free_array(r.U); free_array(r.s); free_array(r.Vt);`, rel:['qr','lu','eig','pinv','inv'] },

qr: { m:'linalg', sig:'QRResult qr(Array *arr)', desc:'QR decomposition via Householder reflections. A = Q·R.', params:[['arr','Array*','m×n matrix']], ret:'QRResult {Array *Q (m×n, orthogonal), Array *R (n×n, upper triangular)}.', notes:'Q^T·Q = I. R is n×n. Pure-C implementation.', example:`QRResult q = qr(A);
// Cleanup: free_array(q.Q); free_array(q.R);`, rel:['lu','svd'] },

lu: { m:'linalg', sig:'LUResult lu(Array *arr)', desc:'LU decomposition with partial pivoting. P·A = L·U.', params:[['arr','Array*','n×n square matrix']], ret:'LUResult {Array *P (permutation), Array *L (unit lower triangular), Array *U (upper triangular)}.', notes:'L has ones on diagonal. P is a row-permutation matrix. Pure-C partial (row) pivoting.', example:`LUResult lr = lu(A);
// P * A ≈ L * U
// Cleanup: free_array(lr.P); free_array(lr.L); free_array(lr.U);`, rel:['qr','svd','det','inv'] },

cholesky: { m:'linalg', sig:'Array* cholesky(Array *arr)', desc:'Cholesky decomposition. A = L·L^T where L is lower triangular.', params:[['arr','Array*','n×n symmetric positive-definite (SPD) matrix']], ret:'n×n lower triangular L.', notes:'Only the lower triangle of A is used. Does NOT verify SPD — caller must ensure. If A is not SPD, sqrt of negative number occurs.', example:`Array *L = cholesky(A);
Array *LT = transpose(L);
// A ≈ matmul(L, LT)`, rel:['lu','inv','svd'] },

inv: { m:'linalg', sig:'Array* inv(Array *arr)', desc:'Compute the (multiplicative) inverse of a square matrix via LU decomposition.', params:[['arr','Array*','n×n square matrix']], ret:'n×n inverse. Returns NULL if singular.', notes:'solve: LU factorization then n triangular solves for each column of identity. Returns NULL on singular detection.', example:`Array *Ai = inv(A);
Array *I = matmul(A, Ai);   // should ≈ identity
Array *x = matmul(Ai, b);   // solve A x = b`, rel:['pinv','det','lu'] },

pinv: { m:'linalg', sig:'Array* pinv(Array *a, double rcond)', desc:'Moore-Penrose pseudoinverse via SVD.', params:[
  ['a','Array*','m×n matrix (rectangular is fine)'],['rcond','double','Cutoff threshold: singular values < rcond * max_sv are truncated to zero']
], ret:'n×m pseudoinverse.', notes:'Works for rank-deficient and non-square matrices. Default rcond ≈ 1e-15.', example:`Array *Ap = pinv(A, 1e-12);
Array *x = matmul(Ap, b);   // least-squares solution`, rel:['inv','svd'] },

det: { m:'linalg', sig:'Array* det(Array *arr)', desc:'Matrix determinant via LU decomposition.', params:[['arr','Array*','n×n square matrix']], ret:'Scalar (ndim=0) FLOAT64 array.', notes:'det = product(U diagonal) * (-1)^(number of row swaps). Singular → 0.', example:`Array *d = det(A);
double detVal = *(double*)d->data;`, rel:['inv','lu','trace'] },

trace: { m:'linalg', sig:'Array* trace(Array *arr)', desc:'Sum of elements along the main diagonal.', params:[['arr','Array*','2-D array']], ret:'Scalar FLOAT64 array.', example:`Array *t = trace(A);`, rel:['det','diag'] },

conj_transpose: { m:'linalg', sig:'Array* conj_transpose(Array *arr)', desc:'Conjugate transpose — Hermitian for complex, regular transpose for real.', params:[['arr','Array*','Input (2-D)']], ret:'2-D array with axes swapped and imag sign flipped for complex.', example:`Array *H = conj_transpose(A);  // A^H = Ā^T`, rel:['transpose','real_array','imag_array'] },

tril: { m:'linalg', sig:'Array* tril(Array *m, int k)', desc:'Lower triangle of array. Elements above the k-th diagonal are zeroed.', params:[['m','Array*','Input'],['k','int','Diagonal offset (0=main, >0=more, <0=less)']], ret:'Array of same shape and dtype.', example:`Array *L = tril(A, 0);  // main diagonal and below`, rel:['triu','tril_indices','tril_indices_from'] },

triu: { m:'linalg', params:[['m','Array*','Input'],['k','int','Diagonal offset']], sig:'Array* triu(Array *m, int k)', desc:'Upper triangle of array. Elements below the k-th diagonal are zeroed.', example:`Array *U = triu(A, 0);`, rel:['tril','triu_indices'] },

tril_indices: { m:'linalg', sig:'Array* tril_indices(int n, int k, int m)', desc:'Return the indices for the lower triangle of an (n,m) array.', params:[['n','int','Rows'],['k','int','Diagonal offset'],['m','int','Columns']], ret:'2×N INT64 array (row indices in [0], col indices in [1]).', example:`Array *idx = tril_indices(3, 0, 3);`, rel:['tril','triu_indices'] },
triu_indices: { m:'linalg', params:[['n','int','Rows'],['k','int','Offset'],['m','int','Cols']], sig:'Array* triu_indices(int n, int k, int m)', desc:'Indices for upper triangle.', rel:['triu','tril_indices'] , example:'Array *idx = triu_indices(3, 0, 3);',},
tril_indices_from: { m:'linalg', params:[['arr','Array*','Existing array'],['k','int','Offset']], sig:'Array* tril_indices_from(Array *arr, int k)', desc:'Lower triangle indices from an existing array.', example:`Array *idx = tril_indices_from(A, 0);`, rel:['tril','tril_indices'] },
triu_indices_from: { m:'linalg', params:[['arr','Array*','Existing array'],['k','int','Offset']], sig:'Array* triu_indices_from(Array *arr, int k)', desc:'Upper triangle indices from array.', rel:['triu','tril_indices_from'] , example:'Array *idx = triu_indices_from(A, 0);',},

eig: { m:'linalg', sig:'EigResult eig(Array *a)', desc:'Compute eigenvalues and eigenvectors of a square matrix.', params:[['a','Array*','n×n matrix (real, FLOAT32/FLOAT64)']], ret:'EigResult {eigenvalues, eigenvectors}. eigenvalues: 1-D COMPLEX128; eigenvectors: n×n COMPLEX128.', notes:'Pure-C Jacobi for symmetric; link -llapack -lblas with -DUSE_LAPACK for general matrices.', example:`EigResult er = eig(A);
// er.eigenvalues[i] = λ_i; er.eigenvectors[:,i] = v_i`, rel:['svd','roots'] },

diagonal: { m:'linalg', sig:'Array* diagonal(Array *arr, int offset, int axis1, int axis2)', desc:'Return specified diagonals from an array.', params:[['arr','Array*','Input (≥2-D)'],['offset','int','Diagonal offset'],['axis1','int','First axis'],['axis2','int','Second axis']], ret:'1-D array of diagonal elements.', rel:['diag','trace'] , example:'Array *d = diagonal(A, 0, 0, 1);',},
diag_indices: { m:'linalg', sig:'Array** diag_indices(int n, int ndim, int *num_arrays)', desc:'Return indices to access the main diagonal of an n-D array.', params:[['n','int','Size'],['ndim','int','Number of dims'],['num_arrays','int*','Output: array count']], rel:['diag','diagonal'] , example:'int n; Array **idx = diag_indices(3, 2, &n);'},

// ── FFT ──
fft: { m:'fft', sig:'Array* fft(Array *arr)', desc:'1-D complex FFT (Cooley-Tukey radix-2 DIT).', params:[['arr','Array*','Input (any dtype; converted to COMPLEX128). Length must be power of 2']], ret:'1-D COMPLEX128 array, same length.', notes:'Requires length = power of 2. Real-only inputs: use rfft() for efficiency (half the computation).', example:`Array *spec = fft(signal);   // frequency spectrum
Array *recon = ifft(spec);   // exact round-trip
// recon[i] ≈ signal[i]`, rel:['ifft','rfft','fft2','fftn','fftfreq','fftshift'] },

ifft: { m:'fft', sig:'Array* ifft(Array *arr)', desc:'1-D inverse FFT. Divides by N (matching NumPy convention).', params:[['arr','Array*','COMPLEX128 spectrum']], ret:'1-D COMPLEX128 array, same length.', notes:'fft → ifft is exact round-trip (within fp error).', example:`Array *recon = ifft(spec);`, rel:['fft','irfft','ifft2','ifftn'] },

fft2: { m:'fft', sig:'Array* fft2(Array *arr)', desc:'2-D FFT — 1-D FFT along rows, then along columns.', params:[['arr','Array*','Input (converted to COMPLEX128)']], ret:'COMPLEX128 array, same shape.', example:`Array *spec2 = fft2(image);`, rel:['fft','ifft2','fftn','rfft2'] },
ifft2: { m:'fft', params:[['arr','Array*','COMPLEX128 spectrum']], sig:'Array* ifft2(Array *arr)', desc:'2-D inverse FFT.', rel:['fft2','ifft','ifftn'] , example:'Array *recon = ifft2(spectrum2D);',},

fftn: { m:'fft', sig:'Array* fftn(Array *arr)', desc:'N-D FFT — 1-D FFT sequentially along each axis.', params:[['arr','Array*','Input (converted to COMPLEX128)']], ret:'COMPLEX128 array, same shape.', example:`Array *specN = fftn(volume);`, rel:['fft','fft2','ifftn','rfftn'] },
ifftn: { m:'fft', params:[['arr','Array*','COMPLEX128 spectrum']], sig:'Array* ifftn(Array *arr)', desc:'N-D inverse FFT.', rel:['fftn','ifft','ifft2'] , example:'Array *recon = ifftn(spectrumND);',},

rfft: { m:'fft', sig:'Array* rfft(Array *arr)', desc:'Real 1-D FFT — returns only non-negative frequencies (half+1 values).', params:[['arr','Array*','Real input (any real dtype)']], ret:'1-D COMPLEX128, length = n/2+1 (n = input length, must be even).', notes:'Uses conjugate symmetry (X[-k]=conj(X[k])) to avoid computing redundant frequencies. ~2× faster than fft() for real data.', example:`Array *half = rfft(realData);  // n=8 → 5 complex values`, rel:['irfft','fft','rfft2','rfftn'] },

irfft: { m:'fft', sig:'Array* irfft(Array *arr, int n)', desc:'Inverse real FFT — from half-spectrum to real signal of length n.', params:[['arr','Array*','Half-spectrum (length n/2+1)'],['n','int','Desired output length (must be even)']], ret:'1-D FLOAT64, length n.', example:`Array *realSig = irfft(halfSpectrum, 8);`, rel:['rfft','irfft2','irfftn'] },

rfft2: { m:'fft', params:[['arr','Array*','Real input (2-D)']], sig:'Array* rfft2(Array *arr)', desc:'Real 2-D FFT.', rel:['rfft','irfft2','fft2','rfftn'] , example:'Array *half2 = rfft2(realImage);'},
irfft2: { m:'fft', params:[['arr','Array*','Half-spectrum'],['s','int[2]','Output shape {rows,cols}']], sig:'Array* irfft2(Array *arr, int s[2])', desc:'Inverse real 2-D FFT.', params:[['arr','Array*','Half-spectrum'],['s','int[2]','Output shape {rows, cols}']], rel:['rfft2','irfft','irfftn'] },
rfftn: { m:'fft', params:[['arr','Array*','Real input (N-D)']], sig:'Array* rfftn(Array *arr)', desc:'Real N-D FFT.', rel:['rfft','fftn','rfft2','irfftn'] , example:'Array *halfN = rfftn(realVolume);'},
irfftn: { m:'fft', params:[['arr','Array*','Half-spectrum'],['s','int*','Output shape array']], sig:'Array* irfftn(Array *arr, int *s)', desc:'Inverse real N-D FFT.', params:[['arr','Array*','Half-spectrum'],['s','int*','Array of output dimension sizes']], rel:['rfftn','irfft','irfft2'] , example:'Array *real = irfftn(half, (int[]){6,8,4});',},

hfft: { m:'fft', params:[['arr','Array*','Half Hermitian spectrum'],['n','int','Output length']], sig:'Array* hfft(Array *arr, int n)', desc:'Hermitian FFT — from half-spectrum to real signal.', params:[['arr','Array*','Half Hermitian spectrum'],['n','int','Output length']], rel:['ihfft','rfft','irfft'] , example:'Array *sig = hfft(half, 8);',},
ihfft: { m:'fft', params:[['arr','Array*','Real input'],['n','int','Output length']], sig:'Array* ihfft(Array *arr, int n)', desc:'Inverse Hermitian FFT — real signal to normalized half-spectrum.', rel:['hfft','rfft'] , example:'Array *spec = ihfft(signal, 8);',},

fftfreq: { m:'fft', sig:'Array* fftfreq(int n, double d)', desc:'FFT sample frequencies. freq[i] = i/(n*d) for i < n/2, negative for i ≥ n/2.', params:[['n','int','Window length'],['d','double','Sample spacing (default 1.0)']], ret:'1-D FLOAT64, length n.', example:`Array *f = fftfreq(1024, 1.0/44100); // frequencies for 44.1kHz`, rel:['rfftfreq','fftshift'] },
rfftfreq: { m:'fft', sig:'Array* rfftfreq(int n, double d)', desc:'Real FFT sample frequencies (non-negative only).', params:[['n','int','Window length'],['d','double','Sample spacing']], ret:'1-D FLOAT64, length n/2+1.', rel:['fftfreq'] , example:'Array *f = rfftfreq(1024, 0.001);',},
fftshift: { m:'fft', sig:'Array* fftshift(Array *arr)', desc:'Shift zero-frequency component to the center of the spectrum.', params:[['arr','Array*','Frequency-domain array']], ret:'Array of same shape.', notes:'Shifts each axis by n//2. Apply to fftfreq output to get centered frequencies.', example:`Array *centered = fftshift(fftfreq(8, 1));  // [-4,-3,-2,-1,0,1,2,3]`, rel:['ifftshift','fftfreq'] },
ifftshift: { m:'fft', sig:'Array* ifftshift(Array *arr)', desc:'Inverse of fftshift (shift by (n+1)//2 per axis).', params:[['arr','Array*','Array to unshift']], example:`Array *unshifted = ifftshift(centered);`, rel:['fftshift'] },

// ── Random ──
random_seed: { m:'random', sig:'void random_seed(uint64_t seed)', desc:'Seed the global xoshiro256** RNG using splitmix64.', params:[['seed','uint64_t','64-bit seed value']], notes:'State is NOT thread-safe. Seed reproducibly: same seed → same sequence.', example:`random_seed(42);`, rel:['random_uint64','random_double','random_sample'] },
random_uint64: { m:'random', sig:'uint64_t random_uint64(void)', desc:'Generate a single uniform uint64 using xoshiro256**.', params:[], ret:'uint64_t in [0, 2^64−1].', example:`uint64_t r = random_uint64();`, rel:['random_seed','random_double'] },
random_double: { m:'random', sig:'double random_double(void)', desc:'Generate a single uniform double in [0, 1).', params:[], ret:'double in [0.0, 1.0).', example:`double u = random_double();`, rel:['random_seed','random_uint64','random_sample'] },

random_sample: { m:'random', sig:'Array* random_sample(int *shape, int ndim)', desc:'Return random floats in [0, 1) of given shape. Like numpy.random.random_sample.', params:[['shape','int*','Output shape'],['ndim','int','Number of dimensions']], ret:'FLOAT64 array.', example:`Array *u = random_sample((int[]){1000}, 1);  // 1000 uniform samples`, rel:['random_rand','random_uniform','random_randn'] },
random_rand: { m:'random', sig:'Array* random_rand(int *shape, int ndim)', desc:'Convenience alias for random_sample. Like numpy.random.rand.', params:[['shape','int*','Output shape'],['ndim','int','Dims']], example:`Array *u = random_rand((int[]){3,4}, 2);`, rel:['random_sample'] },
random_randn: { m:'random', sig:'Array* random_randn(int *shape, int ndim)', desc:'Standard normal N(0,1). Box-Muller transform.', params:[['shape','int*','Output shape'],['ndim','int','Dimensions']], ret:'FLOAT64 array.', example:`Array *n = random_randn((int[]){1000}, 1);`, rel:['random_normal','random_sample'] },
random_randint: { m:'random', sig:'Array* random_randint(int low, int high, int *shape, int ndim, DataType dtype)', desc:'Uniform random integers in [low, high).', params:[['low','int','Lower bound (inclusive)'],['high','int','Upper bound (exclusive)'],['shape','int*','Output shape'],['ndim','int','Dimensions'],['dtype','DataType','INT32, INT64, etc.']], ret:'Integer array of requested dtype.', example:`Array *ri = random_randint(0, 10, (int[]){5,5}, 2, INT32);  // 5×5 rand ints 0..9`, rel:['random_uniform','random_sample'] },
random_uniform: { m:'random', sig:'Array* random_uniform(double low, double high, int *shape, int ndim)', desc:'Uniform distribution in [low, high).', params:[['low','double','Lower bound'],['high','double','Upper bound'],['shape','int*','Output shape'],['ndim','int','Dimensions']], ret:'FLOAT64 array.', example:`Array *u = random_uniform(-1.0, 1.0, (int[]){500}, 1);`, rel:['random_sample','random_normal'] },
random_normal: { m:'random', sig:'Array* random_normal(double loc, double scale, int *shape, int ndim)', desc:'Normal N(loc, scale²) distribution. Box-Muller transform.', params:[['loc','double','Mean (μ)'],['scale','double','Standard deviation (σ, must be > 0)'],['shape','int*','Output shape'],['ndim','int','Dimensions']], ret:'FLOAT64 array.', example:`Array *n = random_normal(5.0, 2.0, (int[]){1000}, 1);  // μ=5, σ=2`, rel:['random_standard_normal','random_randn','random_uniform'] },
random_standard_normal: { m:'random', sig:'Array* random_standard_normal(int *shape, int ndim)', desc:'Standard normal N(0,1). Internal helper used by random_randn.', params:[['shape','int*','Output shape'],['ndim','int','Dims']], rel:['random_randn','random_normal'] , example:'Array *n = random_standard_normal((int[]){100},1);'},
random_exponential: { m:'random', sig:'Array* random_exponential(double scale, int *shape, int ndim)', desc:'Exponential distribution. f(x) = (1/scale)*exp(−x/scale).', params:[['scale','double','Scale parameter (β, > 0); also = 1/λ'],['shape','int*','Output shape'],['ndim','int','Dimensions']], ret:'FLOAT64 array.', example:`Array *e = random_exponential(1.5, (int[]){100}, 1);  // mean=1.5`, rel:['random_standard_exponential','random_gamma'] },
random_standard_exponential: { m:'random', sig:'Array* random_standard_exponential(int *shape, int ndim)', desc:'Standard exponential (scale=1). Internal helper.', params:[['shape','int*','Output shape'],['ndim','int','Dims']], rel:['random_exponential'] , example:'Array *e = random_standard_exponential((int[]){100},1);'},
random_standard_gamma: { m:'random', sig:'Array* random_standard_gamma(double shape, int *shape, int ndim)', desc:'Gamma(shape, 1). Internal helper used by random_gamma.', params:[['shape','double','Shape (k>0)'],['oshape','int*','Output shape'],['ndim','int','Dims']], rel:['random_gamma'] , example:'Array *g = random_standard_gamma(2.0, (int[]){100},1);'},
random_gamma: { m:'random', sig:'Array* random_gamma(double shape, double scale, int *oshape, int ndim)', desc:'Gamma distribution. Marsaglia-Tsang for shape≥1, Johnk for shape<1.', params:[['shape','double','Shape parameter (k, >0)'],['scale','double','Scale parameter (θ, >0); mean = k*θ'],['oshape','int*','Output shape'],['ndim','int','Dimensions']], ret:'FLOAT64 array.', notes:'Marsaglia-Tsang (2000) for k≥1: fast acceptance-rejection. Johnk generator for k<1.', example:`Array *g = random_gamma(2.0, 3.0, (int[]){500}, 1);  // mean=6.0`, rel:['random_beta','random_chisquare','random_exponential'] },
random_beta: { m:'random', sig:'Array* random_beta(double a, double b, int *shape, int ndim)', desc:'Beta distribution. X = G1/(G1+G2) where G1~Gamma(a), G2~Gamma(b).', params:[['a','double','Shape 1 (α, >0)'],['b','double','Shape 2 (β, >0)'],['shape','int*','Output shape'],['ndim','int','Dimensions']], ret:'FLOAT64 array in [0, 1].', example:`Array *b = random_beta(2.0, 5.0, (int[]){1000}, 1);`, rel:['random_gamma','random_dirichlet'] },
random_chisquare: { m:'random', sig:'Array* random_chisquare(double df, int *shape, int ndim)', desc:'χ² distribution. χ²(k) = Gamma(k/2, 2).', params:[['df','double','Degrees of freedom (k, >0)'],['shape','int*','Output shape'],['ndim','int','Dimensions']], ret:'FLOAT64 array.', example:`Array *cs = random_chisquare(5.0, (int[]){500}, 1);`, rel:['random_gamma','random_f'] },
random_standard_t: { m:'random', sig:'Array* random_standard_t(double df, int *shape, int ndim)', desc:'Student t distribution. t = Z / sqrt(χ²/df).', params:[['df','double','Degrees of freedom (>0)'],['shape','int*','Output shape'],['ndim','int','Dimensions']], ret:'FLOAT64 array.', example:`Array *t = random_standard_t(10.0, (int[]){500}, 1);`, rel:['random_normal','random_chisquare'] },
random_f: { m:'random', sig:'Array* random_f(double dfn, double dfd, int *shape, int ndim)', desc:'F distribution. F = (χ²₁/dfn) / (χ²₂/dfd).', params:[['dfn','double','Numerator df'],['dfd','double','Denominator df'],['shape','int*','Output shape'],['ndim','int','Dimensions']], ret:'FLOAT64 array.', example:`Array *f = random_f(5.0, 10.0, (int[]){500}, 1);`, rel:['random_chisquare'] },
random_laplace: { m:'random', sig:'Array* random_laplace(double loc, double scale, int *shape, int ndim)', desc:'Laplace (double exponential). PDF: 1/(2σ)*exp(−|x−μ|/σ).', params:[['loc','double','Location (μ)'],['scale','double','Scale (σ, >0)'],['shape','int*','Output shape'],['ndim','int','Dimensions']], ret:'FLOAT64 array.', example:`Array *l = random_laplace(0.0, 1.0, (int[]){500}, 1);`, rel:['random_normal','random_logistic'] },
random_logistic: { m:'random', sig:'Array* random_logistic(double loc, double scale, int *shape, int ndim)', desc:'Logistic distribution. Inverse CDF: μ + s*log(U/(1−U)).', params:[['loc','double','Location'],['scale','double','Scale (>0)'],['shape','int*','Output shape'],['ndim','int','Dimensions']], ret:'FLOAT64 array.', example:`Array *lg = random_logistic(0.0, 1.0, (int[]){500}, 1);`, rel:['random_laplace','random_normal'] },
random_lognormal: { m:'random', sig:'Array* random_lognormal(double mean, double sigma, int *shape, int ndim)', desc:'Log-normal distribution. exp(N(mean, sigma²)).', params:[['mean','double','Mean of underlying normal'],['sigma','double','Std of underlying normal (>0)'],['shape','int*','Output shape'],['ndim','int','Dimensions']], ret:'FLOAT64 array.', example:`Array *ln = random_lognormal(0.0, 1.0, (int[]){500}, 1);`, rel:['random_normal'] },
random_pareto: { m:'random', sig:'Array* random_pareto(double a, int *shape, int ndim)', desc:'Pareto (Lomax) distribution. PDF: a/(1+x)^(a+1). Inverse CDF: (1/U)^(1/a) − 1.', params:[['a','double','Shape (>0)'],['shape','int*','Output shape'],['ndim','int','Dimensions']], ret:'FLOAT64 array.', example:`Array *p = random_pareto(2.0, (int[]){500}, 1);`, rel:['random_exponential'] },
random_power: { m:'random', sig:'Array* random_power(double a, int *shape, int ndim)', desc:'Power distribution. PDF: a*x^(a−1) on [0,1].', params:[['a','double','Shape (>0)'],['shape','int*','Output shape'],['ndim','int','Dimensions']], ret:'FLOAT64 array in [0,1].', example:`Array *pw = random_power(2.0, (int[]){500}, 1);`, rel:['random_beta'] },
random_rayleigh: { m:'random', sig:'Array* random_rayleigh(double scale, int *shape, int ndim)', desc:'Rayleigh distribution. σ*sqrt(−2*ln(U)).', params:[['scale','double','Scale (σ, >0); mode = σ'],['shape','int*','Output shape'],['ndim','int','Dimensions']], ret:'FLOAT64 array.', example:`Array *r = random_rayleigh(1.0, (int[]){500}, 1);`, rel:['random_weibull','random_exponential'] },
random_weibull: { m:'random', sig:'Array* random_weibull(double a, int *shape, int ndim)', desc:'Weibull distribution. (−ln(U))^(1/k).', params:[['a','double','Shape (k, >0); k=1 → exponential, k=2 → Rayleigh'],['shape','int*','Output shape'],['ndim','int','Dimensions']], ret:'FLOAT64 array.', example:`Array *w = random_weibull(2.0, (int[]){500}, 1);  // Rayleigh-like`, rel:['random_rayleigh','random_exponential'] },
random_standard_cauchy: { m:'random', sig:'Array* random_standard_cauchy(int *shape, int ndim)', desc:'Standard Cauchy. tan(π*(U−0.5)). Heavy-tailed, no finite moments.', params:[['shape','int*','Output shape'],['ndim','int','Dimensions']], ret:'FLOAT64 array.', notes:'No mean, no variance — distribution is pathological. Use with caution.', example:`Array *c = random_standard_cauchy((int[]){500}, 1);`, rel:['random_standard_t'] },
random_gumbel: { m:'random', sig:'Array* random_gumbel(double loc, double scale, int *shape, int ndim)', desc:'Gumbel (extreme value type I). μ − σ*ln(−ln(U)).', params:[['loc','double','Location (μ)'],['scale','double','Scale (σ)'],['shape','int*','Output shape'],['ndim','int','Dimensions']], ret:'FLOAT64 array.', example:`Array *g = random_gumbel(0.0, 1.0, (int[]){500}, 1);`, rel:['random_laplace'] },
random_triangular: { m:'random', sig:'Array* random_triangular(double left, double mode, double right, int *shape, int ndim)', desc:'Triangular distribution on [left, right] with peak at mode.', params:[['left','double','Lower bound'],['mode','double','Peak location (left ≤ mode ≤ right)'],['right','double','Upper bound'],['shape','int*','Output shape'],['ndim','int','Dimensions']], ret:'FLOAT64 array.', example:`Array *t = random_triangular(0.0, 0.5, 1.0, (int[]){500}, 1);`, rel:['random_uniform','random_beta'] },
random_vonmises: { m:'random', sig:'Array* random_vonmises(double mu, double kappa, int *shape, int ndim)', desc:'Von Mises (circular normal) distribution. Best-Fisher acceptance-rejection.', params:[['mu','double','Mean direction (radians, −π..π)'],['kappa','double','Concentration (≥0; 0=uniform on circle)'],['shape','int*','Output shape'],['ndim','int','Dimensions']], ret:'FLOAT64 array in [-π, π].', example:`Array *vm = random_vonmises(0.0, 2.0, (int[]){500}, 1);`, rel:['random_normal'] },
random_wald: { m:'random', sig:'Array* random_wald(double mean, double scale, int *shape, int ndim)', desc:'Wald (inverse Gaussian) distribution. Uses χ²₁-based transform.', params:[['mean','double','Mean (μ, >0)'],['scale','double','Scale (λ, >0)'],['shape','int*','Output shape'],['ndim','int','Dimensions']], ret:'FLOAT64 array.', example:`Array *wa = random_wald(1.0, 1.0, (int[]){500}, 1);`, rel:['random_normal','random_chisquare'] },
random_binomial: { m:'random', sig:'Array* random_binomial(int n, double p, int *shape, int ndim)', desc:'Binomial(n, p). Sum of n independent Bernoulli trials.', params:[['n','int','Number of trials'],['p','double','Success probability (0..1)'],['shape','int*','Output shape'],['ndim','int','Dimensions']], ret:'INT64 array.', notes:'For n*p*(1-p) > 30, uses normal approximation for efficiency.', example:`Array *b = random_binomial(100, 0.3, (int[]){500}, 1);  // coin flips`, rel:['random_negative_binomial','random_poisson','random_geometric'] },
random_poisson: { m:'random', sig:'Array* random_poisson(double lam, int *shape, int ndim)', desc:'Poisson(λ). Knuth\'s algorithm for λ<30; normal approximation for large λ.', params:[['lam','double','Rate (λ, >0) — mean = variance = λ'],['shape','int*','Output shape'],['ndim','int','Dimensions']], ret:'INT64 array.', example:`Array *p = random_poisson(5.0, (int[]){500}, 1);  // arrivals/interval`, rel:['random_binomial','random_exponential'] },
random_geometric: { m:'random', sig:'Array* random_geometric(double p, int *shape, int ndim)', desc:'Geometric(p). Number of trials until first success.', params:[['p','double','Success probability (0<p≤1)'],['shape','int*','Output shape'],['ndim','int','Dimensions']], ret:'INT64 array.', example:`Array *g = random_geometric(0.5, (int[]){500}, 1);`, rel:['random_binomial','random_negative_binomial'] },
random_negative_binomial: { m:'random', sig:'Array* random_negative_binomial(int n, double p, int *shape, int ndim)', desc:'Negative binomial. Gamma-Poisson mixture.', params:[['n','int','Target successes'],['p','double','Success probability'],['shape','int*','Output shape'],['ndim','int','Dimensions']], ret:'INT64 array.', example:`Array *nb = random_negative_binomial(5, 0.5, (int[]){500}, 1);`, rel:['random_binomial','random_gamma','random_poisson'] },
random_hypergeometric: { m:'random', sig:'Array* random_hypergeometric(int ngood, int nbad, int nsample, int *shape, int ndim)', desc:'Hypergeometric — sampling without replacement from a finite population.', params:[['ngood','int','Number of good items in population'],['nbad','int','Number of bad items in population'],['nsample','int','Number of draws without replacement'],['shape','int*','Output shape'],['ndim','int','Dimensions']], ret:'INT64 array.', example:`Array *hg = random_hypergeometric(50, 50, 20, (int[]){500}, 1);`, rel:['random_binomial'] },
random_zipf: { m:'random', sig:'Array* random_zipf(double a, int *shape, int ndim)', desc:'Zipf distribution. P(X=k) ∝ 1/k^a.', params:[['a','double','Exponent (>1)'],['shape','int*','Output shape'],['ndim','int','Dimensions']], ret:'INT64 array.', example:`Array *z = random_zipf(2.0, (int[]){500}, 1);`, rel:['random_pareto'] },
random_logseries: { m:'random', sig:'Array* random_logseries(double p, int *shape, int ndim)', desc:'Logarithmic series distribution.', params:[['p','double','Parameter (0<p<1)'],['shape','int*','Output shape'],['ndim','int','Dimensions']], ret:'INT64 array.', example:`Array *ls = random_logseries(0.5, (int[]){500}, 1);`, rel:['random_geometric'] },
random_permutation: { m:'random', sig:'Array* random_permutation(Array *x)', desc:'Randomly permute a sequence (returns new copy). Fisher-Yates shuffle.', params:[['x','Array*','Input (any dtype)']], ret:'New Array with shuffled elements, same shape and dtype.', notes:'O(n) Fisher-Yates (Knuth shuffle). Does not modify x.', example:`Array *shuffled = random_permutation(arr);`, rel:['random_shuffle','random_choice'] },
random_shuffle: { m:'random', sig:'void random_shuffle(Array *x)', desc:'Randomly permute x in place. Fisher-Yates shuffle.', params:[['x','Array*','Array to shuffle (modified in place)']], notes:'O(n). Modifies x directly — no copy made.', example:`random_shuffle(arr);  // arr is now shuffled`, rel:['random_permutation'] },
random_choice: { m:'random', sig:'Array* random_choice(Array *a, int size, int replace, Array *p)', desc:'Random sampling from a population.', params:[['a','Array*','Population (any shape, flattened)'],['size','int','Number of samples to draw'],['replace','int','1=with replacement (bootstrap), 0=without'],['p','Array*','Probability weights (NULL=uniform)']], ret:'FLOAT64 array of length size.', notes:'p must sum to 1 (if provided) and have same length as flattened a.', example:`Array *samples = random_choice(pop, 10, 0, NULL);  // 10 unique uniform draws
Array *weights = random_dirichlet(ones(…), NULL, 1); // dirichlet weights
Array *wtd = random_choice(pop, 100, 1, weights);    // weighted, with replacement`, rel:['random_permutation','random_shuffle','random_multinomial'] },
random_multinomial: { m:'random', sig:'Array* random_multinomial(int n, Array *pvals, int *shape, int ndim)', desc:'Multinomial distribution. n draws from K categories with probabilities pvals.', params:[['n','int','Number of trials'],['pvals','Array*','1-D probability vector (length K, must sum to 1)'],['shape','int*','Batch shape (prefix dimensions)'],['ndim','int','Number of batch dimensions']], ret:'Array with shape = shape + [K]. Each row sums to n.', example:`Array *p = random_dirichlet(ones((int[]){3},1,FLOAT64), NULL, 1);
Array *m = random_multinomial(100, p, (int[]){1}, 1);  // 1×3`, rel:['random_choice','random_dirichlet'] },
random_multivariate_normal: { m:'random', sig:'Array* random_multivariate_normal(Array *mean, Array *cov, int *shape, int ndim)', desc:'Multivariate normal via Cholesky. μ + L·z where cov = L·L^T, z~N(0,I).', params:[['mean','Array*','1-D mean vector (μ, length K)'],['cov','Array*','K×K covariance matrix (Σ, must be SPD)'],['shape','int*','Batch shape'],['ndim','int','Number of batch dims']], ret:'Array, last dim=K (length of mean).', notes:'Uses cholesky(cov) to get L, then transforms standard normals. Returns NULL if cov is not SPD.', example:`Array *mu = arange(0, 3, 1, FLOAT64);
Array *Sigma = eye(3, 3, 0, FLOAT64);
Array *mvn = random_multivariate_normal(mu, Sigma, (int[]){500}, 1);  // 500×3`, rel:['random_normal','random_dirichlet','cholesky'] },
random_dirichlet: { m:'random', sig:'Array* random_dirichlet(Array *alpha, int *shape, int ndim)', desc:'Dirichlet distribution via gamma normalization. x_i = g_i / Σgⱼ where g_j~Gamma(αⱼ,1).', params:[['alpha','Array*','1-D concentration parameters (αⱼ > 0)'],['shape','int*','Batch shape'],['ndim','int','Number of batch dims']], ret:'Array, last dim=len(alpha). Each row sums to 1 (simplex).', example:`Array *alpha = ones((int[]){4}, 1, FLOAT64);
Array *d = random_dirichlet(alpha, (int[]){100}, 1);  // 100×4, each row sums to 1`, rel:['random_gamma','random_multinomial'] },
random_bytes: { m:'random', sig:'Array* random_bytes(int n)', desc:'Generate n random bytes from PRNG.', params:[['n','int','Number of bytes to generate']], ret:'Array of n bytes (dtype=UINT8).', notes:'Each byte is extracted from random_uint64() output.', example:`Array *rb = random_bytes(32);  // 32 random bytes`, rel:['random_seed','random_uint64'] },

// ── Polynomials ──
poly: { m:'poly', sig:'Array* poly(Array *roots)', desc:'Find polynomial coefficients from roots. p[0]=1, p[i] are coefficients.', params:[['roots','Array*','1-D array of roots (complex or real)']], ret:'1-D FLOAT64 array of length = len(roots)+1.', notes:'p(x) = (x−r₁)(x−r₂)⋯(x−rₙ) = x^n + p[1]x^(n−1) + … + p[n]. Leading coefficient always 1.0.', example:`Array *rts = create_array((int[]){2},1,COMPLEX128);
*(complex double*)rts->data = 2+0*I; *((complex double*)rts->data+1) = 3+0*I;
Array *p = poly(rts);  // [1, -5, 6] → x²-5x+6`, rel:['roots','polyval','polymul'] },

roots: { m:'poly', sig:'Array* roots(Array *p)', desc:'Compute polynomial roots via companion matrix + Francis double-shift QR.', params:[['p','Array*','1-D coefficients in descending powers (p[0]*x^n + … + p[n])']], ret:'1-D COMPLEX128 array of length = degree.', notes:'Builds Hessenberg companion matrix, then uses built-in Francis implicit QR to find all eigenvalues (roots). Leading zeros are stripped. Works for real, complex, and multiple roots.', example:`Array *coeff = create_array((int[]){3},1,FLOAT64);
double c[] = {1,-5,6}; memcpy(coeff->data,c,3*sizeof(double));
Array *r = roots(coeff);  // → [3+0i, 2+0i]`, rel:['poly','polyval','eig'] },

polyadd: { m:'poly', sig:'Array* polyadd(Array *a, Array *b)', desc:'Add two polynomials (sum coefficient arrays, padding shorter with zeros).', params:[['a','Array*','First polynomial coeffs'],['b','Array*','Second polynomial coeffs']], ret:'1-D FLOAT64 array, length = max(len(a), len(b)).', example:`Array *sum = polyadd(p1, p2);`, rel:['polysub','polymul'] },
polysub: { m:'poly', params:[['a','Array*','First'],['b','Array*','Second']], sig:'Array* polysub(Array *a, Array *b)', desc:'Subtract two polynomials.', example:`Array *diff = polysub(p1, p2);`, rel:['polyadd'] },
polymul: { m:'poly', sig:'Array* polymul(Array *a, Array *b)', desc:'Multiply polynomials (convolution of coefficient arrays).', params:[['a','Array*','First polynomial'],['b','Array*','Second polynomial']], ret:'1-D FLOAT64 array, length = len(a)+len(b)−1.', notes:'Equivalent to convolve(a,b). For two polynomials of degree n and m, result degree = n+m.', example:`Array *prod = polymul(p1, p2);`, rel:['polydiv','convolve','polyadd'] },
polydiv: { m:'poly', sig:'Array* polydiv(Array *u, Array *v, Array **r)', desc:'Divide polynomial u by v. Returns quotient; r gets remainder.', params:[['u','Array*','Dividend coefficients'],['v','Array*','Divisor coefficients'],['r','Array**','Output: remainder (set to NULL if exact division)']], ret:'1-D FLOAT64 quotient coefficients.', example:`Array *rem = NULL;
Array *quot = polydiv(num, den, &rem);`, rel:['polymul','polysub'] },
polyder: { m:'poly', sig:'Array* polyder(Array *p, int m)', desc:'m-th derivative of polynomial p.', params:[['p','Array*','Coefficients (descending powers)'],['m','int','Order of derivative (1=first, 2=second, etc.)']], ret:'1-D FLOAT64 array, length = len(p)−m (or empty if m ≥ len(p)).', example:`Array *dp = polyder(p, 1);  // first derivative
Array *d2p = polyder(p, 2); // second derivative`, rel:['polyint','diff'] },
polyint: { m:'poly', sig:'Array* polyint(Array *p, int m)', desc:'m-th antiderivative (integral) of polynomial p. Integration constant = 0.', params:[['p','Array*','Coefficients'],['m','int','Order of integration']], ret:'1-D FLOAT64 array, length = len(p)+m.', example:`Array *ip = polyint(p, 1);  // indefinite integral`, rel:['polyder'] },
polyfit: { m:'poly', sig:'Array* polyfit(Array *x, Array *y, int deg)', desc:'Least-squares polynomial fit. Minimizes ||V·c − y||².', params:[['x','Array*','x-coordinates (same length as y)'],['y','Array*','y-coordinates'],['deg','int','Polynomial degree (must be < len(x))']], ret:'1-D FLOAT64 array of length deg+1 (descending powers).', notes:'Uses normal equations: V^T·V·c = V^T·y. For high-degree or ill-conditioned fits, consider orthogonal polynomials.', example:`Array *c = polyfit(x, y, 2);  // quadratic fit: c[0]*x² + c[1]*x + c[2]
Array *yhat = polyval(c, x); // evaluate fit`, rel:['polyval','roots','vander'] },
polyval: { m:'poly', sig:'Array* polyval(Array *p, Array *x)', desc:'Evaluate polynomial at points x using Horner\'s method.', params:[['p','Array*','Coefficients (descending powers)'],['x','Array*','Evaluation points (any shape)']], ret:'FLOAT64 array, same shape as x.', notes:'Horner: p(x) = ((p[0]*x + p[1])*x + …) + p[n]. O(deg * len(x)).', example:`Array *c = polyfit(x, y, 3);
Array *yhat = polyval(c, xnew);  // evaluate fitted curve`, rel:['polyfit','poly','roots'] },

// ── Window Functions ──
hamming: { m:'window', sig:'Array* hamming(int m)', desc:'Hamming window: w[n] = 0.54 − 0.46·cos(2πn/(M−1)).', params:[['m','int','Window length (≥ 1)']], ret:'1-D FLOAT64 array of length m.', notes:'Side-lobe level: −42.7 dB. Common in speech processing. Not zero at endpoints.', example:`Array *w = hamming(1024);
// Apply: for(i=0;i<signal->size;i++) wdata[i] *= ((double*)w->data)[i];`, rel:['hanning','blackman','bartlett','kaiser'] },
hanning: { m:'window', sig:'Array* hanning(int m)', desc:'Hann window: w[n] = 0.5 − 0.5·cos(2πn/(M−1)).', params:[['m','int','Window length (≥ 1)']], ret:'1-D FLOAT64 array of length m.', notes:'Also called "Hann window" (after Julius von Hann, not "Hanning"). Side-lobe: −31.5 dB. Zero at endpoints. Excellent for spectral analysis.', example:`Array *w = hanning(512);
// Overlap-add spectral analysis`, rel:['hamming','blackman','kaiser','bartlett'] },
bartlett: { m:'window', sig:'Array* bartlett(int M)', desc:'Bartlett (triangular) window. w[n] = 1 − |2n/(M−1) − 1|.', params:[['M','int','Window length']], ret:'1-D FLOAT64 array.', notes:'Simplest window function. Side-lobe level: −26 dB. Zero at endpoints.', rel:['blackman','hamming'] , example:'Array *w = bartlett(512);',},
blackman: { m:'window', sig:'Array* blackman(int M)', desc:'Blackman window. w[n] = 0.42 − 0.5·cos(2πn/(M−1)) + 0.08·cos(4πn/(M−1)).', params:[['M','int','Window length']], ret:'1-D FLOAT64 array.', notes:'Best side-lobe suppression of classical windows: −58.1 dB.', example:`Array *w = blackman(256);`, rel:['hamming','hanning','kaiser'] },
kaiser: { m:'window', sig:'Array* kaiser(int M, double beta)', desc:'Kaiser window. β=0→rectangular; β≈5.44→~Hamming; β≈8.6→~Blackman.', params:[['M','int','Window length'],['beta','double','Shape parameter (≥ 0). Larger β = narrower main lobe, lower side-lobes']], ret:'1-D FLOAT64 array.', notes:'Near-optimal for given side-lobe level. Uses I₀ Bessel function approximation. β=0 → rectangular. β=5.44 → −42.7 dB. β=8.6 → −65 dB.', example:`Array *w = kaiser(256, 6.0);  // −55dB side-lobe`, rel:['blackman','hamming','i0'] },

// ── Interpolation ──
interp: { m:'interp', sig:'Array* interp(Array *x, Array *xp, Array *fp, Array *left, Array *right)', desc:'1-D linear interpolation. xp must be monotonically increasing.', params:[
  ['x','Array*','Evaluation points (any shape, any real dtype)'],['xp','Array*','1-D known x-coordinates (must be strictly increasing)'],['fp','Array*','1-D known y-coordinates (same length as xp)'],['left','Array*','Fill value for x < xp[0] — if NULL, uses fp[0]'],['right','Array*','Fill value for x > xp[-1] — if NULL, uses fp[-1]']
], ret:'FLOAT64 array, same shape as x.', notes:'Uses find_interval() binary search, O(log n) per evaluation point. xp must be sorted — NO validation is performed. Returns fp[0]/fp[-1] for out-of-bounds when left/right are NULL.', example:`// xp = [1,2,3,4,5], fp = [2,4,6,8,10]
Array *result = interp(xnew, xp, fp, NULL, NULL);
// xnew=1.5 → 3.0; xnew=0 → 2.0 (flat extrapolation)
double lv=0.0, rv=12.0;
Array *l = create_array((int[]){1},1,FLOAT64); *(double*)l->data=lv;
Array *r = create_array((int[]){1},1,FLOAT64); *(double*)r->data=rv;
Array *r2 = interp(xnew, xp, fp, l, r);  // custom extrapolation`, rel:['find_interval','polyval','searchsorted'] },

// ── Signal Processing ──
convolve: { m:'signal', sig:'Array* convolve(Array *a, Array *v)', desc:'1-D linear convolution (mode="full"). result[k] = Σ a[i]·v[k−i].', params:[['a','Array*','First array (flattened, any dtype → FLOAT64)'],['v','Array*','Second array (flattened, any dtype → FLOAT64)']], ret:'1-D FLOAT64 array, length = len(a) + len(v) − 1.', notes:'Both inputs flattened and converted to FLOAT64. O(n1·n2) naive algorithm — for large arrays (N>1000) consider FFT-based methods.', example:`Array *a = arange(1,4,1,FLOAT64);   // [1,2,3]
Array *b = arange(4,7,1,FLOAT64);   // [4,5,6]
Array *c = convolve(a, b);           // [4,13,28,27,18]`, rel:['correlate','polymul','fft'] },
correlate: { m:'signal', sig:'Array* correlate(Array *a, Array *v, const char *mode)', desc:'1-D cross-correlation. Σ a[i]·v[i+k]. Mode: "full" "valid" "same".', params:[['a','Array*','First array (flattened)'],['v','Array*','Second array'],['mode','const char*','"full"=all, "valid"=no padding, "same"=same length as a']], ret:'FLOAT64 array.', example:`Array *xc = correlate(signal, template, "same");`, rel:['convolve'] },

// ── Comparison & Logic ──
greater: { m:'compare', sig:'Array* greater(Array *x1, Array *x2)', desc:'Element-wise > (greater than). Returns BOOL array.', params:[['x1','Array*','First'],['x2','Array*','Second']], ret:'BOOL array of broadcast shape.', example:`Array *mask = greater(A, threshold);  // A > threshold`, rel:['greater_equal','less','equal','isclose'] },
greater_equal: { m:'compare', sig:'Array* greater_equal(Array *x1, Array *x2)', desc:'Element-wise >=.', params:[['x1','Array*','First'],['x2','Array*','Second']], example:`Array *mask = greater_equal(A, 0);  // non-negative`, rel:['greater','less_equal'] },
equal: { m:'compare', sig:'Array* equal(Array *x1, Array *x2)', desc:'Element-wise == (equality). Returns BOOL array.', example:`Array *eq = equal(A, B);`, rel:['greater','less','isclose'] },
less: { m:'compare', sig:'Array* less(Array *x1, Array *x2)', desc:'Element-wise <. Returns BOOL array.', params:[['x1','Array*','First'],['x2','Array*','Second']], example:`Array *lt = less(A, tooBig);`, rel:['less_equal','greater'] },
less_equal: { m:'compare', sig:'Array* less_equal(Array *x1, Array *x2)', desc:'Element-wise <=.', params:[['x1','Array*','First'],['x2','Array*','Second']], rel:['less','greater_equal'] , example:'Array *le = less_equal(A, ub);'},
isclose: { m:'compare', sig:'Array* isclose(Array *a, Array *b, double rtol, double atol, int equal_nan)', desc:'Element-wise approximate equality: |a−b| ≤ atol + rtol·|b|.', params:[['a','Array*','First'],['b','Array*','Second'],['rtol','double','Relative tolerance (e.g. 1e-5)'],['atol','double','Absolute tolerance (e.g. 1e-8)'],['equal_nan','int','If 1, NaN==NaN returns true']], ret:'BOOL array of broadcast shape.', example:`Array *close = isclose(expected, actual, 1e-5, 1e-8, 0);`, rel:['equal','greater','less'] },
logical_and: { m:'compare', sig:'Array* logical_and(Array *x1, Array *x2)', desc:'Element-wise logical AND. Both inputs treated as truthy (≠0).', params:[['x1','Array*','First'],['x2','Array*','Second']], ret:'BOOL array.', example:`Array *both = logical_and(A>0, A<10);`, rel:['logical_or','logical_xor','logical_not','bitwise_and'] },
logical_or: { m:'compare', sig:'Array* logical_or(Array *x1, Array *x2)', desc:'Element-wise logical OR.', params:[['x1','Array*','First'],['x2','Array*','Second']], example:`Array *either = logical_or(mask1, mask2);`, rel:['logical_and','logical_xor'] },
logical_xor: { m:'compare', sig:'Array* logical_xor(Array *x1, Array *x2)', desc:'Element-wise logical XOR (exclusive OR).', params:[['x1','Array*','First'],['x2','Array*','Second']], example:`Array *one = logical_xor(mask1, mask2);`, rel:['logical_and','logical_or'] },
logical_not: { m:'compare', sig:'Array* logical_not(Array *arr)', desc:'Element-wise logical NOT.', params:[['arr','Array*','Input']], example:`Array *inverse = logical_not(mask);`, rel:['logical_and','invert'] },
all: { m:'compare', sig:'Array* all(Array *arr, int axis, int keepdims)', desc:'Test whether all elements evaluate to true (≠0) along axis.', params:[['arr','Array*','Input'],['axis','int','Axis (-1=flat)'],['keepdims','int','Keep dims']], ret:'BOOL array.', example:`if (*(uint8_t*)all(A>0, -1, 0)->data) printf("All positive\\n");`, rel:['any','count_nonzero'] },
any: { m:'compare', sig:'Array* any(Array *arr, int axis, int keepdims)', desc:'Test whether any element evaluates to true along axis.', params:[['arr','Array*','Input'],['axis','int','Axis (-1=flat)'],['keepdims','int','Keep reduced dims']], ret:'BOOL array.', example:`int hasNaN = *(uint8_t*)any(isnan_check, -1, 0)->data;`, rel:['all','count_nonzero'] },
where: { m:'compare', sig:'Array* where(Array *condition, Array *x, Array *y)', desc:'Return elements chosen from x or y depending on condition.', params:[['condition','Array*','BOOL condition array'],['x','Array*','Values where true (broadcastable)'],['y','Array*','Values where false (broadcastable)']], ret:'Array of broadcast shape, promoted dtype.', notes:'All three inputs broadcast to common shape. This is the 3-argument form.', example:`Array *z = where(mask, a, b);  // mask[i] ? a[i] : b[i]`, rel:['choose','clip','compress'] },
count_nonzero: { m:'compare', sig:'Array* count_nonzero(Array *arr, int axis)', desc:'Count number of non-zero elements along axis.', params:[['arr','Array*','Input'],['axis','int','Axis (-1=flat)']], ret:'INT64 array.', example:`Array *n = count_nonzero(A, -1);  // total nonzero count`, rel:['all','any','sum'] },

// ── Search & Set ──
argmax: { m:'searchm', sig:'Array* argmax(Array *arr, int axis, int keepdims)', desc:'Indices of maximum values along axis.', params:[['arr','Array*','Input'],['axis','int','Axis (-1=flat)'],['keepdims','int','Keep dims']], ret:'INT64 index array.', example:`Array *imax = argmax(A, 1, 1);  // column index of max per row`, rel:['argmin','argsort','max'] },
argmin: { m:'searchm', sig:'Array* argmin(Array *arr, int axis, int keepdims)', desc:'Indices of minimum values along axis.', params:[['arr','Array*','Input'],['axis','int','Axis (-1=flat)'],['keepdims','int','Keep dims']], example:`Array *imin = argmin(A, 0, 0);`, rel:['argmax','min'] },
argsort: { m:'searchm', sig:'Array* argsort(Array *arr, int axis, const char *kind)', desc:'Return indices that would sort the array along axis.', params:[['arr','Array*','Input'],['axis','int','Axis to sort'],['kind','const char*','"quicksort" (default) or "mergesort" (stable)']], ret:'INT64 index array, same shape.', notes:'Mergesort is stable: equal keys preserve original relative order.', example:`Array *idx = argsort(A, -1, "mergesort");
// A->data[idx[i]] is sorted`, rel:['argpartition','searchsorted'] },
argpartition: { m:'searchm', sig:'Array* argpartition(Array *arr, int kth, int axis)', desc:'Indices that partially sort — kth element is in its final sorted position.', params:[['arr','Array*','Input'],['kth','int','Pivot position'],['axis','int','Axis']], ret:'INT64 index array, same shape.', notes:'Faster than argsort when only the k smallest/largest are needed.', example:`Array *idx = argpartition(A, 2, -1);  // first 3 elements are ≤ remaining`, rel:['argsort','partition'] },
argwhere: { m:'searchm', sig:'Array* argwhere(Array *arr)', desc:'Indices of non-zero elements, as a (n_nonzero, ndim) array.', params:[['arr','Array*','Input (elements ≠0 are True)']], ret:'2-D INT64 array, shape (n_nonzero, ndim).', example:`Array *coords = argwhere(A > 0);  // positions of positives
// coords[0,:] = [row, col] of first positive`, rel:['where','count_nonzero'] },
unique: { m:'searchm', sig:'UniqueResult unique(Array *arr, int return_index, int return_inverse, int return_counts)', desc:'Find sorted unique elements of an array.', params:[['arr','Array*','Input (flattened)'],['return_index','int','If 1, also return indices of first occurrences'],['return_inverse','int','If 1, return indices to reconstruct input'],['return_counts','int','If 1, return count of each unique value']], ret:'UniqueResult {values, indices, inverse, counts}. Non-requested fields = NULL.', example:`UniqueResult ur = unique(A, 0, 0, 1);
// ur.values = sorted unique; ur.counts = frequency per value`, rel:['intersect1d','setdiff1d','searchsorted'] },
intersect1d: { m:'searchm', sig:'Array* intersect1d(Array *a, Array *b, int assume_unique)', desc:'Find sorted unique intersection of two arrays.', params:[['a','Array*','First input (any shape/dtype)'],['b','Array*','Second input'],['assume_unique','int','If non-zero, skip sort+dedup (inputs must already be sorted and unique)']], ret:'1-D array: FLOAT64 for real inputs, COMPLEX128 if either input is complex.', notes:'Two-pointer merge after sorting: O(n₁log n₁ + n₂log n₂). Complex comparison is lexicographic (real first). Empty input→empty output.', example:`Array *both = intersect1d(a, b, 0);  // sorted unique values in both`, rel:['unique','setdiff1d','searchsorted'] },
setdiff1d: { m:'searchm', sig:'Array* setdiff1d(Array *arr1, Array *arr2)', desc:'Elements in arr1 that are not in arr2 (sorted, unique).', params:[['arr1','Array*','First set'],['arr2','Array*','Set to exclude']], ret:'1-D FLOAT64 array.', example:`Array *onlyIn1 = setdiff1d(A, B);  // A \\ B`, rel:['intersect1d','unique'] },
searchsorted: { m:'searchm', sig:'Array* searchsorted(Array *a, Array *v, const char *side)', desc:'Find indices where elements of v should be inserted into sorted a to preserve order.', params:[['a','Array*','1-D sorted array (real)'],['v','Array*','Values to locate (any shape)'],['side','const char*','"left"=first suitable index; "right"=last suitable index']], ret:'INT64 array, same shape as v. Indices in [0, len(a)].', notes:'a must be sorted — NOT validated. Binary search O(n·log m).', example:`Array *pos = searchsorted(bins, values, "left");  // which bin?`, rel:['digitize','unique','argsort'] },
digitize: { m:'searchm', sig:'Array* digitize(Array *arr, Array *bins, int right)', desc:'Return bin indices for each value in arr.', params:[['arr','Array*','Values to bin (any shape, any real dtype)'],['bins','Array*','1-D monotonically increasing bin edges'],['right','int','0=left-inclusive intervals [...]), 1=right-inclusive (...]']], ret:'INT64 array, same shape as arr. Indices in [0, len(bins)].', example:`Array *x = arange(0, 2, 0.5, FLOAT64);
Array *bins = create_array((int[]){2},1,FLOAT64); ((double*)bins->data)[0]=0.5; ((double*)bins->data)[1]=1.5;
Array *idx = digitize(x, bins, 0);  // [0,0,1,1,2]`, rel:['searchsorted','histogram'] },

// ── Array Manipulation ──
clip: { m:'manip', sig:'Array* clip(Array *arr, Array *min, Array *max)', desc:'Clip (limit) values in an array to [min, max].', params:[['arr','Array*','Input'],['min','Array*','Lower bound (broadcastable)'],['max','Array*','Upper bound (broadcastable)']], ret:'Same shape and dtype as arr.', notes:'Equivalent to maximum(min, minimum(arr, max)). No check that min ≤ max.', example:`Array *clipped = clip(A, low, high);  // clamp to range`, rel:['maximum','minimum','where'] },
choose: { m:'manip', sig:'Array* choose(Array *arr, int num_choices, Array **choices, int mode)', desc:'Construct array from index array and list of choices.', params:[['arr','Array*','INT64 index array (0..num_choices-1)'],['num_choices','int','Number of choices'],['choices','Array**','Array of arrays (must be broadcastable)'],['mode','int','0=RAISE, 1=WRAP, 2=CLIP for out-of-range indices']], ret:'Broadcast shape array, promoted dtype.', example:`Array *choices[] = {a, b, c};
Array *result = choose(idx, 3, choices, CHOOSE_RAISE);`, rel:['where','take'] },
compress: { m:'manip', sig:'Array* compress(Array *condition, Array *a, int axis)', desc:'Return elements of a where condition is true, along axis.', params:[['condition','Array*','1-D BOOL array (flattened if needed)'],['a','Array*','Input'],['axis','int','Axis (0 for 1-D)']], ret:'Array with axis reduced to number of True values.', example:`Array *pos = compress(mask, A, 0);  // rows where mask is true`, rel:['extract','take','where'] },
npc_delete: { m:'manip', sig:'Array* npc_delete(Array *arr, Array *obj, int axis)', desc:'Return new array with sub-arrays deleted along axis.', params:[['arr','Array*','Input'],['obj','Array*','INT64 indices to delete'],['axis','int','Axis to delete along']], ret:'New array with obj entries removed along axis.', notes:'Renamed from delete for C++ compatibility (delete is a C++ keyword).', example:`Array *trimmed = npc_delete(A, indices, 0);  // remove rows`, rel:['insert','append'] },
extract: { m:'manip', sig:'Array* extract(Array *condition, Array *arr)', desc:'Return elements of arr where condition is true (flat 1-D output).', params:[['condition','Array*','BOOL condition'],['arr','Array*','Source values']], ret:'1-D array of same dtype, length = #true.', notes:'Both arrays are flattened first. Result is always 1-D.', example:`Array *pos = extract(A > 0, A);  // all positive values as 1-D`, rel:['compress','where','argwhere'] },
insert: { m:'manip', sig:'Array* insert(Array *arr, Array *obj, Array *values, int axis)', desc:'Insert values before the given indices along axis.', params:[['arr','Array*','Input'],['obj','Array*','INT64 indices (where to insert before)'],['values','Array*','Values to insert (must match along other axes)'],['axis','int','Axis']], ret:'New array with values inserted.', example:`Array *expanded = insert(A, positions, newRows, 0);`, rel:['npc_delete','append','concatenate'] },
place: { m:'manip', sig:'int place(Array *arr, Array *mask, Array *vals)', desc:'Change elements of arr where mask is True (in-place).', params:[['arr','Array*','Array to modify (in-place)'],['mask','Array*','BOOL mask'],['vals','Array*','Values to place (repeated cyclically if shorter)']], ret:'0 on success.', example:`place(A, A<0, zerosArr);  // set all negatives to zero`, rel:['put','putmask'] },
put: { m:'manip', sig:'int put(Array *arr, Array *indices, Array *values, const char *mode)', desc:'Replace elements at flat indices. Mode: "raise","wrap","clip".', params:[['arr','Array*','Array (modified in-place)'],['indices','Array*','INT64 flat indices'],['values','Array*','Replacement values'],['mode','const char*','"raise" "wrap" "clip"']], ret:'0 on success.', example:`put(A, idx, vals, "raise");`, rel:['place','putmask','take'] },
putmask: { m:'manip', sig:'void putmask(Array *arr, Array *mask, Array *values)', desc:'Set arr[mask] = values. In-place; values broadcast to mask.', params:[['arr','Array*','Array to modify'],['mask','Array*','BOOL mask'],['values','Array*','Source values (broadcast to arr shape)']], example:`putmask(A, A<0, negVals);`, rel:['put','place'] },
append: { m:'manip', sig:'Array* append(Array *arr, Array *values, int axis)', desc:'Append values to end of array along axis.', params:[['arr','Array*','Base'],['values','Array*','Values to append'],['axis','int','Axis (-1=flatten both first)']], example:`Array *extended = append(A, newRows, 0);`, rel:['concatenate','insert'] },
trim_zeros: { m:'manip', sig:'Array* trim_zeros(Array *arr, const char *trim)', desc:'Trim leading and/or trailing zeros from 1-D array.', params:[['arr','Array*','1-D input'],['trim','const char*','"f" (front only), "b" (back only), "fb" (both)']], ret:'1-D array (subset of arr).', example:`Array *t = trim_zeros(arr, "fb");  // strip zeros both ends`, rel:['extract'] },
around: { m:'manip', sig:'Array* around(Array *arr, int decimals)', desc:'Round to given number of decimals. decimals<0 rounds to powers of 10.', params:[['arr','Array*','Input'],['decimals','int','Decimal places (0=nearest integer)']], example:`Array *r = around(arr, 2);  // round to 2 decimals`, rel:['floor_array','ceil_array','arint','fix'] },
fix: { m:'manip', sig:'Array* fix(Array *arr)', desc:'Round toward zero. Same as trunc.', params:[['arr','Array*','Input']], example:`Array *f = fix(arr);  // truncate toward zero`, rel:['atrunc','floor_array','around'] },

// ── Bitwise ──
bitwise_and: { m:'bitwise', sig:'Array* bitwise_and(Array *x1, Array *x2)', desc:'Element-wise bitwise AND. Integer types only.', params:[['x1','Array*','First (integer)'],['x2','Array*','Second (integer)']], ret:'Same dtype as inputs, broadcast shape.', example:`Array *ba = bitwise_and(a, b);`, rel:['bitwise_or','bitwise_xor','bitwise_not','logical_and'] },
bitwise_or: { m:'bitwise', sig:'Array* bitwise_or(Array *x1, Array *x2)', desc:'Element-wise bitwise OR.', params:[['x1','Array*','First (integer)'],['x2','Array*','Second (integer)']], example:`Array *bo = bitwise_or(a, b);`, rel:['bitwise_and','bitwise_xor'] },
bitwise_xor: { m:'bitwise', sig:'Array* bitwise_xor(Array *x1, Array *x2)', desc:'Element-wise bitwise XOR.', params:[['x1','Array*','First (integer)'],['x2','Array*','Second (integer)']], rel:['bitwise_and','bitwise_or'] , example:'Array *bx = bitwise_xor(a, b);'},
bitwise_not: { m:'bitwise', sig:'Array* bitwise_not(Array *arr)', desc:'Element-wise bitwise NOT (ones\' complement).', params:[['arr','Array*','Integer array']], ret:'Same shape and dtype.', example:`Array *bn = bitwise_not(arr);`, rel:['invert','bitwise_and'] },
invert: { m:'bitwise', sig:'Array* invert(Array *arr)', desc:'Bitwise negation (alias for bitwise_not).', params:[['arr','Array*','Integer array']], example:`Array *inv = invert(arr);`, rel:['bitwise_not'] },
left_shift: { m:'bitwise', sig:'Array* left_shift(Array *x1, Array *x2)', desc:'Element-wise left shift: x1 << x2. Integer types only.', params:[['x1','Array*','Base (integer)'],['x2','Array*','Shift (integer)']], example:`Array *ls = left_shift(a, b);`, rel:['right_shift'] },
right_shift: { m:'bitwise', sig:'Array* right_shift(Array *x1, Array *x2)', desc:'Element-wise right shift: x1 >> x2. Integer types only.', params:[['x1','Array*','Base (integer)'],['x2','Array*','Shift (integer)']], example:`Array *rs = right_shift(a, b);`, rel:['left_shift'] },

// ── I/O ──
readtxt: { m:'io', sig:'Array* readtxt(const char *fname, const char *delim, char comment, int skiplines, int max_rows, Array *usecols, int ndmin, int unpack, DataType dtype)', desc:'Read data from a text file. Like numpy.loadtxt.', params:[['fname','const char*','File path'],['delim','const char*','Field delimiter (NULL=whitespace)'],['comment','char','Comment character (# is common)'],['skiplines','int','Skip first N lines'],['max_rows','int','Max rows to read (0=all)'],['usecols','Array*','Column indices (NULL=all)'],['ndmin','int','Minimum output dimensions'],['unpack','int','If non-zero, transpose result'],['dtype','DataType','Output type']], ret:'FLOAT64 array.', example:`Array *data = readtxt("data.csv", ",", '#', 1, 0, NULL, 2, 0, FLOAT64);`, rel:['genfromtxt','savetxt','frombuffer'] },
genfromtxt: { m:'io', sig:'Array* genfromtxt(const char *filename, const char *delim, int skip_header)', desc:'Read text file with missing value handling.', params:[['filename','const char*','File path'],['delim','const char*','Delimiter'],['skip_header','int','Lines to skip']], ret:'FLOAT64 array. Missing/empty values → NaN.', example:`Array *data = genfromtxt("data.txt", ",", 1);`, rel:['readtxt','savetxt'] },
savetxt: { m:'io', sig:'int savetxt(const char *filename, Array *arr, const char *delim)', desc:'Save array to a text file. Returns 0 on success.', params:[['filename','const char*','Output path'],['arr','Array*','Array to save (≤2-D)'],['delim','const char*','Delimiter (NULL=space)']], ret:'0 on success.', example:`savetxt("out.txt", A, "\\t");  // tab-separated`, rel:['readtxt','savez'] },
savez: { m:'io', sig:'int savez(const char *filename, Array **arrays, const char **names, int n)', desc:'Save multiple arrays in uncompressed NPZ format (requires libzip).', params:[['filename','const char*','Output .npz path'],['arrays','Array**','Array of pointers to arrays'],['names','const char**','Array of names for each array'],['n','int','Number of arrays']], ret:'0 on success.', notes:'REQUIRES libzip at link time (-lzip). Without libzip, returns error.', example:`const char *names[] = {"x","y"};
savez("data.npz", (Array*[]){xArr, yArr}, names, 2);`, rel:['savetxt'] },

// ── Utilities ──
nan_to_num: { m:'utils', sig:'Array* nan_to_num(Array *arr)', desc:'Replace NaN with 0, +Inf with MAX, -Inf with -MAX.', params:[['arr','Array*','Input (any dtype including complex)']], ret:'Same shape and dtype.', notes:'Complex: NaN in real or imag → zero that component. Inf→MAX of appropriate type (FLT_MAX, DBL_MAX, etc.).', example:`Array *clean = nan_to_num(dirtyData);  // no more NaN/Inf`, rel:['unwrap'] },
unwrap: { m:'utils', sig:'Array* unwrap(Array *arr, double discont, int axis)', desc:'Unwrap radian phases by changing absolute jumps > discont to their 2π complement.', params:[['arr','Array*','Phase angles'],['discont','double','Maximum discontinuity (default=π)'],['axis','int','Axis along which to unwrap']], ret:'Same shape and dtype.', notes:'Period is always 2π. discont is the detection threshold, NOT the period. Jumps > discont are shifted by multiples of 2π.', example:`Array *unwrapped = unwrap(phase, M_PI, 0);  // smooth phase`, rel:['angle'] },
real_array: { m:'utils', sig:'Array* real_array(Array *arr)', desc:'Real part of complex array. Non-complex inputs return a copy.', params:[['arr','Array*','Input']], example:`Array *re = real_array(complexArr);`, rel:['imag_array','angle'] },
imag_array: { m:'utils', sig:'Array* imag_array(Array *arr)', desc:'Imaginary part of complex array. Non-complex inputs return zeros.', params:[['arr','Array*','Input (any dtype)']], example:`Array *im = imag_array(complexArr);`, rel:['real_array','angle'] },
ix_: { m:'utils', sig:'int ix_(Array **arrays, int n, Array ***out)', desc:'Construct an open mesh from multiple index arrays for fancy indexing.', params:[['arrays','Array**','n 1-D index arrays'],['n','int','Number of arrays'],['out','Array***','Output: reshaped arrays with newaxes for broadcasting']], ret:'0 on success.', example:`Array *in[2] = {rowIdx, colIdx}, *out[2];
ix_(in, 2, &out);
A_subset = take_along_axis(A, ...);  // fancy grid indexing`, rel:['meshgrid'] },
ediff1d: { m:'utils', sig:'Array* ediff1d(Array *y, Array *to_begin, Array *to_end)', desc:'Differences between consecutive elements of a 1-D array, with optional prepend/append.', params:[['y','Array*','Input (flattened)'],['to_begin','Array*','Prepend before diff result (NULL=none)'],['to_end','Array*','Append after diff result (NULL=none)']], ret:'1-D FLOAT64 array.', example:`Array *d = ediff1d(y, NULL, NULL);  // y[1:]-y[:-1]`, rel:['diff'] },
bincount: { m:'utils', sig:'Array* bincount(Array *a, Array *weights, int minlength)', desc:'Count number of occurrences of each value in array of non-negative ints.', params:[['a','Array*','1-D non-negative integers'],['weights','Array*','Optional — weight for each occurrence (NULL=count 1)'],['minlength','int','Minimum output length (padded with zeros)']], ret:'1-D FLOAT64 array.', example:`Array *a = create_array((int[]){5},1,INT32); // {0,1,1,3,2}
Array *cnt = bincount(a, NULL, 0);  // count of each value`, rel:['histogram'] },
lcm: { m:'utils', sig:'Array* lcm(Array *x1, Array *x2)', desc:'Least common multiple. Integer dtypes only. With broadcasting.', params:[['x1','Array*','First integer'],['x2','Array*','Second integer']], ret:'INT64 array of broadcast shape.', notes:'lcm(a,b) = |a·b| / gcd(a,b). May overflow for large values (matches NumPy behavior).', example:`Array *l = lcm(a, b);  // lcm of each pair`, rel:['gcd_array'] },
gcd_array: { m:'utils', sig:'Array* gcd_array(Array *x1, Array *x2)', desc:'Greatest common divisor. Integer dtypes only. Euclidean algorithm.', params:[['x1','Array*','First integer'],['x2','Array*','Second integer']], ret:'INT64 array of broadcast shape.', example:`Array *g = gcd_array(a, b);  // gcd of each pair`, rel:['lcm'] },
piecewise: { m:'utils', sig:'Array* piecewise(Array *x, Array **condlist, int ncond, pf_func *funclist)', desc:'Evaluate a piecewise-defined function. Each condition→function mapping.', params:[['x','Array*','Input'],['condlist','Array**','ncond BOOL condition arrays'],['ncond','int','Number of conditions'],['funclist','pf_func*','ncond function pointers: double (*)(double)']], ret:'FLOAT64 array, same shape as x.', notes:'Conditions are evaluated in order; first true condition wins. If no condition is true, result is 0.0.', example:`pf_func funcs[] = {fn_pos, fn_neg};
Array *conds[] = {greater(x, 0), less(x, 0)};
Array *result = piecewise(x, conds, 2, funcs);`, rel:['where','choose'] },
i0: { m:'utils', sig:'Array* i0(Array *arr)', desc:'Modified Bessel function of the first kind, order 0: I₀(x).', params:[['arr','Array*','Real input']], ret:'FLOAT64 array.', notes:'Uses polynomial approximation accurate to ~2e-16. Used internally by kaiser window.', example:`Array *b = i0(x);`, rel:['kaiser'] },
bessel_i0: { m:'utils', sig:'double bessel_i0(double x)', desc:'Scalar I₀(x). Low-level helper used by i0() and kaiser().', params:[['x','double','Real value']], ret:'double — I₀(x).', example:`double bi0 = bessel_i0(5.0);`, rel:['i0','kaiser'] },
fill_diagonal: { m:'utils', sig:'void fill_diagonal(Array *arr, void *val)', desc:'Fill the main diagonal of a 2-D array with a scalar value (in-place).', params:[['arr','Array*','2-D array (modified)'],['val','void*','Pointer to fill value (type must match arr->dtype)']], example:`double v = 5.0; fill_diagonal(A, &v);`, rel:['diag','diagonal'] },
aldexp: { m:'utils', sig:'Array* aldexp(Array *x1, Array *x2)', desc:'Compute x1 << x2 for integer arrays (arithmetic left shift).', params:[['x1','Array*','Integer base'],['x2','Array*','Integer shift amount']], ret:'INT64 array of broadcast shape.', rel:['left_shift'] , example:'Array *ls = aldexp(base, shift);',},
check_finite: { m:'utils', sig:'void check_finite(Array *arr, int *all_finite)', desc:'Check if all elements are finite (not NaN, not ±Inf). Internal utility.', params:[['arr','Array*','Input'],['all_finite','int*','Output: 1=all finite, 0=has NaN/Inf']] , example:'int ok; check_finite(A, &ok);',},

// ── Datetime ──
datetime_now: { m:'datetime', sig:'DateTime64 datetime_now(void)', desc:'Return the current system time as a DateTime64 value.', params:[], ret:'DateTime64 struct with value in nanoseconds since epoch, unit=DT_UNIT_NS.', notes:'Reads system clock via time(). Unit is always nanoseconds.', example:`DateTime64 now = datetime_now();
char *s = datetime_as_string(&now); printf("%s\\n", s); free(s);`, rel:['datetime_from_string','datetime_as_string'] },
datetime_from_string: { m:'datetime', sig:'DateTime64 datetime_from_string(const char *str, DateTimeUnit unit)', desc:'Parse an ISO 8601 datetime string into a DateTime64 value.', params:[['str','const char*','ISO 8601 string, e.g. "2025-04-27" or "2025-04-27T12:34:56"'],['unit','DateTimeUnit','Storage resolution — DT_UNIT_NS, DT_UNIT_S, DT_UNIT_D, etc.']], ret:'DateTime64 struct. On parse failure, value=0.', notes:'Supports date-only, datetime, and datetime with fractional seconds. Timezone not supported.', example:`DateTime64 dt = datetime_from_string("2025-04-27T12:34:56", DT_UNIT_S);`, rel:['datetime_now','datetime_as_string'] },
datetime_as_string: { m:'datetime', sig:'char* datetime_as_string(const DateTime64 *dt)', desc:'Format a DateTime64 value as an ISO 8601 string.', params:[['dt','const DateTime64*','Pointer to DateTime64 to format']], ret:'malloc\'d string — caller must free(). Returns NULL on error.', notes:'Format depends on unit: date → "YYYY-MM-DD", seconds → "YYYY-MM-DDTHH:MM:SS", ns → with fractional seconds.', example:`char *s = datetime_as_string(&dt);
printf("%s\\n", s);  // "2025-04-27T12:34:56"
free(s);`, rel:['datetime_from_string','datetime_now'] },
datetime_add_days: { m:'datetime', sig:'DateTime64 datetime_add_days(DateTime64 dt, int64_t days)', desc:'Add or subtract days from a DateTime64 value.', params:[['dt','DateTime64','Base datetime'],['days','int64_t','Days to add (positive) or subtract (negative)']], ret:'New DateTime64 value shifted by days.', notes:'Handles month/year boundaries correctly via normalization to days.', example:`DateTime64 tomorrow = datetime_add_days(today, 1);
DateTime64 lastWeek = datetime_add_days(today, -7);`, rel:['datetime_sub_days','datetime_now'] },
datetime_sub_days: { m:'datetime', sig:'DateTime64 datetime_sub_days(DateTime64 dt, int64_t days)', desc:'Subtract days from a DateTime64 value. Equivalent to add_days(dt, -days).', params:[['dt','DateTime64','Base datetime'],['days','int64_t','Days to subtract']], ret:'New DateTime64 value.', example:`DateTime64 yesterday = datetime_sub_days(today, 1);`, rel:['datetime_add_days'] },
datetime_eq: { m:'datetime', sig:'bool datetime_eq(DateTime64 a, DateTime64 b)', desc:'Compare two DateTime64 values for equality.', params:[['a','DateTime64','First datetime'],['b','DateTime64','Second datetime']], ret:'bool — true if a and b represent the same instant.', notes:'Comparison is in nanoseconds regardless of unit.', example:`if (datetime_eq(d1, d2)) printf("Same moment\\n");`, rel:['datetime_lt','datetime_gt'] },
datetime_lt: { m:'datetime', sig:'bool datetime_lt(DateTime64 a, DateTime64 b)', desc:'Return true if a is strictly earlier than b.', params:[['a','DateTime64','First'],['b','DateTime64','Second']], ret:'bool.', example:`if (datetime_lt(start, end)) { /* start before end */ }`, rel:['datetime_eq','datetime_gt'] },
datetime_gt: { m:'datetime', sig:'bool datetime_gt(DateTime64 a, DateTime64 b)', desc:'Return true if a is strictly later than b.', params:[['a','DateTime64','First'],['b','DateTime64','Second']], ret:'bool.', rel:['datetime_eq','datetime_lt'] },
datetime_to_ns: { m:'datetime', sig:'int64_t datetime_to_ns(DateTime64 dt)', desc:'Convert a DateTime64 value to nanoseconds since epoch.', params:[['dt','DateTime64','Datetime value']], ret:'int64_t — total nanoseconds since 1970-01-01T00:00:00Z.', notes:'Useful for arithmetic and comparison in raw nanosecond form.', example:`int64_t ns = datetime_to_ns(dt);
int64_t diff_ns = datetime_to_ns(t2) - datetime_to_ns(t1);`, rel:['datetime_now','datetime_eq'] },
is_busday: { m:'datetime', sig:'Array* is_busday(Array *dates, const char *weekmask, Array *holidays)', desc:'Check whether each date in an array is a business day. Like numpy.is_busday.', params:[['dates','Array*','1-D DATETIME64 array'],['weekmask','const char*','7-char string, 1=workday 0=holiday, e.g. "1111100" for Mon-Fri'],['holidays','Array*','1-D DATETIME64 array of holidays (NULL=none)']], ret:'1-D BOOL array, same length as dates.', notes:'weekmask characters: Mon Tue Wed Thu Fri Sat Sun. Default "1111100" = Sat/Sun off.', example:`Array *dates = /* datetime64 array */;
Array *bus = is_busday(dates, "1111100", NULL);
// bus[i] = true if dates[i] is a weekday`, rel:['datetime_from_string','datetime_now'] },
};

/* ═══ Helpers ─── */
function esc(s) { return (s||'').replace(/</g,'&lt;').replace(/>/g,'&gt;'); }
