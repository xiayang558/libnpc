#ifndef EIG_H
#define EIG_H

#include "array.h"

typedef struct {
    Array *eigenvalues;   // complex eigenvalues, shape (n,)
    Array *eigenvectors;  // complex eigenvectors, shape (n, n), column-major (each column is an eigenvector)
} EigResult;

EigResult eig(Array *a);

#endif