#ifndef DATETIME64_H
#define DATETIME64_H

#define _GNU_SOURCE

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "array.h"
// #include "_dt_utils.h"

typedef enum {
    DT_UNIT_NS,   // nanosecond
    DT_UNIT_US,   // microsecond
    DT_UNIT_MS,   // millisecond
    DT_UNIT_S,    // second
    DT_UNIT_M,    // minute
    DT_UNIT_H,    // hour
    DT_UNIT_D,    // day
    DT_UNIT_W,    // week
    DT_UNIT_MONTH, // month (simplified)
    DT_UNIT_YEAR   // year (simplified)
} DateTimeUnit;

typedef struct {
    int64_t value;   // nanoseconds since 1970-01-01
    DateTimeUnit unit; // storage unit (for display)
} DateTime64;

// create a datetime64 for the current time
DateTime64 datetime_now(void);

// parse from a string, format such as "2025-04-27T12:34:56.123456789" or "2025-04-27"
// unit is auto-detected from the string or explicitly specified
DateTime64 datetime_from_string(const char* str, DateTimeUnit unit);

// convert to string (ISO 8601 format)
char* datetime_as_string(const DateTime64* dt);

// date arithmetic: add/subtract a specified number of days
DateTime64 datetime_add_days(DateTime64 dt, int64_t days);
DateTime64 datetime_sub_days(DateTime64 dt, int64_t days);

// comparison
bool datetime_eq(DateTime64 a, DateTime64 b);
bool datetime_lt(DateTime64 a, DateTime64 b);
bool datetime_gt(DateTime64 a, DateTime64 b);

// get nanosecond value (unified conversion to nanoseconds)
int64_t datetime_to_ns(DateTime64 dt);


// check whether dates in the array are business days (similar to numpy.is_busday)
// dates: 1-D array of DateTime64 type
// weekmask: string of length 7 indicating whether Mon-Sun are business days ('1' = business day, '0' = rest day), default "1111100" (Sat-Sun off)
// holidays: 1-D array of DateTime64 type for official holidays (optional, can be NULL)
Array* is_busday(Array *dates, const char *weekmask, Array *holidays);



#endif