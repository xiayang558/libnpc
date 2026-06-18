#include "def.h"
#include "array.h"




Array* bincount(Array *a, Array *weights, int minlength);

Array* extract(Array *condition, Array *arr);               // extract elements satisfying a condition (similar to numpy.extract)

int find_interval(double x, double *xp, int len);
Array* lcm(Array *x1, Array *x2);
Array* gcd_array(Array *x1, Array *x2);  // greatest common divisor (numpy.gcd)
Array* aldexp(Array *x1, Array *x2);

Array* left_shift(Array *x1, Array *x2);
Array* right_shift(Array *x1, Array *x2);

Array* less(Array *x1, Array *x2);
Array* less_equal(Array *x1, Array *x2);
Array* linspace(float start, float stop, int num);

Array* logspace(double start, double stop, int num, double base, int endpoint);         // evenly spaced numbers on a log scale (similar to numpy.logspace)

int64_t gcd_int64(int64_t a, int64_t b);
int64_t to_int64(void *ptr, DataType dtype);
uint64_t to_uint64(void *ptr, DataType dtype);
double to_double(void *ptr, DataType dtype);
complex double to_complex(void *ptr, DataType dtype);

Array* readtxt(const char *fname, const char *delimiter, char comment,
               int skiplines, int max_rows, Array *usecols, int ndmin, int unpack, DataType dtype);