/**
 * test_parallel.c — Tests for set_parallel_config, get_parallel_config
 */
#include "test_utils.h"

int main(void) {
    TEST_MAIN("set_parallel_config / get_parallel_config");

    /* ── get_parallel_config ────────────────────────────────────────── */
    {
        ParallelConfig *cfg = get_parallel_config();
        ASSERT_NOTNULL(cfg, "get_parallel_config returns non-NULL");
        if (cfg) {
            ASSERT_TRUE(cfg->num_threads >= 0, "num_threads >= 0");
        }
    }

    /* ── set_parallel_config enable=1 threads=4 ─────────────────────── */
    {
        set_parallel_config(4, 1);
        ParallelConfig *cfg = get_parallel_config();
        ASSERT_NOTNULL(cfg, "get after set(4,1)");
        if (cfg) {
            ASSERT_EQ_INT(cfg->num_threads, 4, "num_threads=4 after set");
            ASSERT_EQ_INT(cfg->enable_parallel, 1, "enable_parallel=1 after set");
        }
    }

    /* ── set_parallel_config enable=0 threads=1 ─────────────────────── */
    {
        set_parallel_config(1, 0);
        ParallelConfig *cfg = get_parallel_config();
        ASSERT_NOTNULL(cfg, "get after set(1,0)");
        if (cfg) {
            ASSERT_EQ_INT(cfg->num_threads, 1, "num_threads=1 after set");
            ASSERT_EQ_INT(cfg->enable_parallel, 0, "enable_parallel=0 after set");
        }
    }

    /* ── set_parallel_config enable=1 threads=2 ─────────────────────── */
    {
        set_parallel_config(2, 1);
        ParallelConfig *cfg = get_parallel_config();
        ASSERT_NOTNULL(cfg, "get after set(2,1)");
        if (cfg) {
            ASSERT_EQ_INT(cfg->num_threads, 2, "num_threads=2 after set");
            ASSERT_EQ_INT(cfg->enable_parallel, 1, "enable_parallel=1 after set");
        }
    }

    /* ── set_parallel_config enable=0 threads=0 ─────────────────────── */
    {
        set_parallel_config(0, 0);
        ParallelConfig *cfg = get_parallel_config();
        ASSERT_NOTNULL(cfg, "get after set(0,0)");
        if (cfg) {
            ASSERT_EQ_INT(cfg->enable_parallel, 0, "enable_parallel=0 after set(0,0)");
        }
    }

    /* ── restore default (1 thread, enabled) ────────────────────────── */
    {
        set_parallel_config(1, 1);
        ParallelConfig *cfg = get_parallel_config();
        ASSERT_NOTNULL(cfg, "get after restore default");
        if (cfg) {
            ASSERT_EQ_INT(cfg->num_threads, 1, "num_threads restored to 1");
            ASSERT_EQ_INT(cfg->enable_parallel, 1, "enable_parallel restored to 1");
        }
    }

    /* ── edge: rapid toggling ───────────────────────────────────────── */
    {
        set_parallel_config(1, 0);
        set_parallel_config(8, 1);
        set_parallel_config(1, 1);
        ParallelConfig *cfg = get_parallel_config();
        ASSERT_NOTNULL(cfg, "get after rapid toggling");
        if (cfg) {
            ASSERT_EQ_INT(cfg->num_threads, 1, "rapid toggle final num_threads");
            ASSERT_EQ_INT(cfg->enable_parallel, 1, "rapid toggle final enable");
        }
    }

    TEST_SUMMARY();
    return (g_fail > 0) ? 1 : 0;
}
