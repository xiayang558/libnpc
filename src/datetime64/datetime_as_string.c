#include "datetime64.h"
#include <time.h>


char* datetime_as_string(const DateTime64* dt) {
    // convertisnanosecond
    int64_t ns = dt->value;
    time_t sec = ns / 1000000000LL;
    int nsec = ns % 1000000000LL;
    struct tm tm;
    gmtime_r(&sec, &tm);
    char* buf = malloc(64);
    if (nsec > 0)
        sprintf(buf, "%04d-%02d-%02dT%02d:%02d:%02d.%09d",
                tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
                tm.tm_hour, tm.tm_min, tm.tm_sec, nsec);
    else
        sprintf(buf, "%04d-%02d-%02dT%02d:%02d:%02d",
                tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
                tm.tm_hour, tm.tm_min, tm.tm_sec);
    return buf;
}