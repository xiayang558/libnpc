#include "array.h"
#include "histogram.h"


int histogram2d(Array *x, Array *y, int bins_x, int bins_y,
                Array *range_x, Array *range_y, int density,
                Histogram2DResult *result) {
    if (x == NULL || y == NULL || result == NULL) {
        fprintf(stderr, "histogram2d: NULL argument\n");
        return -1;
    }
    if (x->ndim != 1 || y->ndim != 1) {
        fprintf(stderr, "histogram2d: x and y must be 1-dimensional\n");
        return -1;
    }
    if (x->dtype != FLOAT64 || y->dtype != FLOAT64) {
        fprintf(stderr, "histogram2d: only FLOAT64 supported\n");
        return -1;
    }
    int n = x->size;
    if (n != y->size) {
        fprintf(stderr, "histogram2d: x and y must have same length\n");
        return -1;
    }
    if (bins_x <= 0 || bins_y <= 0) {
        fprintf(stderr, "histogram2d: bins must be positive\n");
        return -1;
    }

    double *x_data = (double*)x->data;
    double *y_data = (double*)y->data;
    // determine range
    double x_min, x_max, y_min, y_max;
    if (range_x != NULL && range_x->size == 2) {
        x_min = ((double*)range_x->data)[0];
        x_max = ((double*)range_x->data)[1];
    } else {
        x_min = x_data[0];
        x_max = x_data[0];
        for (int i = 1; i < n; ++i) {
            if (x_data[i] < x_min) x_min = x_data[i];
            if (x_data[i] > x_max) x_max = x_data[i];
        }
        // expand by a tiny margin to avoid boundary values falling into the last bin
        double eps_x = (x_max - x_min) * 1e-10;
        x_min -= eps_x;
        x_max += eps_x;
    }
    if (range_y != NULL && range_y->size == 2) {
        y_min = ((double*)range_y->data)[0];
        y_max = ((double*)range_y->data)[1];
    } else {
        y_min = y_data[0];
        y_max = y_data[0];
        for (int i = 1; i < n; ++i) {
            if (y_data[i] < y_min) y_min = y_data[i];
            if (y_data[i] > y_max) y_max = y_data[i];
        }
        double eps_y = (y_max - y_min) * 1e-10;
        y_min -= eps_y;
        y_max += eps_y;
    }
    if (x_max <= x_min || y_max <= y_min) {
        fprintf(stderr, "histogram2d: invalid range (min >= max)\n");
        return -1;
    }
    double bin_width_x = (x_max - x_min) / bins_x;
    double bin_width_y = (y_max - y_min) / bins_y;

    // allocatecountmatrix (bins_x * bins_y)
    int64_t *counts = calloc(bins_x * bins_y, sizeof(int64_t));
    if (!counts) return -1;

    for (int i = 0; i < n; ++i) {
        double xv = x_data[i];
        double yv = y_data[i];
        int ix = (int)((xv - x_min) / bin_width_x);
        int iy = (int)((yv - y_min) / bin_width_y);
        if (ix < 0) ix = 0;
        if (ix >= bins_x) ix = bins_x - 1;
        if (iy < 0) iy = 0;
        if (iy >= bins_y) iy = bins_y - 1;
        counts[ix * bins_y + iy]++;
    }

    // build edge arrays
    int x_edges_shape[1] = {bins_x + 1};
    int y_edges_shape[1] = {bins_y + 1};
    Array *x_edges = create_array(x_edges_shape, 1, FLOAT64);
    Array *y_edges = create_array(y_edges_shape, 1, FLOAT64);
    if (!x_edges || !y_edges) {
        free(counts);
        if (x_edges) free_array(x_edges);
        if (y_edges) free_array(y_edges);
        return -1;
    }
    double *x_edges_data = (double*)x_edges->data;
    double *y_edges_data = (double*)y_edges->data;
    for (int i = 0; i <= bins_x; ++i) x_edges_data[i] = x_min + i * bin_width_x;
    for (int i = 0; i <= bins_y; ++i) y_edges_data[i] = y_min + i * bin_width_y;

    // build H array (shape bins_x, bins_y)
    int H_shape[2] = {bins_x, bins_y};
    Array *H = create_array(H_shape, 2, FLOAT64);
    if (!H) {
        free(counts);
        free_array(x_edges);
        free_array(y_edges);
        return -1;
    }
    double *H_data = (double*)H->data;
    double total_area = (x_max - x_min) * (y_max - y_min);
    for (int i = 0; i < bins_x; ++i) {
        for (int j = 0; j < bins_y; ++j) {
            double cnt = (double)counts[i * bins_y + j];
            if (density) {
                H_data[i * bins_y + j] = cnt / (n * bin_width_x * bin_width_y);
            } else {
                H_data[i * bins_y + j] = cnt;
            }
        }
    }
    free(counts);
    result->H = H;
    result->x_edges = x_edges;
    result->y_edges = y_edges;
    return 0;
}