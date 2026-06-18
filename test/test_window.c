/**
 * test_window.c — Tests for bartlett, blackman, hamming, hanning, kaiser
 */
#include "test_utils.h"

int main(void) {
    TEST_MAIN("bartlett / blackman / hamming / hanning / kaiser");

    /* ── bartlett M=5 ───────────────────────────────────────────────── */
    {
        Array *w = bartlett(5);
        ASSERT_NOTNULL(w, "bartlett M=5");
        if (w) {
            ASSERT_SIZE(w, 5, "bartlett size");
            ASSERT_DTYPE(w, FLOAT64, "bartlett dtype");
            double *d = (double*)w->data;
            double expected[] = {0.0, 0.5, 1.0, 0.5, 0.0};
            ASSERT_EQ_ARR(d, expected, 5, TOL_F64, "bartlett values");
        }
        free_a(w);
    }

    /* ── bartlett M=1 ───────────────────────────────────────────────── */
    {
        Array *w = bartlett(1);
        ASSERT_NOTNULL(w, "bartlett M=1");
        if (w) {
            ASSERT_SIZE(w, 1, "bartlett M=1 size");
        }
        free_a(w);
    }

    /* ── bartlett M=2 ───────────────────────────────────────────────── */
    {
        Array *w = bartlett(2);
        ASSERT_NOTNULL(w, "bartlett M=2");
        if (w) {
            ASSERT_SIZE(w, 2, "bartlett M=2 size");
            double *d = (double*)w->data;
            /* bartlett(2) should be [0, 0] for endpoint=True symmetric */
            ASSERT_TRUE(d[0] >= 0.0 && d[0] <= 1.0, "bartlett M=2 values in [0,1]");
            ASSERT_TRUE(d[1] >= 0.0 && d[1] <= 1.0, "bartlett M=2 values in [0,1]");
        }
        free_a(w);
    }

    /* ── blackman M=4 ───────────────────────────────────────────────── */
    {
        Array *w = blackman(4);
        ASSERT_NOTNULL(w, "blackman M=4");
        if (w) {
            ASSERT_SIZE(w, 4, "blackman size");
            ASSERT_DTYPE(w, FLOAT64, "blackman dtype");
            double *d = (double*)w->data;
            for (int i = 0; i < 4; i++) {
                ASSERT_TRUE(d[i] >= -1e-15 && d[i] <= 1.0, "blackman value in [0,1]");
            }
        }
        free_a(w);
    }

    /* ── blackman M=1 ───────────────────────────────────────────────── */
    {
        Array *w = blackman(1);
        ASSERT_NOTNULL(w, "blackman M=1");
        if (w) {
            ASSERT_SIZE(w, 1, "blackman M=1 size");
            double *d = (double*)w->data;
            ASSERT_TRUE(d[0] >= -1e-15 && d[0] <= 1.0, "blackman M=1 value");
        }
        free_a(w);
    }

    /* ── hamming M=5 ────────────────────────────────────────────────── */
    {
        Array *w = hamming(5);
        ASSERT_NOTNULL(w, "hamming M=5");
        if (w) {
            ASSERT_SIZE(w, 5, "hamming size");
            ASSERT_DTYPE(w, FLOAT64, "hamming dtype");
            double *d = (double*)w->data;
            /* hamming: 0.08, 0.54, 1.0, 0.54, 0.08 (approx) */
            double expected[] = {0.08, 0.54, 1.0, 0.54, 0.08};
            ASSERT_EQ_ARR(d, expected, 5, 0.02, "hamming values (tol 0.02)");
        }
        free_a(w);
    }

    /* ── hanning M=5 ────────────────────────────────────────────────── */
    {
        Array *w = hanning(5);
        ASSERT_NOTNULL(w, "hanning M=5");
        if (w) {
            ASSERT_SIZE(w, 5, "hanning size");
            ASSERT_DTYPE(w, FLOAT64, "hanning dtype");
            double *d = (double*)w->data;
            double expected[] = {0.0, 0.5, 1.0, 0.5, 0.0};
            ASSERT_EQ_ARR(d, expected, 5, 0.01, "hanning values (tol 0.01)");
        }
        free_a(w);
    }

    /* ── hanning M=1 ────────────────────────────────────────────────── */
    {
        Array *w = hanning(1);
        ASSERT_NOTNULL(w, "hanning M=1");
        if (w) {
            ASSERT_SIZE(w, 1, "hanning M=1 size");
        }
        free_a(w);
    }

    /* ── kaiser M=5 beta=0 ──────────────────────────────────────────── */
    {
        Array *w = kaiser(5, 0.0);
        ASSERT_NOTNULL(w, "kaiser M=5 beta=0");
        if (w) {
            ASSERT_SIZE(w, 5, "kaiser beta=0 size");
            double *d = (double*)w->data;
            double expected[] = {1.0, 1.0, 1.0, 1.0, 1.0};
            ASSERT_EQ_ARR(d, expected, 5, TOL_F64, "kaiser beta=0 all 1s");
        }
        free_a(w);
    }

    /* ── kaiser M=5 beta=5 ──────────────────────────────────────────── */
    {
        Array *w = kaiser(5, 5.0);
        ASSERT_NOTNULL(w, "kaiser M=5 beta=5");
        if (w) {
            ASSERT_SIZE(w, 5, "kaiser beta=5 size");
            ASSERT_DTYPE(w, FLOAT64, "kaiser beta=5 dtype");
            double *d = (double*)w->data;
            for (int i = 0; i < 5; i++) {
                ASSERT_TRUE(d[i] >= 0.0 && d[i] <= 1.0, "kaiser beta=5 value in [0,1]");
            }
        }
        free_a(w);
    }

    /* ── kaiser M=10 beta=8.6 ───────────────────────────────────────── */
    {
        Array *w = kaiser(10, 8.6);
        ASSERT_NOTNULL(w, "kaiser M=10 beta=8.6");
        if (w) {
            ASSERT_SIZE(w, 10, "kaiser M=10 size");
            double *d = (double*)w->data;
            for (int i = 0; i < 10; i++) {
                ASSERT_TRUE(d[i] >= 0.0 && d[i] <= 1.0, "kaiser beta=8.6 value in [0,1]");
            }
        }
        free_a(w);
    }

    /* ── hamming M=10 ───────────────────────────────────────────────── */
    {
        Array *w = hamming(10);
        ASSERT_NOTNULL(w, "hamming M=10");
        if (w) {
            ASSERT_SIZE(w, 10, "hamming M=10 size");
            double *d = (double*)w->data;
            for (int i = 0; i < 10; i++) {
                ASSERT_TRUE(d[i] >= 0.0 && d[i] <= 1.0, "hamming M=10 value in [0,1]");
            }
        }
        free_a(w);
    }

    /* ── blackman M=10 ──────────────────────────────────────────────── */
    {
        /* blackman M=10 endpoints may be slightly negative (~1e-30) */
        Array *w = blackman(10);
        ASSERT_NOTNULL(w, "blackman M=10");
        if (w) {
            ASSERT_SIZE(w, 10, "blackman M=10 size");
            double *d = (double*)w->data;
            for (int i = 0; i < 10; i++) {
                ASSERT_TRUE(d[i] >= -1e-15 && d[i] <= 1.0, "blackman M=10 value in [0,1]");
            }
        }
        free_a(w);
    }

    TEST_SUMMARY();
    return (g_fail > 0) ? 1 : 0;
}
