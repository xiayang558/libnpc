#include "array.h"
#include "shape.h"
#include "utils.h"

Array* setdiff1d(Array *arr1, Array *arr2) {
    if (arr1 == NULL || arr2 == NULL) {
        fprintf(stderr, "setdiff1d: NULL array argument\n");
        return NULL;
    }
    // flatten and convert to float64
    Array *flat1 = flatten(arr1);
    Array *flat2 = flatten(arr2);
    if (flat1 == NULL || flat2 == NULL) {
        if (flat1) free_array(flat1);
        if (flat2) free_array(flat2);
        return NULL;
    }
    Array *dbl1 = astype(flat1, FLOAT64);
    Array *dbl2 = astype(flat2, FLOAT64);
    free_array(flat1); free_array(flat2);
    if (dbl1 == NULL || dbl2 == NULL) {
        if (dbl1) free_array(dbl1);
        if (dbl2) free_array(dbl2);
        return NULL;
    }
    int n1 = dbl1->size;
    int n2 = dbl2->size;
    double *data1 = (double*)dbl1->data;
    double *data2 = (double*)dbl2->data;
    
    // sort and deduplicate data1
    qsort(data1, n1, sizeof(double), cmp_double);
    int uniq_len = 0;
    for (int i = 0; i < n1; i++) {
        if (i == 0 || data1[i] != data1[i-1]) {
            data1[uniq_len++] = data1[i];
        }
    }
    n1 = uniq_len;
    // sort data2
    qsort(data2, n2, sizeof(double), cmp_double);
    // mark elements to keep
    int *keep = malloc(n1 * sizeof(int));
    if (!keep) {
        free_array(dbl1); free_array(dbl2);
        return NULL;
    }
    for (int i = 0; i < n1; i++) {
        keep[i] = 1;
        int lo = 0, hi = n2 - 1;
        while (lo <= hi) {
            int mid = (lo + hi) / 2;
            if (data2[mid] == data1[i]) {
                keep[i] = 0;
                break;
            } else if (data2[mid] < data1[i]) {
                lo = mid + 1;
            } else {
                hi = mid - 1;
            }
        }
    }
    int out_len = 0;
    for (int i = 0; i < n1; i++) if (keep[i]) out_len++;
    // output type: could try to preserve original integer type if both inputs are integer. for simplicity, output float64
    Array *res = create_array((int[]){out_len}, 1, FLOAT64);
    if (!res) {
        free(keep); free_array(dbl1); free_array(dbl2);
        return NULL;
    }
    double *out_data = (double*)res->data;
    int idx = 0;
    for (int i = 0; i < n1; i++) {
        if (keep[i]) out_data[idx++] = data1[i];
    }
    free(keep);
    free_array(dbl1);
    free_array(dbl2);
    return res;
}