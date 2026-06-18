#include "array.h"
#include "shape.h"
#include "def.h"

// ==================== helper functions (shared across branches) ====================
static void mat_copy(double *src, int rows, int cols, double *dst) {
    memcpy(dst, src, rows * cols * sizeof(double));
}

static int cmp_desc(const void *a, const void *b) {
    double da = *(double*)a, db = *(double*)b;
    return (da < db) - (da > db);
}

static void mat_mul(double *A, int m, int k, double *B, int kk, int n, double *C) {
    if (kk != k) return;
    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            double sum = 0.0;
            for (int l = 0; l < k; ++l) {
                sum += A[i * k + l] * B[l * n + j];
            }
            C[i * n + j] = sum;
        }
    }
}

static void mat_trans(double *A, int m, int n, double *T) {
    for (int i = 0; i < m; ++i)
        for (int j = 0; j < n; ++j)
            T[j * m + i] = A[i * n + j];
}

static int sym_eigen(double *A, int n, double *eigenvals, double *eigenvecs) {
    // Initialize eigenvectors as identity matrix
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            eigenvecs[i * n + j] = (i == j) ? 1.0 : 0.0;
        }
    }
    double eps = 1e-12;
    for (int iter = 0; iter < 100; ++iter) {
        double max_off = 0.0;
        int p = 0, q = 1;
        for (int i = 0; i < n; ++i) {
            for (int j = i+1; j < n; ++j) {
                double off = fabs(A[i * n + j]);
                if (off > max_off) {
                    max_off = off;
                    p = i; q = j;
                }
            }
        }
        if (max_off < eps) break;
        double app = A[p * n + p];
        double aqq = A[q * n + q];
        double apq = A[p * n + q];
        double theta = 0.5 * atan2(2.0 * apq, app - aqq);
        double c = cos(theta);
        double s = sin(theta);
        for (int i = 0; i < n; ++i) {
            double tmp = eigenvecs[i * n + p] * c + eigenvecs[i * n + q] * s;
            eigenvecs[i * n + q] = -eigenvecs[i * n + p] * s + eigenvecs[i * n + q] * c;
            eigenvecs[i * n + p] = tmp;
        }
        for (int i = 0; i < n; ++i) {
            if (i != p && i != q) {
                double aip = A[i * n + p];
                double aiq = A[i * n + q];
                A[i * n + p] = aip * c + aiq * s;
                A[p * n + i] = A[i * n + p];
                A[i * n + q] = -aip * s + aiq * c;
                A[q * n + i] = A[i * n + q];
            }
        }
        double new_app = app * c * c + aqq * s * s + 2 * apq * c * s;
        double new_aqq = app * s * s + aqq * c * c - 2 * apq * c * s;
        A[p * n + p] = new_app;
        A[q * n + q] = new_aqq;
        A[p * n + q] = 0.0;
        A[q * n + p] = 0.0;
    }
    for (int i = 0; i < n; ++i) eigenvals[i] = A[i * n + i];
    return 0;
}


#ifdef USE_LAPACK

// LAPACK function declarations
void dgesvd_(char *jobu, char *jobvt, int *m, int *n,
             double *a, int *lda, double *s,
             double *u, int *ldu, double *vt, int *ldvt,
             double *work, int *lwork, int *info);

