#include "array.h"


/**
 * @brief first kind zero-order modified Bessel function I0(x)
 * @param x input real number (any non-negative value)
 * @return I0(x) double precision computed result
 */
double bessel_i0(double x) {
    // 1. even function property: I0(-x) = I0(x), take absolute value to simplify computation
    double abs_x = fabs(x);
    
    // 2. initialize: n=0 term is 1, cumulative sum starts at 1
    double sum = 1.0;
    double term = 1.0;
    int n = 1;
    
    // 3. iteration precision: relative precision 1e-15, sufficient for double precision requirements
    const double epsilon = 1e-15;
    
    // 4. iteratively accumulate, until terms are sufficiently small (meeting precision)
    while (term > epsilon * sum)
    {
        // iteratively compute current term: core formula, avoid factorial overflow
        term *= (abs_x * abs_x) / (4.0 * n * n);
        // accumulate to total sum
        sum += term;
        n++;
    }
    
    return sum;
}