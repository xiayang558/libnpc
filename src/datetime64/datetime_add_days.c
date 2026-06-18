#include "datetime64.h"
#include <time.h>
#include "_dt_utils.h"


DateTime64 datetime_add_days(DateTime64 dt, int64_t days) {
    int64_t ns_step = days * unit_to_ns(DT_UNIT_D);
    dt.value += ns_step;
    return dt;
}