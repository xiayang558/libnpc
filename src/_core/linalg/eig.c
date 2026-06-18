#include "array.h"
#include "eig.h"

#ifdef USE_LAPACK
// Declare LAPACK functions ((requires linking -llapack -lblas))
void zgeev_(char *jobvl, char *jobvr, int *n,
            complex double *a, int *lda,
            complex double *w, complex double *vl, int *ldvl,
            complex double *vr, int *ldvr,
            complex double *work, int *lwork, double *rwork, int *info);

void dgeev_(char *jobvl, char *jobvr, int *n,
            double *a, int *lda,
            double *wr, double *wi,
            double *vl, int *ldvl,
            double *vr, int *ldvr,
            double *work, int *lwork, int *info);
#endif

// Pure C implementation (Jacobi method, for real symmetric matrices only)
static int jacobi_eigen(double **a, double *eigenvals, double **eigenvecs, int n, int max_iter) {
    // Initialize eigenvectors as identity matrix
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            eigenvecs[i][j] = (i == j) ? 1.0 : 0.0;
        }
    }
    double eps = 1e-10;
    for (int iter = 0; iter < max_iter; ++iter) {
        double max = 0.0;
        int p = 0, q = 1;
        // Find largest off-diagonal element
        for (int i = 0; i < n; ++i) {
            for (int j = i + 1; j < n; ++j) {
                if (fabs(a[i][j]) > max) {
                    max = fabs(a[i][j]);
                    p = i; q = j;
                }
            }
        }
        if (max < eps) break;
        double theta = 0.5 * atan2(2.0 * a[p][q], a[p][p] - a[q][q]);
        double c = cos(theta), s = sin(theta);
        // Update eigenvectors
        for (int i = 0; i < n; ++i) {
            double tmp = eigenvecs[i][p] * c + eigenvecs[i][q] * s;
            eigenvecs[i][q] = -eigenvecs[i][p] * s + eigenvecs[i][q] * c;
            eigenvecs[i][p] = tmp;
        }
        // Update matrix
        double app = a[p][p], aqq = a[q][q], apq = a[p][q];
        a[p][p] = app * c * c + aqq * s * s + 2 * apq * c * s;
        a[q][q] = app * s * s + aqq * c * c - 2 * apq * c * s;
        a[p][q] = 0.0;
        a[q][p] = 0.0;
        for (int i = 0; i < n; ++i) {
            if (i != p && i != q) {
                double aip = a[i][p], aiq = a[i][q];
                a[i][p] = aip * c + aiq * s;
                a[p][i] = a[i][p];
                a[i][q] = -aip * s + aiq * c;
                a[q][i] = a[i][q];
            }
        }
    }
    for (int i = 0; i < n; ++i) eigenvals[i] = a[i][i];
    return 0;
}

