#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include "array.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    Array *hist;        // histogram count values (or density)
    Array *bin_edges;   // bin boundaries (length = bins+1)
} HistogramResult;

typedef struct {
    Array *H;          // 2-D histogram array, shape (bins_x, bins_y)
    Array *x_edges;    // 1-D array, length bins_x+1
    Array *y_edges;    // 1-D array, length bins_y+1
} Histogram2DResult;

// multi-dimensional histogram result
typedef struct {
    Array *hist;        // N-D histogram array
    Array **edges;      // bin boundaries for each dimension (list of array pointers)
    int num_edges;      // length of edges (equals number of dimensions)
} HistogramDDResult;


/**
 * @brief Compute a histogram of a 1-D array.
 * @param x       Input array (must be 1-D FLOAT64)
 * @param bins    Number of bins (positive integer)
 * @param density If 1, normalize to a probability density; otherwise counts
 * @param result  Output histogram result (caller must free result.hist and result.bin_edges)
 * @return 0 on success, -1 on failure
 */
int histogram(Array *x, int bins, int density, HistogramResult *result);


/**
 * @brief Compute a 2-D histogram
 * @param x       1-D FLOAT64 array
 * @param y       1-D FLOAT64 array
 * @param bins_x  Number of bins in the x direction
 * @param bins_y  Number of bins in the y direction
 * @param range_x Optional: array of length 2 [xmin, xmax]; auto-computed if NULL
 * @param range_y Optional: array of length 2 [ymin, ymax]; auto-computed if NULL
 * @param density Whether to normalize to a probability density (each bin divided by total area)
 * @param result  Output result
 * @return 0 on success, -1 on failure
 */
int histogram2d(Array *x, Array *y, int bins_x, int bins_y,
                Array *range_x, Array *range_y, int density,
                Histogram2DResult *result);

/**
 * @brief Compute a multi-dimensional histogram.
 * @param sample      Input data (2-D array with N rows and D columns; each column is a dimension)
 * @param bins        Number of bins (integer or 1-D integer array)
 * @param range       Range for each dimension (may be NULL; auto-computed)
 * @param density     If 1, normalize to a probability density; otherwise counts
 * @param result      Output result (hist and edges); caller must free hist and each edges
 * @return 0 on success, -1 on failure
 */
int histogramdd(Array *sample, Array *bins, Array *range, int density, HistogramDDResult *result);

#ifdef __cplusplus
}
#endif

#endif // HISTOGRAM_H