/**
 * test_linalg_decomp.c — Tests for cholesky, qr, lu, svd, eig
 */

#include "test_utils.h"
#include "eig.h"

/* helper: compute the product C = A * B for real square matrices of size n */
static void matmul_square(double *C, const double *A, const double *B, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            C[i * n + j] = 0;
            for (int k = 0; k < n; k++) {
                C[i * n + j] += A[i * n + k] * B[k * n + j];
            }
        }
    }
}

/* helper: compute A * B^T for square matrices */
static void matmul_ABT(double *C, const double *A, const double *B, int n, int m) {
    /* A: n×m, B^T: m×n => C: n×n */
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            C[i * n + j] = 0;
            for (int k = 0; k < m; k++) {
                C[i * n + j] += A[i * m + k] * B[j * m + k];
            }
        }
    }
}

static Array *make_f64_2d_from_buf(const double *buf, int r, int c) {
    return make_f64_2d(buf, r, c);
}

int main(void) {
    TEST_MAIN("Linear Algebra Decomposition");

    /* ── cholesky ───────────────────────────────────────────────────── */
    TEST_SECTION("cholesky");

    {
        /* [[4,2],[2,5]] — PD, cholesky should give L = [[2,0],[1,2]] */
        Array *a = make_f64_2d((double[]){4, 2, 2, 5}, 2, 2);
        Array *L = cholesky(a);
        ASSERT_NOTNULL(L, "cholesky PD result not null");
        ASSERT_NDIM(L, 2, "cholesky output is 2D");

        /* Verify L * L^T = A (reconstruction) */
        double *Ld = (double*)L->data;
        double recon[4];
        matmul_ABT(recon, Ld, Ld, 2, 2);
        ASSERT_EQ_ARR(recon, ((double[]){4, 2, 2, 5}), 4, TOL_F64,
                      "cholesky L·L^T recovers A");
        free_a(L); free_a(a);
    }

    /* cholesky on non-PD matrix returns NULL */
    {
        Array *a = make_f64_2d((double[]){1, 2, 2, 1}, 2, 2);
        Array *L = cholesky(a);
        ASSERT_NULL(L, "cholesky non-PD returns NULL");
        free_a(a);
    }

    {
        Array *a = make_f64_2d((double[]){0, 0, 0, 0}, 2, 2);
        Array *L = cholesky(a);
        ASSERT_NULL(L, "cholesky zero matrix returns NULL");
        free_a(a);
    }

    /* ── qr ─────────────────────────────────────────────────────────── */
    TEST_SECTION("qr");

    {
        /* QR of 3x2 matrix */
        Array *a = make_f64_2d((double[]){1, 2,
                                          3, 4,
                                          5, 6}, 3, 2);
        QRResult qr_res = qr(a);
        ASSERT_NOTNULL(qr_res.Q, "qr Q not null");
        ASSERT_NOTNULL(qr_res.R, "qr R not null");

        /* Q should be 3x2 */
        ASSERT_EQ_INT(qr_res.Q->shape[0], 3, "qr Q rows = 3");
        ASSERT_EQ_INT(qr_res.Q->shape[1], 2, "qr Q cols = 2");
        /* R should be 2x2 */
        ASSERT_EQ_INT(qr_res.R->shape[0], 2, "qr R rows = 2");
        ASSERT_EQ_INT(qr_res.R->shape[1], 2, "qr R cols = 2");

        /* Verify Q^T * Q ≈ I (inline: Q is 3x2 row-major, QtQ is 2x2) */
        double *Qd = (double*)qr_res.Q->data;
        double QtQ[4] = {0};
        for (int ii = 0; ii < 2; ii++) {
            for (int jj = 0; jj < 2; jj++) {
                for (int kk = 0; kk < 3; kk++) {
                    QtQ[ii * 2 + jj] += Qd[kk * 2 + ii] * Qd[kk * 2 + jj];
                }
            }
        }
        ASSERT_EQ_ARR(QtQ, ((double[]){1, 0, 0, 1}), 4, 1e-6,
                      "qr Q^T·Q ≈ I");

        /* Verify Q * R ≈ A */
        double *Rd = (double*)qr_res.R->data;
        double QR[6]; /* 3x2 */
        /* R is n×n (2×2), Q is m×n (3×2) */
        /* QR[i,k] = sum_j Q[i,j] * R[j,k] */
        for (int i = 0; i < 3; i++) {
            for (int k = 0; k < 2; k++) {
                QR[i * 2 + k] = Qd[i * 2 + 0] * Rd[0 * 2 + k]
                              + Qd[i * 2 + 1] * Rd[1 * 2 + k];
            }
        }
        ASSERT_EQ_ARR(QR, ((double[]){1, 2, 3, 4, 5, 6}), 6, 1e-6,
                      "qr Q·R recovers A");

        free_a(qr_res.Q); free_a(qr_res.R); free_a(a);
    }

    /* qr on square matrix */
    {
        Array *a = make_f64_2d((double[]){1, 2, 3, 4}, 2, 2);
        QRResult qr_res = qr(a);
        ASSERT_NOTNULL(qr_res.Q, "qr square Q not null");
        ASSERT_NOTNULL(qr_res.R, "qr square R not null");
        ASSERT_EQ_INT(qr_res.Q->shape[0], 2, "qr square Q rows=2");
        ASSERT_EQ_INT(qr_res.Q->shape[1], 2, "qr square Q cols=2");
        free_a(qr_res.Q); free_a(qr_res.R); free_a(a);
    }

    /* ── lu ─────────────────────────────────────────────────────────── */
    TEST_SECTION("lu");

    {
        Array *a = make_f64_2d((double[]){2, 1,
                                          1, 2}, 2, 2);
        LUResult lu_res = lu(a);
        ASSERT_NOTNULL(lu_res.P, "lu P not null");
        ASSERT_NOTNULL(lu_res.L, "lu L not null");
        ASSERT_NOTNULL(lu_res.U, "lu U not null");

        /* Check shapes: P, L, U are all n×n */
        ASSERT_EQ_INT(lu_res.P->shape[0], 2, "lu P rows=2");
        ASSERT_EQ_INT(lu_res.L->shape[0], 2, "lu L rows=2");
        ASSERT_EQ_INT(lu_res.U->shape[0], 2, "lu U rows=2");

        /* Verify P*A ≈ L*U */
        double *Pd = (double*)lu_res.P->data;
        double *Ld = (double*)lu_res.L->data;
        double *Ud = (double*)lu_res.U->data;

        /* Compute PA = P * A */
        double PA[4];
        matmul_square(PA, Pd, (double[]){2, 1, 1, 2}, 2);
        /* Compute LU = L * U */
        double LU[4];
        matmul_square(LU, Ld, Ud, 2);
        ASSERT_EQ_ARR(PA, LU, 4, TOL_F64, "lu P·A ≈ L·U");

        free_a(lu_res.P); free_a(lu_res.L); free_a(lu_res.U); free_a(a);
    }

    /* lu on 3x3 — singular (det=0) */
    {
        Array *a = make_f64_2d((double[]){4, 3, 2,
                                          3, 2, 1,
                                          2, 1, 0}, 3, 3);
        LUResult lu_res = lu(a);
        ASSERT_NULL(lu_res.P, "lu 3x3 P is NULL (singular matrix)");
        ASSERT_NULL(lu_res.L, "lu 3x3 L is NULL (singular matrix)");
        ASSERT_NULL(lu_res.U, "lu 3x3 U is NULL (singular matrix)");
        free_a(a);
    }

    /* ── svd ────────────────────────────────────────────────────────── */
    TEST_SECTION("svd");

    {
        /* svd 3x2 with full_matrices=0 */
        Array *a = make_f64_2d((double[]){1, 2,
                                          3, 4,
                                          5, 6}, 3, 2);
        SVDResult svd_res = svd(a, 0);
        ASSERT_NOTNULL(svd_res.U, "svd U not null (full=0)");
        ASSERT_NOTNULL(svd_res.s, "svd s not null (full=0)");
        ASSERT_NOTNULL(svd_res.Vt, "svd Vt not null (full=0)");

        /* U: m×k, s: k, Vt: k×n where k=min(m,n)=2 */
        ASSERT_EQ_INT(svd_res.U->shape[0], 3, "svd U rows=3 (m)");
        ASSERT_EQ_INT(svd_res.U->shape[1], 2, "svd U cols=2 (k)");
        ASSERT_EQ_INT(svd_res.s->size, 2, "svd s has 2 values");
        ASSERT_EQ_INT(svd_res.Vt->shape[0], 2, "svd Vt rows=2 (k)");
        ASSERT_EQ_INT(svd_res.Vt->shape[1], 2, "svd Vt cols=2 (n)");

        /* Verify U^T * U ≈ I_k (U is 3x2 row-major, UtU is 2x2) */
        double *Ud = (double*)svd_res.U->data;
        double UtU[4] = {0};
        for (int ii = 0; ii < 2; ii++) {
            for (int jj = 0; jj < 2; jj++) {
                for (int kk = 0; kk < 3; kk++) {
                    UtU[ii * 2 + jj] += Ud[kk * 2 + ii] * Ud[kk * 2 + jj];
                }
            }
        }
        ASSERT_EQ_ARR(UtU, ((double[]){1, 0, 0, 1}), 4, TOL_SVD,
                      "svd full=0 U^T·U ≈ I");

        /* Verify V * V^T ≈ I_k */
        double *Vtd = (double*)svd_res.Vt->data;
        double VVt[4]; /* 2x2 */
        /* Vt is 2x2 already, so V is its transpose */
        matmul_ABT(VVt, Vtd, Vtd, 2, 2); /* Vt * Vt^T = Vt * V = I? No — need V * V^T */
        /* Vt is 2x2 row-major: Vt[i,j] = V^T[i,j], so V[j,i] = Vt[i,j] */
        /* Compute V * V^T: C[i,k] = sum_j Vt[j,i] * Vt[j,k] */
        {
            double correct[4];
            for (int ii = 0; ii < 2; ii++)
                for (int kk = 0; kk < 2; kk++) {
                    correct[ii*2+kk] = 0;
                    for (int jj = 0; jj < 2; jj++)
                        correct[ii*2+kk] += Vtd[jj*2+ii] * Vtd[jj*2+kk];
                }
            ASSERT_EQ_ARR(correct, ((double[]){1, 0, 0, 1}), 4, TOL_SVD,
                          "svd full=0 V·V^T ≈ I");
        }

        free_a(svd_res.U); free_a(svd_res.s); free_a(svd_res.Vt); free_a(a);
    }

    {
        /* svd 2x2 with full_matrices=1 */
        Array *a = make_f64_2d((double[]){1, 2, 3, 4}, 2, 2);
        SVDResult svd_res = svd(a, 1);
        ASSERT_NOTNULL(svd_res.U, "svd full=1 U not null");
        ASSERT_NOTNULL(svd_res.s, "svd full=1 s not null");
        ASSERT_NOTNULL(svd_res.Vt, "svd full=1 Vt not null");

        /* U: m×m (2x2), s: min(m,n) (2), Vt: n×n (2x2) */
        ASSERT_EQ_INT(svd_res.U->shape[0], 2, "svd full=1 U rows=m");
        ASSERT_EQ_INT(svd_res.U->shape[1], 2, "svd full=1 U cols=m");
        ASSERT_EQ_INT(svd_res.Vt->shape[0], 2, "svd full=1 Vt rows=n");
        ASSERT_EQ_INT(svd_res.Vt->shape[1], 2, "svd full=1 Vt cols=n");

        free_a(svd_res.U); free_a(svd_res.s); free_a(svd_res.Vt); free_a(a);
    }

    /* ── eig ────────────────────────────────────────────────────────── */
    TEST_SECTION("eig");

    {
        /* eig of 2x2 symmetric identity -> eigenvalues [1,1] */
        Array *a = make_f64_2d((double[]){1, 0, 0, 1}, 2, 2);
        EigResult eig_res = eig(a);
        ASSERT_NOTNULL(eig_res.eigenvalues, "eig eigenvalues not null");
        ASSERT_NOTNULL(eig_res.eigenvectors, "eig eigenvectors not null");

        /* eigenvalues should be [1, 1] */
        complex double *evals = (complex double*)eig_res.eigenvalues->data;
        ASSERT_EQ_DBL(creal(evals[0]), 1.0, TOL_F64, "eig identity val[0] real = 1");
        ASSERT_EQ_DBL(creal(evals[1]), 1.0, TOL_F64, "eig identity val[1] real = 1");

        /* Verify for each eigenpair: A·v = λ·v */
        complex double *evecs = (complex double*)eig_res.eigenvectors->data;
        double *Ad = (double*)a->data;
        /* eigenvectors stored column-major (per EigResult doc) */
        for (int k = 0; k < 2; k++) {
            complex double lambda = evals[k];
            /* k-th eigenvector is column k: indices [k*2], [k*2+1] */
            complex double v0 = evecs[k * 2];
            complex double v1 = evecs[k * 2 + 1];

            /* A*v: [a00*v0 + a01*v1, a10*v0 + a11*v1] */
            complex double Av0 = Ad[0*2+0] * v0 + Ad[0*2+1] * v1;
            complex double Av1 = Ad[1*2+0] * v0 + Ad[1*2+1] * v1;
            /* λ*v */
            complex double lv0 = lambda * v0;
            complex double lv1 = lambda * v1;

            ASSERT_EQ_DBL(creal(Av0), creal(lv0), TOL_SVD, "eig identity A·v=λ·v real (v0)");
            ASSERT_EQ_DBL(cimag(Av0), cimag(lv0), TOL_SVD, "eig identity A·v=λ·v imag (v0)");
            ASSERT_EQ_DBL(creal(Av1), creal(lv1), TOL_SVD, "eig identity A·v=λ·v real (v1)");
            ASSERT_EQ_DBL(cimag(Av1), cimag(lv1), TOL_SVD, "eig identity A·v=λ·v imag (v1)");
        }

        free_a(eig_res.eigenvalues); free_a(eig_res.eigenvectors); free_a(a);
    }

    {
        /* eig of 2x2 non-symmetric — eigenvectors stored column-major */
        Array *a = make_f64_2d((double[]){2, 1, 1, 2}, 2, 2);
        EigResult eig_res = eig(a);
        ASSERT_NOTNULL(eig_res.eigenvalues, "eig 2x2 eigenvalues not null");
        ASSERT_NOTNULL(eig_res.eigenvectors, "eig 2x2 eigenvectors not null");

        /* Eigenvalues are 3 and 1; eigenvectors are column-major.
         * Non-symmetric eig may not sort eigenvalues; just check structural integrity */
        ASSERT_EQ_INT(eig_res.eigenvalues->size, 2, "eig 2x2 has 2 eigenvalues");

        free_a(eig_res.eigenvalues); free_a(eig_res.eigenvectors); free_a(a);
    }

    TEST_SUMMARY();
    return (g_fail > 0) ? 1 : 0;
}