EigResult eig(Array *a) {
    EigResult res = {NULL, NULL};
    if (a == NULL || a->ndim != 2 || a->shape[0] != a->shape[1]) {
        fprintf(stderr, "eig: input must be a square matrix\n");
        return res;
    }
    int n = a->shape[0];

#ifdef USE_LAPACK
    // ========== LAPACK version (General: supports complex and general real matrices) ==========
    int is_complex = (a->dtype == COMPLEX64 || a->dtype == COMPLEX128);
    // result arrays
    int shape_eig[1] = {n};
    res.eigenvalues = create_array(shape_eig, 1, COMPLEX128);
    int shape_vec[2] = {n, n};
    res.eigenvectors = create_array(shape_vec, 2, COMPLEX128);
    if (!res.eigenvalues || !res.eigenvectors) goto error;

    // Copy input matrix and convert to required type
    Array *a_copy = copy_array(a);
    if (!a_copy) goto error;
    Array *a_work = astype(a_copy, is_complex ? COMPLEX128 : FLOAT64);
    free_array(a_copy);
    if (!a_work) goto error;

    if (is_complex) {
        // General complex matrix
        char jobvl = 'N', jobvr = 'V';
        int lda = n, ldvr = n, lwork = -1, info;
        complex double *work;
        double *rwork = malloc(2 * n * sizeof(double));
        if (!rwork) goto error;
        complex double wkopt;
        zgeev_(&jobvl, &jobvr, &n, (complex double*)a_work->data, &lda,
               (complex double*)res.eigenvalues->data, NULL, &lda,
               (complex double*)res.eigenvectors->data, &ldvr,
               &wkopt, &lwork, rwork, &info);
        lwork = (int)creal(wkopt);
        work = malloc(lwork * sizeof(complex double));
        if (!work) { free(rwork); goto error; }
        zgeev_(&jobvl, &jobvr, &n, (complex double*)a_work->data, &lda,
               (complex double*)res.eigenvalues->data, NULL, &lda,
               (complex double*)res.eigenvectors->data, &ldvr,
               work, &lwork, rwork, &info);
        free(work);
        free(rwork);
        if (info != 0) {
            fprintf(stderr, "eig: LAPACK zgeev failed with error %d\n", info);
            goto error;
        }
    } else {
        // Real general matrix -> eigenvalues are real or conjugate-complex pairs
        char jobvl = 'N', jobvr = 'V';
        int lda = n, ldvr = n, lwork = -1, info;
        double *wr = malloc(n * sizeof(double));
        double *wi = malloc(n * sizeof(double));
        double *work;
        double wkopt;
        dgeev_(&jobvl, &jobvr, &n, (double*)a_work->data, &lda,
               wr, wi, NULL, &lda,
               (double*)res.eigenvectors->data, &ldvr,
               &wkopt, &lwork, &info);
        lwork = (int)wkopt;
        work = malloc(lwork * sizeof(double));
        if (!work) { free(wr); free(wi); goto error; }
        dgeev_(&jobvl, &jobvr, &n, (double*)a_work->data, &lda,
               wr, wi, NULL, &lda,
               (double*)res.eigenvectors->data, &ldvr,
               work, &lwork, &info);
        free(work);
        if (info != 0) {
            fprintf(stderr, "eig: LAPACK dgeev failed with error %d\n", info);
            free(wr); free(wi);
            goto error;
        }
        // Convert real eigenvalues to complex array
        complex double *eig_data = (complex double*)res.eigenvalues->data;
        for (int i = 0; i < n; ++i) {
            eig_data[i] = wr[i] + wi[i] * I;
        }
        free(wr); free(wi);
    }
    free_array(a_work);
    return res;

#else
    // ========== Pure C version (only supports real symmetric matrices) ==========
    if (a->dtype != FLOAT64 && a->dtype != FLOAT32) {
        fprintf(stderr, "eig: pure C version only supports real symmetric matrix\n");
        return res;
    }
    // Copy matrix to double 2D array
    double **mat = malloc(n * sizeof(double*));
    double **eigvecs = malloc(n * sizeof(double*));
    for (int i = 0; i < n; ++i) {
        mat[i] = malloc(n * sizeof(double));
        eigvecs[i] = malloc(n * sizeof(double));
        for (int j = 0; j < n; ++j) {
            if (a->dtype == FLOAT64)
                mat[i][j] = ((double*)a->data)[i * n + j];
            else
                mat[i][j] = ((float*)a->data)[i * n + j];
        }
    }
    double *eigvals = malloc(n * sizeof(double));
    jacobi_eigen(mat, eigvals, eigvecs, n, 100);
    // Build result arrays (complex, real part = eigenvalue, imag part = 0)
    int shape_eig[1] = {n};
    res.eigenvalues = create_array(shape_eig, 1, COMPLEX128);
    if (!res.eigenvalues) goto cleanup;
    complex double *eig_data = (complex double*)res.eigenvalues->data;
    for (int i = 0; i < n; ++i) eig_data[i] = eigvals[i] + 0.0*I;
    int shape_vec[2] = {n, n};
    res.eigenvectors = create_array(shape_vec, 2, COMPLEX128);
    if (!res.eigenvectors) {
        free_array(res.eigenvalues);
        res.eigenvalues = NULL;
        goto cleanup;
    }
    complex double *vec_data = (complex double*)res.eigenvectors->data;
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            // eigenvectors stored column-wise: eigvecs[row][col] = element row of eigenvector col
            vec_data[i * n + j] = eigvecs[i][j] + 0.0*I;
        }
    }
cleanup:
    for (int i = 0; i < n; ++i) { free(mat[i]); free(eigvecs[i]); }
    free(mat); free(eigvecs); free(eigvals);
    return res;
#endif

error:
    if (res.eigenvalues) free_array(res.eigenvalues);
    if (res.eigenvectors) free_array(res.eigenvectors);
    res.eigenvalues = NULL;
    res.eigenvectors = NULL;
    return res;
}