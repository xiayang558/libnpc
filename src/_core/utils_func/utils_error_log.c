#include <stdio.h>
#include <stdarg.h>
#include "array.h"
void utils_error_log(const char* level, const char* func, const char* msg, ...) {
    fprintf(stderr, "[%s] ", level);
    if (func) fprintf(stderr, "%s: ", func);
    va_list ap; va_start(ap, msg); vfprintf(stderr, msg, ap); va_end(ap);
    fprintf(stderr, "\n");
}
