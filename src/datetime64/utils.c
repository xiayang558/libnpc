#include "_dt_utils.h"


// helper function: number of nanoseconds equivalent to a single unit
int64_t unit_to_ns(DateTimeUnit unit) {
    switch (unit) {
        case DT_UNIT_NS:   return 1;
        case DT_UNIT_US:   return 1000;
        case DT_UNIT_MS:   return 1000 * 1000;
        case DT_UNIT_S:    return 1000 * 1000 * 1000;
        case DT_UNIT_M:    return 60LL * 1000 * 1000 * 1000;
        case DT_UNIT_H:    return 3600LL * 1000 * 1000 * 1000;
        case DT_UNIT_D:    return 86400LL * 1000 * 1000 * 1000;
        case DT_UNIT_W:    return 7LL * 86400 * 1000 * 1000 * 1000;
        case DT_UNIT_MONTH: return 30LL * 86400 * 1000 * 1000 * 1000; // approximate
        case DT_UNIT_YEAR:  return 365LL * 86400 * 1000 * 1000 * 1000;
        default: return 1;
    }
}

// helper: convert DateTime64 (nanoseconds) to a time struct
struct tm datetime_to_tm(DateTime64 dt) {
    time_t sec = dt.value / 1000000000LL;
    struct tm tm;
    gmtime_r(&sec, &tm);
    return tm;
}

// helper: check if a date is in the holidays list
bool is_holiday(DateTime64 dt, Array *holidays) {
    if (holidays == NULL) return false;
    for (int i = 0; i < holidays->size; ++i) {
        DateTime64 *hd = (DateTime64*)holidays->data + i;
        if (hd->value == dt.value) return true;
    }
    return false;
}