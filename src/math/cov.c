#include "array.h"
#include "shape.h"


Array* cov(Array *m, int rowvar, int ddof, DataType dtype) {
    // ignore dtype parameter, always return FLOAT64
    (void)dtype;
    if (m == NULL) {
        fprintf(stderr, "cov: NULL array argument\n");
        return NULL;
    }
    if (m->ndim != 2) {
        fprintf(stderr, "cov: input must be 2-dimensional\n");
        return NULL;
    }

    // 1. ensure each row is one variable, shape (n_vars, n_obs)
    Array *data;
    if (rowvar) {
        data = astype(m, FLOAT64);
        if (data == NULL) return NULL;
    } else {
        Array *tmp = transpose(m);
        if (tmp == NULL) return NULL;
        data = astype(tmp, FLOAT64);
        free_array(tmp);
        if (data == NULL) return NULL;
    }

    int n_vars = data->shape[0];
    int n_obs = data->shape[1];
    if (ddof < 0 || ddof >= n_obs) {
        fprintf(stderr, "cov: ddof must be between 0 and %d (inclusive)\n", n_obs - 1);
        free_array(data);
        return NULL;
    }

    // 2. compute per-row mean, keep dims (n_vars, 1) for broadcasting
    Array *mean_row = mean(data, 1, 1);  // shape (n_vars, 1)
    if (mean_row == NULL) {
        free_array(data);
        return NULL;
    }

    // 3. broadcast mean to same shape as data (n_vars, n_obs)
    int target_shape[2] = {n_vars, n_obs};
    Array *mean_broadcast = broadcast_to(mean_row, target_shape, 2);
    free_array(mean_row);
    if (mean_broadcast == NULL) {
        free_array(data);
        return NULL;
    }

    // 4. center the data
    Array *centered = subtract(data, mean_broadcast);
    free_array(mean_broadcast);
    if (centered == NULL) {
        free_array(data);
        return NULL;
    }

    // 5. compute covariance matrix = (centered @ centered.T) / (n_obs - ddof)
    Array *centered_t = transpose(centered);
    if (centered_t == NULL) {
        free_array(data);
        free_array(centered);
        return NULL;
    }
    Array *cov_raw = dot(centered, centered_t);
    free_array(centered);
    free_array(centered_t);
    if (cov_raw == NULL) {
        free_array(data);
        return NULL;
    }

    double factor = 1.0 / (n_obs - ddof);
    
    Array *result = multiply_scalar(cov_raw, &factor);

    free_array(cov_raw);
    free_array(data);

    return result;
}