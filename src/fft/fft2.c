#include <stdlib.h>
#include <string.h>
#include <complex.h>
#include "array.h"
#include "utils.h"

/**
 * fft2 — 2D Fast Fourier Transform (like numpy.fft.fft2)
 *
 * Apply 1D FFT along axis=0 (columns), then along axis=1 (rows).
 *
 * @param arr 2D input array (any numeric type)
 * @return 2D COMPLEX128 spectrum
 */
Array* fft2(Array *arr) {
    if (arr == NULL) {
        fprintf(stderr, "fft2: NULL argument\n");
        return NULL;
    }
    if (arr->ndim != 2) {
        fprintf(stderr, "fft2: input must be 2D\n");
        return NULL;
    }

    int rows = arr->shape[0];
    int cols = arr->shape[1];
    size_t in_sz = dtype_size(arr->dtype);
    char *src = (char*)arr->data;

    /* Step 1: Copy input as COMPLEX128 */
    complex double *data = malloc(rows * cols * sizeof(complex double));
    if (!data) return NULL;

    for (int i = 0; i < rows * cols; i++) {
        double r = 0, im = 0;
        switch (arr->dtype) {
            case COMPLEX64:
                r = crealf(*(complex float*)(src + i * in_sz));
                im = cimagf(*(complex float*)(src + i * in_sz));
                break;
            case COMPLEX128:
                r = creal(*(complex double*)(src + i * in_sz));
                im = cimag(*(complex double*)(src + i * in_sz));
                break;
            default:
                r = get_value(src + i * in_sz, arr->dtype);
                break;
        }
        data[i] = r + im * I;
    }

    /* Step 2: FFT along columns (axis=0) */
    {
        complex double *col = malloc(rows * sizeof(complex double));
        if (!col) { free(data); return NULL; }

        for (int c = 0; c < cols; c++) {
            /* Extract column */
            for (int r = 0; r < rows; r++)
                col[r] = data[r * cols + c];

            /* FFT the column */
            Array *col_arr = create_array((int[]){rows}, 1, COMPLEX128);
            if (!col_arr) { free(col); free(data); return NULL; }
            memcpy(col_arr->data, col, rows * sizeof(complex double));
            Array *fft_col = fft(col_arr);
            free_array(col_arr);
            if (!fft_col) { free(col); free(data); return NULL; }

            /* Write back */
            complex double *fc = (complex double*)fft_col->data;
            for (int r = 0; r < rows; r++)
                data[r * cols + c] = fc[r];
            free_array(fft_col);
        }
        free(col);
    }

    /* Step 3: FFT along rows (axis=1) */
    {
        for (int r = 0; r < rows; r++) {
            Array *row_arr = create_array((int[]){cols}, 1, COMPLEX128);
            if (!row_arr) { free(data); return NULL; }
            memcpy(row_arr->data, data + r * cols, cols * sizeof(complex double));
            Array *fft_row = fft(row_arr);
            free_array(row_arr);
            if (!fft_row) { free(data); return NULL; }
            memcpy(data + r * cols, fft_row->data, cols * sizeof(complex double));
            free_array(fft_row);
        }
    }

    Array *result = create_array(arr->shape, 2, COMPLEX128);
    if (!result) { free(data); return NULL; }
    memcpy(result->data, data, rows * cols * sizeof(complex double));
    free(data);
    return result;
}
