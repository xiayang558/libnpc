/**
 * distributions.c — gamma family and continuous probability distributions
 */

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "random.h"
#include "array.h"

/** Allocate a FLOAT64 array; NULL/ndim=0 returns scalar (1 element). */
static Array* make_f64(int *shape, int ndim) {
    if (!shape || ndim <= 0) { shape = (int[]){1}; ndim = 1; }
    return create_array(shape, ndim, FLOAT64);
}

/** Compute total element count from shape */
static int out_size(int *shape, int ndim) {
    if (!shape || ndim <= 0) return 1;
    int s = 1; for (int i = 0; i < ndim; i++) s *= shape[i]; return s;
}

/* ─── Gamma family ──────────────────────────────────────────────── */

/**
 * Marsaglia-Tsang rejection method for shape >= 1.
 *
 * Reference: Marsaglia & Tsang (2000), "A Simple Method for
 * Generating Gamma Variables", ACM TOMS.
 */
static double gamma_gt1(double shape) {
    double d = shape - 1.0 / 3.0;
    double c = 1.0 / sqrt(9.0 * d);
    for (;;) {
        double x, v;
        do {
            x = random_standard_normal_val();
            v = 1.0 + c * x;
        } while (v <= 0);
        v = v * v * v;
        double u = random_double();
        if (u < 1.0 - 0.0331 * (x * x) * (x * x)) return d * v;
        if (log(u) < 0.5 * x * x + d * (1.0 - v + log(v))) return d * v;
    }
}

/** Johnk's generator for shape < 1 */
static double gamma_lt1(double shape) {
    double e = exp(1.0);
    for (;;) {
        double u = random_double();
        double v = random_double();
        double w = u * (1.0 - u);
        double y = sqrt(e / shape) * w / (u - 1.0);
        if (w <= 0) continue;
        double z = u * u * v;
        double r = y * (shape - 1.0);
        if (log(z) <= r) return y;
    }
}

/**
 * random_standard_gamma_val — single Gamma(shape, 1) variate
 *
 * Shape >= 1: Marsaglia-Tsang algorithm.
 * Shape < 1:  Johnk's generator.
 *
 * @param shape  Shape parameter (> 0)
 * @return A standard gamma variate
 */
double random_standard_gamma_val(double shape) {
    if (shape <= 0) return 0;
    if (shape >= 1.0) return gamma_gt1(shape);
    return gamma_lt1(shape);
}

/**
 * random_standard_gamma — generate standard gamma variates Gamma(shape, 1)
 *
 * Equivalent to numpy.random.standard_gamma.
 *
 * @param shape      Gamma shape parameter (> 0)
 * @param out_shape  Output shape
 * @param ndim       Number of dimensions
 * @return FLOAT64 array of gamma variates
 */
Array* random_standard_gamma(double shape, int *out_shape, int ndim) {
    Array *a = make_f64(out_shape, ndim);
    if (!a) return NULL;
    int n = a->size;
    double *d = (double*)a->data;
    for (int i = 0; i < n; i++) d[i] = random_standard_gamma_val(shape);
    return a;
}

/**
 * random_gamma — generate gamma variates Gamma(shape, scale)
 *
 * Equivalent to numpy.random.gamma.
 *
 * @param shape      Shape parameter (> 0)
 * @param scale      Scale parameter (> 0)
 * @param out_shape  Output shape
 * @param ndim       Number of dimensions
 * @return FLOAT64 array of gamma variates
 */
Array* random_gamma(double shape, double scale, int *out_shape, int ndim) {
    Array *a = make_f64(out_shape, ndim);
    if (!a) return NULL;
    int n = a->size;
    double *d = (double*)a->data;
    for (int i = 0; i < n; i++)
        d[i] = scale * random_standard_gamma_val(shape);
    return a;
}

/**
 * random_chisquare — generate chi-squared variates with df degrees of freedom
 *
 * Equivalent to numpy.random.chisquare.
 * Computed as: 2 * Gamma(df/2, 1).
 *
 * @param df     Degrees of freedom (> 0)
 * @param shape  Output shape
 * @param ndim   Number of dimensions
 * @return FLOAT64 array of chi-squared variates
 */
Array* random_chisquare(double df, int *shape, int ndim) {
    Array *a = make_f64(shape, ndim);
    if (!a) return NULL;
    int n = a->size;
    double *d = (double*)a->data;
    for (int i = 0; i < n; i++)
        d[i] = 2.0 * random_standard_gamma_val(df / 2.0);
    return a;
}

/**
 * random_beta — generate beta variates Beta(a, b)
 *
 * Equivalent to numpy.random.beta.
 * Computed as: G1 / (G1 + G2) where G1 ~ Gamma(a), G2 ~ Gamma(b).
 *
 * @param a      First shape parameter (> 0)
 * @param b      Second shape parameter (> 0)
 * @param shape  Output shape
 * @param ndim   Number of dimensions
 * @return FLOAT64 array of beta variates in [0, 1]
 */
