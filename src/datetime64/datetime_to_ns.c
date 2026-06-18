#include "datetime64.h"
#include <time.h>


int64_t datetime_to_ns(DateTime64 dt) {
    return dt.value;
}