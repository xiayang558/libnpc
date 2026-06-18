#include "array.h"
#include "shape.h"


Array* corrcoef(Array *m, int rowvar) {
    if (m == NULL) {
        fprintf(stderr, "corrcoef: NULL array argument\n");
        return NULL;
    }
    if (m->ndim != 2) {
        fprintf(stderr, "corrcoef: input must be 2-dimensional\n");
        return NULL;
    }

    int is_complex = (m->dtype == COMPLEX64 || m->dtype == COMPLEX128);

    // ensure each row is one variable, and unify to FLOAT64 (real) or COMPLEX128 (complex)
    Array *data;
    if (rowvar) {
        if (is_complex) {
            data = astype(m, COMPLEX128);
        } else {
            data = astype(m, FLOAT64);
        }
    } else {
        Array *tmp = transpose(m);
        if (tmp == NULL) return NULL;
        if (is_complex) {
            data = astype(tmp, COMPLEX128);
        } else {
            data = astype(tmp, FLOAT64);
        }
        free_array(tmp);
    }
    if (data == NULL) return NULL;

    int n_vars = data->shape[0];
    int n_obs = data->shape[1];
    if (n_obs < 2) {
        fprintf(stderr, "corrcoef: need at least 2 observations\n");
        free_array(data);
        return NULL;
    }

    // compute per-row mean (keepdims=True)
    Array *mean_row = mean(data, 1, 1);
    if (mean_row == NULL) {
        free_array(data);
        return NULL;
    }

    // broadcast mean to (n_vars, n_obs)
    int target_shape[2] = {n_vars, n_obs};
    Array *mean_broadcast = broadcast_to(mean_row, target_shape, 2);
    free_array(mean_row);
    if (mean_broadcast == NULL) {
        free_array(data);
        return NULL;
    }

    // center the data
    Array *centered = subtract(data, mean_broadcast);
    free_array(mean_broadcast);
    if (centered == NULL) {
        free_array(data);
        return NULL;
    }

    // compute covariance matrix: real uses ordinary transpose, complex uses conjugate transpose
    Array *cov;
    if (is_complex) {
        Array *centered_h = conj_transpose(centered);
        if (centered_h == NULL) {
            free_array(data);
            free_array(centered);
            return NULL;
        }
        cov = dot(centered, centered_h);
        free_array(centered_h);
    } else {
        Array *centered_t = transpose(centered);
        if (centered_t == NULL) {
            free_array(data);
            free_array(centered);
            return NULL;
        }
        cov = dot(centered, centered_t);
        free_array(centered_t);
    }
    free_array(centered);
    if (cov == NULL) {
        free_array(data);
        return NULL;
    }

    // divide by (n_obs - 1)
    double factor = 1.0 / (n_obs - 1);
    Array *factor_arr = create_scalar_array(&factor, FLOAT64);
    if (factor_arr == NULL) {
        free_array(data);
        free_array(cov);
        return NULL;
    }
    Array *cov_scaled = multiply_scalar(cov, factor_arr);
    free_array(factor_arr);
    free_array(cov);
    if (cov_scaled == NULL) {
        free_array(data);
        return NULL;
    }

    // extract variance (diagonal) and compute standard deviation
    Array *std = NULL;
    if (is_complex) {
        // complex covariance matrix diagonal is real (since it is z * conj(z)), directly take the real part
        Array *var = create_array((int[]){n_vars}, 1, FLOAT64);
        if (var == NULL) {
            free_array(data);
            free_array(cov_scaled);
            return NULL;
        }
        double *var_data = (double*)var->data;
        complex double *cov_data = (complex double*)cov_scaled->data;
        for (int i = 0; i < n_vars; i++) {
            var_data[i] = creal(cov_data[i * n_vars + i]);
        }
        std = sqrt_array(var);
        free_array(var);
    } else {
        Array *var = create_array((int[]){n_vars}, 1, FLOAT64);
        if (var == NULL) {
            free_array(data);
            free_array(cov_scaled);
            return NULL;
        }
        double *var_data = (double*)var->data;
        double *cov_data = (double*)cov_scaled->data;
        for (int i = 0; i < n_vars; i++) {
            var_data[i] = cov_data[i * n_vars + i];
        }
        std = sqrt_array(var);
        free_array(var);
    }
    if (std == NULL) {
        free_array(data);
        free_array(cov_scaled);
        return NULL;
    }

    // compute correlation coefficient matrix
    DataType out_dtype = is_complex ? COMPLEX128 : FLOAT64;
    Array *corr = create_array((int[]){n_vars, n_vars}, 2, out_dtype);
    if (corr == NULL) {
        free_array(data);
        free_array(cov_scaled);
        free_array(std);
        return NULL;
    }

    if (is_complex) {
        complex double *corr_data = (complex double*)corr->data;
        complex double *cov_data = (complex double*)cov_scaled->data;
        double *std_data = (double*)std->data;
        for (int i = 0; i < n_vars; i++) {
            for (int j = 0; j < n_vars; j++) {
                double denom = std_data[i] * std_data[j];
                if (denom == 0.0) {
                    corr_data[i * n_vars + j] = NAN + NAN*I;
                } else {
                    corr_data[i * n_vars + j] = cov_data[i * n_vars + j] / denom;
                }
            }
        }
        // force diagonal to 1+0j
        for (int i = 0; i < n_vars; i++) {
            corr_data[i * n_vars + i] = 1.0 + 0.0*I;
        }
    } else {
        double *corr_data = (double*)corr->data;
        double *cov_data = (double*)cov_scaled->data;
        double *std_data = (double*)std->data;
        for (int i = 0; i < n_vars; i++) {
            for (int j = 0; j < n_vars; j++) {
                double denom = std_data[i] * std_data[j];
                if (denom == 0.0) {
                    corr_data[i * n_vars + j] = NAN;
                } else {
                    corr_data[i * n_vars + j] = cov_data[i * n_vars + j] / denom;
                }
            }
        }
        for (int i = 0; i < n_vars; i++) {
            corr_data[i * n_vars + i] = 1.0;
        }
    }

    free_array(cov_scaled);
    free_array(std);
    free_array(data);
    return corr;
}
