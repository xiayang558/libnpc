/**
 * test_utils.c — Shared test framework implementation
 */

#include "test_utils.h"

int g_pass = 0;
int g_fail = 0;

void t_start(const char *name) {
    printf("  %-48s ... ", name);
    fflush(stdout);
}

void t_pass(void) {
    printf("PASS\n");
    g_pass++;
}

void t_fail(const char *fmt, ...) {
    printf("FAIL");
    if (fmt) {
        printf(": ");
        va_list ap;
        va_start(ap, fmt);
        vprintf(fmt, ap);
        va_end(ap);
    }
    printf("\n");
    g_fail++;
}
