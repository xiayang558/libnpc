#include "array.h"
#include "histogram.h"


// helper: get bins list
static int* get_bins_list(Array *bins, int ndim, int *out_bins) {
    if (bins->ndim == 0) {  // scalar
        int val;
        switch (bins->dtype) {
            case INT8:   val = *(int8_t*)bins->data; break;
            case INT16:  val = *(int16_t*)bins->data; break;
            case INT32:  val = *(int32_t*)bins->data; break;
            case INT64:  val = (int)*(int64_t*)bins->data; break;
            default: return NULL;
        }
        int *b = malloc(ndim * sizeof(int));
        for (int i = 0; i < ndim; i++) b[i] = val;
        *out_bins = ndim;
        return b;
    } else if (bins->ndim == 1) {
        if (bins->size != ndim) return NULL;
        int *b = malloc(ndim * sizeof(int));
        size_t elem_sz = dtype_size(bins->dtype);
        char *data = (char*)bins->data;
        for (int i = 0; i < ndim; i++) {
            void *ptr = data + i * elem_sz;
            switch (bins->dtype) {
                case INT8:   b[i] = *(int8_t*)ptr; break;
                case INT16:  b[i] = *(int16_t*)ptr; break;
                case INT32:  b[i] = *(int32_t*)ptr; break;
                case INT64:  b[i] = (int)*(int64_t*)ptr; break;
                default: free(b); return NULL;
            }
        }
        *out_bins = ndim;
        return b;
    }
    return NULL;
}

// helper: get range
static double** get_range(Array *range, int ndim, double **min_vals, double **max_vals) {
    if (range == NULL) return NULL;
    if (range->ndim == 2 && range->shape[0] == ndim && range->shape[1] == 2) {
        double **r = malloc(ndim * sizeof(double*));
        double *data = (double*)range->data;
        for (int i = 0; i < ndim; i++) {
            r[i] = malloc(2 * sizeof(double));
            r[i][0] = data[i*2];
            r[i][1] = data[i*2+1];
        }
        return r;
    }
    return NULL;
}

int histogramdd(Array *sample, Array *bins, Array *range, int density, HistogramDDResult *result) {
    if (sample == NULL || result == NULL) {
        fprintf(stderr, "histogramdd: NULL argument\n");
        return -1;
    }
    if (sample->ndim != 2) {
        fprintf(stderr, "histogramdd: sample must be 2D (N x D)\n");
        return -1;
    }
    if (sample->dtype != FLOAT64) {
        fprintf(stderr, "histogramdd: only FLOAT64 supported\n");
        return -1;
    }

    int n_obs = sample->shape[0];
    int ndim = sample->shape[1];
    double *data = (double*)sample->data;

    // parse bins
    int *bins_list = NULL;
    int tmp;
    if (bins == NULL) {
        bins_list = malloc(ndim * sizeof(int));
        for (int i = 0; i < ndim; i++) bins_list[i] = 10;
    } else {
        bins_list = get_bins_list(bins, ndim, &tmp);
        if (bins_list == NULL) return -1;
    }

    // parse range
    double **ranges = NULL;
    double *min_vals = malloc(ndim * sizeof(double));
    double *max_vals = malloc(ndim * sizeof(double));
    if (range) {
        ranges = get_range(range, ndim, &min_vals, &max_vals);
        if (ranges) {
            for (int i = 0; i < ndim; i++) {
                min_vals[i] = ranges[i][0];
                max_vals[i] = ranges[i][1];
            }
        }
    }
    if (!ranges) {
        // auto-compute range
        for (int i = 0; i < ndim; i++) {
            min_vals[i] = data[i];
            max_vals[i] = data[i];
        }
        for (int j = 1; j < n_obs; j++) {
            for (int i = 0; i < ndim; i++) {
                double v = data[j * ndim + i];
                if (v < min_vals[i]) min_vals[i] = v;
                if (v > max_vals[i]) max_vals[i] = v;
            }
        }
        // expand by a tiny margin
        double eps = 1e-10;
        for (int i = 0; i < ndim; i++) {
            min_vals[i] -= eps;
            max_vals[i] += eps;
        }
    }

    // compute bin width per dimension
    double *bin_width = malloc(ndim * sizeof(double));
    int total_bins = 1;
    for (int i = 0; i < ndim; i++) {
        bin_width[i] = (max_vals[i] - min_vals[i]) / bins_list[i];
        total_bins *= bins_list[i];
    }

    // allocatecountarray (flatten)
    int64_t *counts = calloc(total_bins, sizeof(int64_t));
    if (!counts) {
        free(bins_list); free(min_vals); free(max_vals); free(bin_width);
        if (ranges) for (int i = 0; i < ndim; i++) free(ranges[i]); free(ranges);
        return -1;
    }

    // map each sample to a bin
    for (int j = 0; j < n_obs; j++) {
        int flat = 0;
        int stride = 1;
        for (int i = ndim - 1; i >= 0; i--) {
            double val = data[j * ndim + i];
            int idx = (int)((val - min_vals[i]) / bin_width[i]);
            if (idx < 0) idx = 0;
            if (idx >= bins_list[i]) idx = bins_list[i] - 1;
            flat += idx * stride;
            stride *= bins_list[i];
        }
        counts[flat]++;
    }

    // create output hist array (ndim-dimensional)
    int *hist_shape = malloc(ndim * sizeof(int));
    memcpy(hist_shape, bins_list, ndim * sizeof(int));
    Array *hist = create_array(hist_shape, ndim, FLOAT64);
    free(hist_shape);
    if (!hist) {
        free(counts); free(bins_list); free(min_vals); free(max_vals); free(bin_width);
        if (ranges) for (int i = 0; i < ndim; i++) free(ranges[i]); free(ranges);
        return -1;
    }
    double *hist_data = (double*)hist->data;

    // key logic: determine output values based on density
    if (density) {
        double vol = 1.0;
        for (int i = 0; i < ndim; i++) vol *= bin_width[i];
        double norm = 1.0 / (n_obs * vol);
        for (int i = 0; i < total_bins; i++) {
            hist_data[i] = (double)counts[i] * norm;
        }
    } else {
        // count mode: use raw frequencies directly
        for (int i = 0; i < total_bins; i++) {
            hist_data[i] = (double)counts[i];
        }
    }

    // createperndim edges
    Array **edges = malloc(ndim * sizeof(Array*));
    for (int i = 0; i < ndim; i++) {
        int edge_shape[1] = {bins_list[i] + 1};
        Array *edge = create_array(edge_shape, 1, FLOAT64);
        if (!edge) {
            for (int j = 0; j < i; j++) free_array(edges[j]);
            free(edges); free_array(hist); free(counts);
            free(bins_list); free(min_vals); free(max_vals); free(bin_width);
            if (ranges) for (int k = 0; k < ndim; k++) free(ranges[k]); free(ranges);
            return -1;
        }
        double *edge_data = (double*)edge->data;
        for (int j = 0; j <= bins_list[i]; j++) {
            edge_data[j] = min_vals[i] + j * bin_width[i];
        }
        edges[i] = edge;
    }

    result->hist = hist;
    result->edges = edges;
    result->num_edges = ndim;

    free(counts);
    free(bins_list);
    free(min_vals);
    free(max_vals);
    free(bin_width);
    if (ranges) for (int i = 0; i < ndim; i++) free(ranges[i]); free(ranges);
    return 0;
}