Array* random_beta(double a, double b, int *shape, int ndim) {
    Array *r = make_f64(shape, ndim);
    if (!r) return NULL;
    int n = r->size;
    double *d = (double*)r->data;
    for (int i = 0; i < n; i++) {
        double ga = random_standard_gamma_val(a);
        double gb = random_standard_gamma_val(b);
        d[i] = ga / (ga + gb);
    }
    return r;
}

/**
 * random_standard_t — generate Student's t variates with df degrees of freedom
 *
 * Equivalent to numpy.random.standard_t.
 * Computed as: Z / sqrt(chi^2 / df) where Z ~ N(0,1).
 *
 * @param df     Degrees of freedom (> 0)
 * @param shape  Output shape
 * @param ndim   Number of dimensions
 * @return FLOAT64 array of t variates
 */
Array* random_standard_t(double df, int *shape, int ndim) {
    Array *r = make_f64(shape, ndim);
    if (!r) return NULL;
    int n = r->size;
    double *d = (double*)r->data;
    for (int i = 0; i < n; i++) {
        double z = random_standard_normal_val();
        double chi = 2.0 * random_standard_gamma_val(df / 2.0);
        d[i] = z / sqrt(chi / df);
    }
    return r;
}

/**
 * random_f — generate F-distribution variates F(dfn, dfd)
 *
 * Equivalent to numpy.random.f.
 * Computed as: (chi^2_n / dfn) / (chi^2_d / dfd).
 *
 * @param dfn    Numerator degrees of freedom (> 0)
 * @param dfd    Denominator degrees of freedom (> 0)
 * @param shape  Output shape
 * @param ndim   Number of dimensions
 * @return FLOAT64 array of F variates
 */
Array* random_f(double dfn, double dfd, int *shape, int ndim) {
    Array *r = make_f64(shape, ndim);
    if (!r) return NULL;
    int n = r->size;
    double *d = (double*)r->data;
    for (int i = 0; i < n; i++) {
        double chin = 2.0 * random_standard_gamma_val(dfn / 2.0);
        double chid = 2.0 * random_standard_gamma_val(dfd / 2.0);
        d[i] = (chin / dfn) / (chid / dfd);
    }
    return r;
}

/* ─── Simple continuous distributions ───────────────────────────── */

/**
 * random_laplace — generate Laplace (double exponential) variates
 *
 * Equivalent to numpy.random.laplace.
 * Uses the inverse CDF method.
 *
 * @param loc    Location parameter (mean)
 * @param scale  Scale parameter (> 0)
 * @param shape  Output shape / ndim
 * @return FLOAT64 array of Laplace variates
 */
Array* random_laplace(double loc, double scale, int *shape, int ndim) {
    Array *r = make_f64(shape, ndim); if (!r) return NULL;
    double *d = (double*)r->data;
    for (int i = 0; i < r->size; i++) {
        double u = random_double() - 0.5;
        d[i] = loc - scale * ((u > 0) ? 1.0 : -1.0) * log(1.0 - 2.0 * fabs(u));
    }
    return r;
}

/**
 * random_logistic — generate logistic variates
 *
 * Equivalent to numpy.random.logistic.
 * Inverse CDF: loc + scale * log(u / (1-u)).
 */
Array* random_logistic(double loc, double scale, int *shape, int ndim) {
    Array *r = make_f64(shape, ndim); if (!r) return NULL;
    double *d = (double*)r->data;
    for (int i = 0; i < r->size; i++) {
        double u = random_double();
        d[i] = loc + scale * log(u / (1.0 - u));
    }
    return r;
}

/**
 * random_lognormal — generate log-normal variates
 *
 * Equivalent to numpy.random.lognormal.
 * Computed as: exp(normal(mean, sigma)).
 */
Array* random_lognormal(double mean, double sigma, int *shape, int ndim) {
    Array *r = make_f64(shape, ndim); if (!r) return NULL;
    double *d = (double*)r->data;
    for (int i = 0; i < r->size; i++)
        d[i] = exp(mean + sigma * random_standard_normal_val());
    return r;
}

/**
 * random_pareto — generate Pareto variates
 *
 * Equivalent to numpy.random.pareto.
 * Computed as: 1 / u^(1/a) with shape parameter a.
 */
Array* random_pareto(double a, int *shape, int ndim) {
    Array *r = make_f64(shape, ndim); if (!r) return NULL;
    double *d = (double*)r->data;
    for (int i = 0; i < r->size; i++)
        d[i] = 1.0 / pow(random_double(), 1.0 / a);
    return r;
}

/**
 * random_power — generate power-distribution variates
 *
 * Equivalent to numpy.random.power.
 * Computed as: u^(1/a) with shape parameter a.
 */
Array* random_power(double a, int *shape, int ndim) {
    Array *r = make_f64(shape, ndim); if (!r) return NULL;
    double *d = (double*)r->data;
    for (int i = 0; i < r->size; i++)
        d[i] = pow(random_double(), 1.0 / a);
    return r;
}

/**
 * random_rayleigh — generate Rayleigh variates
 *
 * Equivalent to numpy.random.rayleigh.
 * Computed as: scale * sqrt(-2 * log(1-u)).
 */
