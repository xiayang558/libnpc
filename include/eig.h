#ifndef EIG_H
#define EIG_H

#include "array.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    Array *eigenvalues;   // complex eigenvalues, shape (n,)
    Array *eigenvectors;  // complex eigenvectors, shape (n, n), column-major (each column is an eigenvector)
} EigResult;

EigResult eig(Array *a);

#ifdef __cplusplus
}
#endif

#endif