SVDResult svd(Array *a, int full_matrices) {
    SVDResult res = {NULL, NULL, NULL};
    if (a == NULL || a->ndim != 2) {
        fprintf(stderr, "svd: input must be a 2D matrix\n");
        return res;
    }
    if (a->dtype != FLOAT64 && a->dtype != FLOAT32) {
        fprintf(stderr, "svd: only real floating-point types supported\n");
        return res;
    }
    // convert to FLOAT64
    Array *A_arr = astype(a, FLOAT64);
    if (!A_arr) return res;
    int m = A_arr->shape[0];
    int n = A_arr->shape[1];
    // transpose to column-major
    double *a_col = malloc(m * n * sizeof(double));
    if (!a_col) { free_array(A_arr); return res; }
    double *src = (double*)A_arr->data;
    for (int i = 0; i < m; ++i)
        for (int j = 0; j < n; ++j)
            a_col[j * m + i] = src[i * n + j];
    free_array(A_arr);
    // singular value array
    int min_dim = (m < n) ? m : n;
    double *s = malloc(min_dim * sizeof(double));
    if (!s) { free(a_col); return res; }
    // U and VT (column-major)
    int ldu = m, ldvt = n;
    double *u = malloc(m * m * sizeof(double));
    double *vt = malloc(n * n * sizeof(double));
    if (!u || !vt) {
        free(s); free(a_col); free(u); free(vt);
        return res;
    }
    // workspace query
    char jobu = full_matrices ? 'A' : 'S';
    char jobvt = full_matrices ? 'A' : 'S';
    int info;
    int lwork = -1;
    double wkopt;
    dgesvd_(&jobu, &jobvt, &m, &n, a_col, &m, s,
            u, &ldu, vt, &ldvt, &wkopt, &lwork, &info);
    lwork = (int)wkopt;
    double *work = malloc(lwork * sizeof(double));
    if (!work) {
        free(s); free(a_col); free(u); free(vt);
        return res;
    }
    dgesvd_(&jobu, &jobvt, &m, &n, a_col, &m, s,
            u, &ldu, vt, &ldvt, work, &lwork, &info);
    free(work);
    free(a_col);
    if (info != 0) {
        fprintf(stderr, "svd: dgesvd failed with error %d\n", info);
        free(s); free(u); free(vt);
        return res;
    }
    // build row-major U (m x k)
    int k = full_matrices ? m : min_dim;
    res.U = create_array((int[]){m, k}, 2, FLOAT64);
    if (res.U) {
        double *U_data = (double*)res.U->data;
        for (int i = 0; i < m; ++i) {
            for (int j = 0; j < k; ++j) {
                // u is column-major, U[j][i] is stored at position j*m + i
                U_data[i * k + j] = u[j * m + i];
            }
        }
    }
    // singular values s
    res.s = create_array((int[]){min_dim}, 1, FLOAT64);
    if (res.s) memcpy(res.s->data, s, min_dim * sizeof(double));
    // build row-major Vt (k x n)
    int vt_rows = full_matrices ? n : min_dim;
    res.Vt = create_array((int[]){vt_rows, n}, 2, FLOAT64);
    if (res.Vt) {
        double *Vt_data = (double*)res.Vt->data;
        for (int i = 0; i < vt_rows; ++i) {
            for (int j = 0; j < n; ++j) {
                // vt is column-major storing V^T, vt[k][i] equivalent to vt[i*n + k]? needs validation
                // current conversion: Vt_data[i][j] = vt[j][i] (i.e., vt[j * vt_rows + i])
                Vt_data[i * n + j] = vt[j * n + i];
            }
        }
    }
    free(s);
    free(u);
    free(vt);
    return res;
}

#else

#ifdef USE_EIG
#include "eig.h"
#include <stdio.h>

