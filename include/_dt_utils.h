#include "datetime64.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>


int64_t unit_to_ns(DateTimeUnit unit);

struct tm datetime_to_tm(DateTime64 dt);
bool is_holiday(DateTime64 dt, Array *holidays);