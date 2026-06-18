#include <stdlib.h>
#include <string.h>
#include <complex.h>
#include "array.h"
#include "utils.h"

/**
 * ifft2 — 2D Inverse Fast Fourier Transform (like numpy.fft.ifft2)
 *
 * Apply 1D IFFT along axis=1 (rows), then along axis=0 (columns).
 *
 * @param arr 2D COMPLEX128 spectrum
 * @return 2D COMPLEX128 time-domain array
 */
Array* ifft2(Array *arr) {
    if (arr == NULL) {
        fprintf(stderr, "ifft2: NULL argument\n");
        return NULL;
    }
    if (arr->ndim != 2) {
        fprintf(stderr, "ifft2: input must be 2D\n");
        return NULL;
    }

    int rows = arr->shape[0];
    int cols = arr->shape[1];
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

    /* Step 1: IFFT along rows (axis=1) */
    for (int r = 0; r < rows; r++) {
        Array *row_arr = create_array((int[]){cols}, 1, COMPLEX128);
        if (!row_arr) { free(data); return NULL; }
        memcpy(row_arr->data, data + r * cols, cols * sizeof(complex double));
        Array *ifft_row = ifft(row_arr);
        free_array(row_arr);
        if (!ifft_row) { free(data); return NULL; }
        memcpy(data + r * cols, ifft_row->data, cols * sizeof(complex double));
        free_array(ifft_row);
    }

    /* Step 2: IFFT along columns (axis=0) */
    {
        complex double *col = malloc(rows * sizeof(complex double));
        if (!col) { free(data); return NULL; }

        for (int c = 0; c < cols; c++) {
            for (int r = 0; r < rows; r++)
                col[r] = data[r * cols + c];

            Array *col_arr = create_array((int[]){rows}, 1, COMPLEX128);
            if (!col_arr) { free(col); free(data); return NULL; }
            memcpy(col_arr->data, col, rows * sizeof(complex double));
            Array *ifft_col = ifft(col_arr);
            free_array(col_arr);
            if (!ifft_col) { free(col); free(data); return NULL; }

            complex double *fc = (complex double*)ifft_col->data;
            for (int r = 0; r < rows; r++)
                data[r * cols + c] = fc[r];
            free_array(ifft_col);
        }
        free(col);
    }

    Array *result = create_array(arr->shape, 2, COMPLEX128);
    if (!result) { free(data); return NULL; }
    memcpy(result->data, data, rows * cols * sizeof(complex double));
    free(data);
    return result;
}
