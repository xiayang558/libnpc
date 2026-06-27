#include "array.h"

#ifdef __cplusplus
extern "C" {
#endif

Array* bartlett(int M);
Array* blackman(int M);
Array* kaiser(int M, double beta);
Array* hanning(int m);
Array* hamming(int m);

#ifdef __cplusplus
}
#endif
