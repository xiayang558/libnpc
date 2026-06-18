/**
 * random.h — numpy-compatible pseudo-random number generation
 *
 * Core RNG: xoshiro256** algorithm seeded via splitmix64.
 * All distribution functions take a shape/ndim pair to control output dimensions.
 * NULL shape or ndim=0 produces a scalar (single-element 1D array).
 */

#ifndef RANDOM_H
#define RANDOM_H

#include <stdint.h>
#include "array.h"

/* ─── RNG engine ────────────────────────────────────────────────── */

void     random_seed(uint64_t seed);        /* Seed the global RNG state */
uint64_t random_uint64(void);               /* Generate a uniform uint64 */
double   random_double(void);               /* Generate a uniform double in [0, 1) */

/* ─── Uniform generators ────────────────────────────────────────── */

Array* random_sample(int *shape, int ndim); /* Uniform [0,1) — like numpy.random.random_sample */
Array* random_rand(int *shape, int ndim);   /* Alias for random_sample — like numpy.random.rand */
Array* random_randint(int low, int high, int *shape, int ndim, DataType dtype); /* Uniform integers in [low, high) */

/* ─── Normal distribution ───────────────────────────────────────── */

double random_standard_normal_val(void);                              /* Single N(0,1) variate */
Array* random_standard_normal(int *shape, int ndim);                 /* Standard normal N(0,1) */
Array* random_randn(int *shape, int ndim);                           /* Alias — like numpy.random.randn */
Array* random_normal(double loc, double scale, int *shape, int ndim); /* Normal N(loc, scale^2) */

/* ─── Exponential / Uniform ─────────────────────────────────────── */

double random_standard_exponential_val(void);                         /* Single Exp(1) variate */
Array* random_standard_exponential(int *shape, int ndim);            /* Exponential with scale=1 */
Array* random_exponential(double scale, int *shape, int ndim);       /* Exponential with given scale */
Array* random_uniform(double low, double high, int *shape, int ndim); /* Uniform [low, high) */

/* ─── Gamma family ──────────────────────────────────────────────── */

double random_standard_gamma_val(double shape);                       /* Single Gamma(shape, 1) variate */
Array* random_standard_gamma(double shape, int *out_shape, int ndim); /* Gamma(shape, 1) */
Array* random_gamma(double shape, double scale, int *out_shape, int ndim); /* Gamma(shape, scale) */
Array* random_chisquare(double df, int *shape, int ndim);           /* Chi-squared(df) */
Array* random_beta(double a, double b, int *shape, int ndim);       /* Beta(a, b) */
Array* random_standard_t(double df, int *shape, int ndim);          /* Student's t(df) */
Array* random_f(double dfn, double dfd, int *shape, int ndim);      /* F(dfn, dfd) */

/* ─── Continuous distributions ──────────────────────────────────── */

Array* random_laplace(double loc, double scale, int *shape, int ndim);
Array* random_logistic(double loc, double scale, int *shape, int ndim);
Array* random_lognormal(double mean, double sigma, int *shape, int ndim);
Array* random_pareto(double a, int *shape, int ndim);
Array* random_power(double a, int *shape, int ndim);
Array* random_rayleigh(double scale, int *shape, int ndim);
Array* random_weibull(double a, int *shape, int ndim);
Array* random_standard_cauchy(int *shape, int ndim);
Array* random_gumbel(double loc, double scale, int *shape, int ndim);
Array* random_triangular(double left, double mode, double right, int *shape, int ndim);
Array* random_vonmises(double mu, double kappa, int *shape, int ndim);
Array* random_wald(double mean, double scale, int *shape, int ndim);

/* ─── Discrete distributions ────────────────────────────────────── */

Array* random_geometric(double p, int *shape, int ndim);
Array* random_poisson(double lam, int *shape, int ndim);
Array* random_binomial(int n, double p, int *shape, int ndim);
Array* random_negative_binomial(int n, double p, int *shape, int ndim);
Array* random_hypergeometric(int ngood, int nbad, int nsample, int *shape, int ndim);
Array* random_zipf(double a, int *shape, int ndim);
Array* random_logseries(double p, int *shape, int ndim);

/* ─── Multivariate + utilities ──────────────────────────────────── */

Array* random_permutation(Array *x);                                  /* Fisher-Yates permutation (new copy) */
void   random_shuffle(Array *x);                                       /* In-place Fisher-Yates shuffle */
Array* random_choice(Array *a, int size, int replace, Array *p);      /* Weighted/unweighted random sampling */
Array* random_multinomial(int n, Array *pvals, int *shape, int ndim); /* Multinomial(n, pvals) */
Array* random_multivariate_normal(Array *mean, Array *cov, int *shape, int ndim); /* MVN via Cholesky */
Array* random_dirichlet(Array *alpha, int *shape, int ndim);          /* Dirichlet via gamma normalization */
Array* random_bytes(int n);                                            /* Random bytes */

#endif
