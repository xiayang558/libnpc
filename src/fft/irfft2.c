#include <stdlib.h>
#include <string.h>
#include <complex.h>
#include "array.h"
#include "utils.h"

/**
 * irfft2 — 2D Inverse Real FFT (like numpy.fft.irfft2)
 *
 * Apply 1D IFFT along axis=0 (columns), then 1D IRFFT along axis=1 (rows).
 *
 * @param arr 2D half-spectrum array (output of rfft2)
 * @param s   Output shape {rows, cols}; -1 for default
 *            s[0] default arr->shape[0]，s[1] default 2*(arr->shape[1] - 1)
 * @return 2D FLOAT64 real array
 */
Array* irfft2(Array *arr, int s[2]) {
    if (arr == NULL) {
        fprintf(stderr, "irfft2: NULL argument\n");
        return NULL;
    }
    if (arr->ndim != 2) {
        fprintf(stderr, "irfft2: input must be 2D\n");
        return NULL;
    }

    int R = arr->shape[0];
    int C = arr->shape[1];
    int out_rows = (s[0] <= 0) ? R : s[0];
    int out_cols = (s[1] <= 0) ? 2 * (C - 1) : s[1];
    if (out_rows < 1) out_rows = 1;
    if (out_cols < 1) out_cols = 1;

    size_t in_sz = dtype_size(arr->dtype);
    char *src = (char*)arr->data;

    /* Copy input as COMPLEX128 */
    complex double *data = malloc(R * C * sizeof(complex double));
    if (!data) return NULL;

    for (int i = 0; i < R * C; i++) {
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

    /* Step 1: IFFT along columns (axis=0) — supports s[0] pad/truncate */
    int new_rows = out_rows;
    complex double *step1 = malloc(new_rows * C * sizeof(complex double));
    if (!step1) { free(data); return NULL; }

    {
        complex double *col_padded = malloc(new_rows * sizeof(complex double));
        if (!col_padded) { free(data); free(step1); return NULL; }

        for (int c = 0; c < C; c++) {
            /* Extract column, pad/truncate to new_rows */
            int copy_len = (R < new_rows) ? R : new_rows;
            for (int r = 0; r < copy_len; r++)
                col_padded[r] = data[r * C + c];
            for (int r = copy_len; r < new_rows; r++)
                col_padded[r] = 0.0 + 0.0 * I;

            Array *col_arr = create_array((int[]){new_rows}, 1, COMPLEX128);
            if (!col_arr) { free(col_padded); free(data); free(step1); return NULL; }
            memcpy(col_arr->data, col_padded, new_rows * sizeof(complex double));
            Array *ifft_col = ifft(col_arr);
            free_array(col_arr);
            if (!ifft_col) { free(col_padded); free(data); free(step1); return NULL; }

            complex double *fc = (complex double*)ifft_col->data;
            for (int r = 0; r < new_rows; r++)
                step1[r * C + c] = fc[r];
            free_array(ifft_col);
        }
        free(col_padded);
    }
    free(data);

    /* Step 2: IRFFT along rows (axis=1) → real output */
    double *step2 = malloc(new_rows * out_cols * sizeof(double));
    if (!step2) { free(step1); return NULL; }

    for (int r = 0; r < new_rows; r++) {
        Array *row_arr = create_array((int[]){C}, 1, COMPLEX128);
        if (!row_arr) { free(step1); free(step2); return NULL; }
        memcpy(row_arr->data, step1 + r * C, C * sizeof(complex double));
        Array *irfft_row = irfft(row_arr, s[1]);
        free_array(row_arr);
        if (!irfft_row) { free(step1); free(step2); return NULL; }

        memcpy(step2 + r * out_cols, irfft_row->data, out_cols * sizeof(double));
        free_array(irfft_row);
    }
    free(step1);

    /* Build output */
    int out_shape[2] = {new_rows, out_cols};
    Array *result = create_array(out_shape, 2, FLOAT64);
    if (!result) { free(step2); return NULL; }
    memcpy(result->data, step2, new_rows * out_cols * sizeof(double));
    free(step2);
    return result;
}
