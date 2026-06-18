#include "datetime64.h"
#include <time.h>


DateTime64 datetime_now(void) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    DateTime64 dt;
    dt.value = (int64_t)ts.tv_sec * 1000000000LL + ts.tv_nsec;
    dt.unit = DT_UNIT_NS;
    return dt;
}