#include "datetime64.h"
#include <time.h>


// simple string parser (supports "YYYY-MM-DD" and "YYYY-MM-DDThh:mm:ss" etc.)
DateTime64 datetime_from_string(const char* str, DateTimeUnit unit) {
    DateTime64 dt = {0, unit};
    int year, month, day, hour = 0, min = 0, sec = 0, nsec = 0;
    char t;
    int n = sscanf(str, "%d-%d-%d%c%d:%d:%d.%d", &year, &month, &day, &t, &hour, &min, &sec, &nsec);
    if (n < 3) {
        fprintf(stderr, "Invalid datetime string: %s\n", str);
        return dt;
    }
    // convertis struct tm
    struct tm tm = {0};
    tm.tm_year = year - 1900;
    tm.tm_mon = month - 1;
    tm.tm_mday = day;
    tm.tm_hour = hour;
    tm.tm_min = min;
    tm.tm_sec = sec;
    time_t epoch = timegm(&tm); // note: use timegm instead of mktime to avoid timezone offset
    dt.value = (int64_t)epoch * 1000000000LL + nsec;
    return dt;
}