SVDResult svd(Array *a, int full_matrices) {
    SVDResult res = {NULL, NULL, NULL};
    (void)full_matrices; // USE_EIG only supports reduced SVD
    if (a == NULL || a->ndim != 2) {
        fprintf(stderr, "svd: input must be a 2D matrix\n");
        return res;
    }
    if (a->dtype != FLOAT64 && a->dtype != FLOAT32) {
        fprintf(stderr, "svd: only real floating-point types supported\n");
        return res;
    }

    int m = a->shape[0];
    int n = a->shape[1];
    int r = (m < n) ? m : n;  // min(m, n)

    Array *A_f64 = astype(a, FLOAT64);
    if (!A_f64) return res;

    if (m >= n) {
        // ---------- m >= n (tall matrix): A^T A (nxn) -> V, S^2, then U = A V S^{-1} ----------
        Array *At = transpose(A_f64);
        Array *AtA = dot(At, A_f64);
        free_array(At);
        if (!AtA) { free_array(A_f64); return res; }

        EigResult eig_res = eig(AtA);
        free_array(AtA);
        if (!eig_res.eigenvalues || !eig_res.eigenvectors) {
            free_array(A_f64); return res;
        }

        complex double *eig_vals = (complex double*)eig_res.eigenvalues->data;
        complex double *eig_vecs = (complex double*)eig_res.eigenvectors->data;

        double *s_vals = malloc(n * sizeof(double));
        int *idx = malloc(n * sizeof(int));
        for (int i = 0; i < n; ++i) {
            s_vals[i] = sqrt(fmax(creal(eig_vals[i]), 0.0));
            idx[i] = i;
        }
        for (int i = 0; i < n - 1; ++i)
            for (int j = i + 1; j < n; ++j)
                if (s_vals[idx[i]] < s_vals[idx[j]]) {
                    int t = idx[i]; idx[i] = idx[j]; idx[j] = t;
                }

        // construct V (nxn), column vectors are eigenvectors (already sorted)
        double *V = malloc(n * n * sizeof(double));
        for (int i = 0; i < n; ++i)
            for (int j = 0; j < n; ++j)
                V[j * n + i] = creal(eig_vecs[j * n + idx[i]]);

        // compute U = A * V (m x n), then scale each column
        double *U = malloc(m * n * sizeof(double));
        double *A_data = (double*)A_f64->data;
        for (int i = 0; i < m; ++i) {
            for (int j = 0; j < n; ++j) {
                double sum = 0.0;
                for (int k = 0; k < n; ++k)
                    sum += A_data[i * n + k] * V[k * n + j];
                double sj = s_vals[idx[j]];
                U[i * n + j] = (sj > 1e-12) ? sum / sj : 0.0;
            }
        }

        // sign convention: ensure first non-zero element in each column of U is positive
        for (int j = 0; j < n; ++j) {
            for (int i = 0; i < m; ++i) {
                if (fabs(U[i * n + j]) > 1e-12) {
                    if (U[i * n + j] < 0) {
                        for (int k = 0; k < m; ++k) U[k * n + j] *= -1;
                        for (int k = 0; k < n; ++k) V[k * n + j] *= -1;
                    }
                    break;
                }
            }
        }

        // output reduced SVD: U(mxr), S(r), Vt(rxn)
        res.U = create_array((int[]){m, r}, 2, FLOAT64);
        if (res.U) {
            for (int i = 0; i < m; ++i)
                for (int j = 0; j < r; ++j)
                    ((double*)res.U->data)[i * r + j] = U[i * n + j];
        }
        res.s = create_array((int[]){r}, 1, FLOAT64);
        if (res.s) {
            for (int j = 0; j < r; ++j)
                ((double*)res.s->data)[j] = s_vals[idx[j]];
        }
        res.Vt = create_array((int[]){r, n}, 2, FLOAT64);
        if (res.Vt) {
            // Vt = V^T: row i comes from column i of V (the i-th sorted eigenvector)
            for (int i = 0; i < r; ++i)
                for (int j = 0; j < n; ++j)
                    ((double*)res.Vt->data)[i * n + j] = V[j * n + i];
        }

        free(U); free(V); free(s_vals); free(idx);
        free_array(eig_res.eigenvalues);
        free_array(eig_res.eigenvectors);

    } else {
        // ---------- m < n (wide matrix): A A^T (mxm) -> U, S^2, then V^T = S^{-1} U^T A ----------
        Array *At = transpose(A_f64);
        Array *AAt = dot(A_f64, At);
        free_array(At);
        if (!AAt) { free_array(A_f64); return res; }

        EigResult eig_res = eig(AAt);
        free_array(AAt);
        if (!eig_res.eigenvalues || !eig_res.eigenvectors) {
            free_array(A_f64); return res;
        }

        complex double *eig_vals = (complex double*)eig_res.eigenvalues->data;
        complex double *eig_vecs = (complex double*)eig_res.eigenvectors->data;

        double *s_vals = malloc(m * sizeof(double));
        int *idx = malloc(m * sizeof(int));
        for (int i = 0; i < m; ++i) {
            s_vals[i] = sqrt(fmax(creal(eig_vals[i]), 0.0));
            idx[i] = i;
        }
        for (int i = 0; i < m - 1; ++i)
            for (int j = i + 1; j < m; ++j)
                if (s_vals[idx[i]] < s_vals[idx[j]]) {
                    int t = idx[i]; idx[i] = idx[j]; idx[j] = t;
                }

        // construct U (mxm), column vectors are eigenvectors (already sorted)
        double *U = malloc(m * m * sizeof(double));
        for (int i = 0; i < m; ++i)
            for (int j = 0; j < m; ++j)
                U[j * m + i] = creal(eig_vecs[j * m + idx[i]]);

        // compute V^T = S⁻¹ U^T A (m × n)
        double *Vt = malloc(m * n * sizeof(double));
        double *A_data = (double*)A_f64->data;
        for (int i = 0; i < m; ++i) {
            double inv_s = (s_vals[idx[i]] > 1e-12) ? 1.0 / s_vals[idx[i]] : 0.0;
            for (int j = 0; j < n; ++j) {
                double sum = 0.0;
                for (int k = 0; k < m; ++k)
                    sum += U[k * m + i] * A_data[k * n + j];
                Vt[i * n + j] = sum * inv_s;
            }
        }

        // sign convention: ensure first non-zero element in each row of Vt is positive
        for (int j = 0; j < m; ++j) {
            for (int i = 0; i < n; ++i) {
                if (fabs(Vt[j * n + i]) > 1e-12) {
                    if (Vt[j * n + i] < 0) {
                        for (int k = 0; k < n; ++k) Vt[j * n + k] *= -1;
                        for (int k = 0; k < m; ++k) U[k * m + j] *= -1;
                    }
                    break;
                }
            }
        }

        // output reduced SVD: U(mxr), S(r), Vt(rxn)
        res.U = create_array((int[]){m, r}, 2, FLOAT64);
        if (res.U) {
            for (int i = 0; i < m; ++i)
                for (int j = 0; j < r; ++j)
                    ((double*)res.U->data)[i * r + j] = U[i * m + j];
        }
        res.s = create_array((int[]){r}, 1, FLOAT64);
        if (res.s) {
            for (int j = 0; j < r; ++j)
                ((double*)res.s->data)[j] = s_vals[idx[j]];
        }
        res.Vt = create_array((int[]){r, n}, 2, FLOAT64);
        if (res.Vt) {
            for (int i = 0; i < r; ++i)
                for (int j = 0; j < n; ++j)
                    ((double*)res.Vt->data)[i * n + j] = Vt[i * n + j];
        }

        free(U); free(Vt); free(s_vals); free(idx);
        free_array(eig_res.eigenvalues);
        free_array(eig_res.eigenvectors);
    }

    free_array(A_f64);
    return res;
}


