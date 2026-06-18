#include "datetime64.h"
#include <time.h>


bool datetime_gt(DateTime64 a, DateTime64 b) {
    return a.value > b.value;
}