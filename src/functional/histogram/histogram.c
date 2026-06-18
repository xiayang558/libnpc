#include "array.h"
#include "histogram.h"


int histogram(Array *x, int bins, int density, HistogramResult *result) {
    if (x == NULL || result == NULL) {
        fprintf(stderr, "histogram: NULL argument\n");
        return -1;
    }
    if (x->ndim != 1) {
        fprintf(stderr, "histogram: input must be 1-dimensional\n");
        return -1;
    }
    if (x->dtype != FLOAT64) {
        fprintf(stderr, "histogram: only FLOAT64 supported\n");
        return -1;
    }
    if (bins <= 0) {
        fprintf(stderr, "histogram: bins must be positive\n");
        return -1;
    }
    int n = x->size;
    double *data = (double*)x->data;
    if (n == 0) {
        fprintf(stderr, "histogram: empty array\n");
        return -1;
    }
    // computeminimumandmaximum
    double min_val = data[0], max_val = data[0];
    for (int i = 1; i < n; ++i) {
        if (data[i] < min_val) min_val = data[i];
        if (data[i] > max_val) max_val = data[i];
    }
    // expand by a tiny margin (to avoid boundary values falling into the last bin)
    double eps = (max_val - min_val) * 1e-10;
    min_val -= eps;
    max_val += eps;
    double bin_width = (max_val - min_val) / bins;
    // allocatecountarray (integer)
    int64_t *counts = calloc(bins, sizeof(int64_t));
    if (!counts) return -1;
    for (int i = 0; i < n; ++i) {
        double val = data[i];
        int idx = (int)((val - min_val) / bin_width);
        if (idx < 0) idx = 0;
        if (idx >= bins) idx = bins - 1;
        counts[idx]++;
    }
    // build bin_edges array (length bins+1)
    int edges_shape[1] = {bins + 1};
    Array *edges = create_array(edges_shape, 1, FLOAT64);
    if (!edges) { free(counts); return -1; }
    double *edges_data = (double*)edges->data;
    for (int i = 0; i <= bins; ++i) {
        edges_data[i] = min_val + i * bin_width;
    }
    // build hist array
    int hist_shape[1] = {bins};
    Array *hist = create_array(hist_shape, 1, FLOAT64);
    if (!hist) { free(counts); free_array(edges); return -1; }
    double *hist_data = (double*)hist->data;
    double total = n;
    for (int i = 0; i < bins; ++i) {
        if (density) {
            hist_data[i] = (double)counts[i] / (total * bin_width);
        } else {
            hist_data[i] = (double)counts[i];
        }
    }
    free(counts);
    result->hist = hist;
    result->bin_edges = edges;
    return 0;
}