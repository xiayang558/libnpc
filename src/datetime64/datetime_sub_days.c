#include "datetime64.h"
#include <time.h>


DateTime64 datetime_sub_days(DateTime64 dt, int64_t days) {
    return datetime_add_days(dt, -days);
}