Array* random_rayleigh(double scale, int *shape, int ndim) {
    Array *r = make_f64(shape, ndim); if (!r) return NULL;
    double *d = (double*)r->data;
    for (int i = 0; i < r->size; i++)
        d[i] = scale * sqrt(-2.0 * log(1.0 - random_double()));
    return r;
}

/**
 * random_weibull — generate Weibull variates
 *
 * Equivalent to numpy.random.weibull.
 * Computed as: (-log(1-u))^(1/a) with shape parameter a.
 */
Array* random_weibull(double a, int *shape, int ndim) {
    Array *r = make_f64(shape, ndim); if (!r) return NULL;
    double *d = (double*)r->data;
    for (int i = 0; i < r->size; i++)
        d[i] = pow(-log(1.0 - random_double()), 1.0 / a);
    return r;
}

/**
 * random_standard_cauchy — generate standard Cauchy variates
 *
 * Equivalent to numpy.random.standard_cauchy.
 * Computed as: tan(pi * (u - 0.5)).
 */
Array* random_standard_cauchy(int *shape, int ndim) {
    Array *r = make_f64(shape, ndim); if (!r) return NULL;
    double *d = (double*)r->data;
    for (int i = 0; i < r->size; i++)
        d[i] = tan(M_PI * (random_double() - 0.5));
    return r;
}

/**
 * random_gumbel — generate Gumbel variates
 *
 * Equivalent to numpy.random.gumbel.
 * Inverse CDF: loc - scale * log(-log(u)).
 */
Array* random_gumbel(double loc, double scale, int *shape, int ndim) {
    Array *r = make_f64(shape, ndim); if (!r) return NULL;
    double *d = (double*)r->data;
    for (int i = 0; i < r->size; i++)
        d[i] = loc - scale * log(-log(random_double()));
    return r;
}

/**
 * random_triangular — generate triangular variates on [left, right]
 *
 * Equivalent to numpy.random.triangular.
 * Uses piecewise inverse CDF based on the mode.
 */
Array* random_triangular(double left, double mode, double right, int *shape, int ndim) {
    Array *r = make_f64(shape, ndim); if (!r) return NULL;
    double *d = (double*)r->data;
    double c = (mode - left) / (right - left);
    for (int i = 0; i < r->size; i++) {
        double u = random_double();
        if (u < c)
            d[i] = left + sqrt(u * (right - left) * (mode - left));
        else
            d[i] = right - sqrt((1.0 - u) * (right - left) * (right - mode));
    }
    return r;
}

/**
 * random_vonmises — generate von Mises variates
 *
 * Equivalent to numpy.random.vonmises.
 * Uses the Best-Fisher algorithm for the von Mises distribution.
 *
 * @param mu     Mean direction (in radians)
 * @param kappa  Concentration parameter (>= 0)
 */
Array* random_vonmises(double mu, double kappa, int *shape, int ndim) {
    Array *r = make_f64(shape, ndim); if (!r) return NULL;
    double *d = (double*)r->data;
    if (kappa < 1e-8) {
        /* Near-zero kappa: approximately uniform on [-pi, pi] */
        for (int i = 0; i < r->size; i++)
            d[i] = mu + (random_double() - 0.5) * 2.0 * M_PI;
        return r;
    }
    double a = 1.0 + sqrt(1.0 + 4.0 * kappa * kappa);
    double b = (a - sqrt(2.0 * a)) / (2.0 * kappa);
    double rho = (1.0 + b * b) / (2.0 * b);
    for (int i = 0; i < r->size; i++) {
        for (;;) {
            double u1 = random_double();
            double z = cos(M_PI * u1);
            double f = (1.0 + rho * z) / (rho + z);
            double c = kappa * (rho - f);
            double u2 = random_double();
            if (c * (2.0 - c) - u2 > 0) { d[i] = mu + ((u1 < 0.5) ? -1 : 1) * acos(f); break; }
            if (log(c / u2) + 1.0 - c >= 0) { d[i] = mu + ((u1 < 0.5) ? -1 : 1) * acos(f); break; }
        }
    }
    return r;
}

/**
 * random_wald — generate Wald (inverse Gaussian) variates
 *
 * Equivalent to numpy.random.wald.
 * Uses the transformation method based on chi-squared variates.
 *
 * @param mean   Mean parameter (> 0)
 * @param scale  Scale parameter (lambda, > 0)
 */
Array* random_wald(double mean, double scale, int *shape, int ndim) {
    Array *r = make_f64(shape, ndim); if (!r) return NULL;
    double *d = (double*)r->data;
    double lambda = scale;
    for (int i = 0; i < r->size; i++) {
        double z = random_standard_normal_val();
        double y = z * z;
        double x = mean + mean * mean * y / (2.0 * lambda)
                   - mean / (2.0 * lambda) * sqrt(4.0 * mean * lambda * y + mean * mean * y * y);
        if (random_double() > mean / (mean + x))
            x = mean * mean / x;
        d[i] = x;
    }
    return r;
}
