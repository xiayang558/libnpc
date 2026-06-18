#include <stdlib.h>
#include <string.h>
#include <complex.h>
#include "array.h"
#include "utils.h"

/**
 * rfft2 — 2D Real FFT (like numpy.fft.rfft2)
 *
 * Apply full FFT along axis=0, RFFT along axis=1 (keep non-negative frequencies).
 * Output last axis length = n/2 + 1.
 *
 * @param arr 2D real array
 * @return 2D COMPLEX128 half-spectrum
 */
Array* rfft2(Array *arr) {
    if (arr == NULL) {
        fprintf(stderr, "rfft2: NULL argument\n");
        return NULL;
    }
    if (arr->ndim != 2) {
        fprintf(stderr, "rfft2: input must be 2D\n");
        return NULL;
    }

    int rows = arr->shape[0];
    int cols = arr->shape[1];
    int out_cols = cols / 2 + 1;
    size_t in_sz = dtype_size(arr->dtype);
    char *src = (char*)arr->data;

    /* Copy input as COMPLEX128 */
    complex double *data = malloc(rows * cols * sizeof(complex double));
    if (!data) return NULL;

    for (int i = 0; i < rows * cols; i++) {
        double r = 0, im = 0;
        switch (arr->dtype) {
            case COMPLEX64:
                r = crealf(*(complex float*)(src + i * in_sz));
                im = cimagf(*(complex float*)(src + i * in_sz)); break;
            case COMPLEX128:
                r = creal(*(complex double*)(src + i * in_sz));
                im = cimag(*(complex double*)(src + i * in_sz)); break;
            default:
                r = get_value(src + i * in_sz, arr->dtype); break;
        }
        data[i] = r + im * I;
    }

    /* Step 1: FFT along columns (axis=0) */
    {
        complex double *col = malloc(rows * sizeof(complex double));
        if (!col) { free(data); return NULL; }

        for (int c = 0; c < cols; c++) {
            for (int r = 0; r < rows; r++)
                col[r] = data[r * cols + c];

            Array *col_arr = create_array((int[]){rows}, 1, COMPLEX128);
            if (!col_arr) { free(col); free(data); return NULL; }
            memcpy(col_arr->data, col, rows * sizeof(complex double));
            Array *fft_col = fft(col_arr);
            free_array(col_arr);
            if (!fft_col) { free(col); free(data); return NULL; }

            complex double *fc = (complex double*)fft_col->data;
            for (int r = 0; r < rows; r++)
                data[r * cols + c] = fc[r];
            free_array(fft_col);
        }
        free(col);
    }

    /* Step 2: RFFT along rows (axis=1) → output shrinks to out_cols */
    complex double *out = malloc(rows * out_cols * sizeof(complex double));
    if (!out) { free(data); return NULL; }

    for (int r = 0; r < rows; r++) {
        Array *row_arr = create_array((int[]){cols}, 1, COMPLEX128);
        if (!row_arr) { free(out); free(data); return NULL; }
        memcpy(row_arr->data, data + r * cols, cols * sizeof(complex double));
        Array *rfft_row = rfft(row_arr);
        free_array(row_arr);
        if (!rfft_row) { free(out); free(data); return NULL; }

        memcpy(out + r * out_cols, rfft_row->data, out_cols * sizeof(complex double));
        free_array(rfft_row);
    }
    free(data);

    int out_shape[2] = {rows, out_cols};
    Array *result = create_array(out_shape, 2, COMPLEX128);
    if (!result) { free(out); return NULL; }
    memcpy(result->data, out, rows * out_cols * sizeof(complex double));
    free(out);
    return result;
}