#else // no LAPACK implementation (use Jacobi method)

SVDResult svd(Array *a, int full_matrices) {
    SVDResult res = {NULL, NULL, NULL};
    (void)full_matrices; // only supports reduced SVD (full_matrices=0)
    if (a == NULL || a->ndim != 2) {
        fprintf(stderr, "svd: input must be a 2D matrix\n");
        return res;
    }
    if (a->dtype != FLOAT64 && a->dtype != FLOAT32) {
        fprintf(stderr, "svd: only real floating-point types supported\n");
        return res;
    }
    Array *A_arr = astype(a, FLOAT64);
    if (!A_arr) return res;
    double *A = (double*)A_arr->data;
    int m = A_arr->shape[0];
    int n = A_arr->shape[1];
    int transposed = 0;
    double *A_work = NULL;
    if (m < n) {
        // transpose to make m >= n
        A_work = malloc(m * n * sizeof(double));
        if (!A_work) goto cleanup;
        mat_trans(A, m, n, A_work);
        free(A);
        A_arr->data = NULL;  /* prevent double-free in free_array */
        A = A_work;
        int tmp = m; m = n; n = tmp;
        transposed = 1;
    } else {
        A_work = malloc(m * n * sizeof(double));
        if (!A_work) goto cleanup;
        mat_copy(A, m, n, A_work);
        A = A_work;
    }
    // compute A^T A
    double *At = malloc(m * n * sizeof(double));
    double *AtA = malloc(n * n * sizeof(double));
    if (!At || !AtA) goto cleanup;
    mat_trans(A, m, n, At);
    mat_mul(At, n, m, A, m, n, AtA);
    // eigen decomposition of AtA
    double *eigenvals = malloc(n * sizeof(double));
    double *eigenvecs = malloc(n * n * sizeof(double));
    double *AtA_copy = malloc(n * n * sizeof(double));
    if (!eigenvals || !eigenvecs || !AtA_copy) goto cleanup;
    mat_copy(AtA, n, n, AtA_copy);
    if (sym_eigen(AtA_copy, n, eigenvals, eigenvecs) != 0) {
        fprintf(stderr, "svd: eigen decomposition failed\n");
        goto cleanup;
    }
    // sort
    int *idx = malloc(n * sizeof(int));
    for (int i = 0; i < n; ++i) idx[i] = i;
    for (int i = 0; i < n-1; ++i)
        for (int j = i+1; j < n; ++j)
            if (eigenvals[idx[i]] < eigenvals[idx[j]]) {
                int t = idx[i]; idx[i] = idx[j]; idx[j] = t;
            }
    double *s_sorted = malloc(n * sizeof(double));
    double *V = malloc(n * n * sizeof(double));
    for (int i = 0; i < n; ++i) {
        s_sorted[i] = sqrt(fmax(eigenvals[idx[i]], 0.0));
        for (int j = 0; j < n; ++j) {
            V[j * n + i] = eigenvecs[j * n + idx[i]];
        }
    }
    // compute AV
    double *AV = malloc(m * n * sizeof(double));
    if (!AV) goto cleanup;
    mat_mul(A, m, n, V, n, n, AV);
    // compute U
    double *U = malloc(m * n * sizeof(double));
    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            if (s_sorted[j] > 1e-12) {
                U[i * n + j] = AV[i * n + j] / s_sorted[j];
            } else {
                U[i * n + j] = 0.0;
            }
        }
    }
    // if transposed, swap U and V
    if (transposed) {
        // the original matrix U should be the previous V, Vt should be the previous U^T
        double *tmpU = U;
        U = V;
        V = tmpU;
        // now U shape (n, n), V shape (m, n) but we need Vt as (n, m) transpose
        // for reduced SVD, the original matrix A_orig = U_orig * S * Vt_orig, where U_orig is m x r, Vt_orig is r x n
        // after transposing back, we get U' = V_orig (n x r), S' = S, Vt' = U_orig^T (r x m)
        // so we need to adjust shapes: final U = U' (n x r), Vt = Vt' (r x m)
        // swap U and V back, U is now n x n, V is now m x n, but we need Vt shape as (n, m)
        // directly transpose V to get Vt, and take first r rows
        double *Vt = malloc(n * m * sizeof(double));
        mat_trans(V, m, n, Vt);
        free(V);
        V = Vt; // now V shape (n, m)
        // note: U is still n x n, but the actual U_orig should be m x r. When m < n it was already transposed, is the original A tall?
        // for simplicity, we only support m >= n, so this transposed branch should not execute, kept here but may have issues
        fprintf(stderr, "svd: transposed case not supported, please ensure m >= n\n");
        goto cleanup;
    }
    // output reduced SVD
    int k = n; // min(m,n)
    res.U = create_array((int[]){m, k}, 2, FLOAT64);
    if (res.U) memcpy(res.U->data, U, m * k * sizeof(double));
    res.s = create_array((int[]){k}, 1, FLOAT64);
    if (res.s) memcpy(res.s->data, s_sorted, k * sizeof(double));
    double *Vt = malloc(k * n * sizeof(double));
    if (Vt) {
        mat_trans(V, k, n, Vt);
        res.Vt = create_array((int[]){k, n}, 2, FLOAT64);
        if (res.Vt) memcpy(res.Vt->data, Vt, k * n * sizeof(double));
        free(Vt);
    }
cleanup:
    if (A != (double*)A_arr->data) free(A);
    free(At); free(AtA); free(AtA_copy);
    free(eigenvals); free(eigenvecs); free(idx);
    free(s_sorted); free(V); free(AV); free(U);
    free_array(A_arr);
    return res;
}
#endif

#endif