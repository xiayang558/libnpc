#include "datetime64.h"
#include <time.h>
#include "_dt_utils.h"

Array* is_busday(Array *dates, const char *weekmask, Array *holidays) {
    if (dates == NULL) {
        fprintf(stderr, "is_busday: NULL dates array\n");
        return NULL;
    }
    if (dates->dtype != DATETIME64) {
        fprintf(stderr, "is_busday: dates must be of type DATETIME64\n");
        return NULL;
    }
    if (dates->ndim != 1) {
        fprintf(stderr, "is_busday: dates must be 1-dimensional\n");
        return NULL;
    }
    // default weekmask is "1111100" (Monday through Friday are business days)
    char mask[8] = "1111100";
    if (weekmask != NULL && strlen(weekmask) == 7) {
        strncpy(mask, weekmask, 7);
    } else if (weekmask != NULL) {
        fprintf(stderr, "is_busday: weekmask must be a 7-character string\n");
        return NULL;
    }

    // create resultboolean array (BOOL type)
    int shape[1] = {dates->size};
    Array *res = create_array(shape, 1, BOOL);
    if (res == NULL) return NULL;
    uint8_t *out = (uint8_t*)res->data;

    DateTime64 *dt_arr = (DateTime64*)dates->data;
    for (int i = 0; i < dates->size; ++i) {
        DateTime64 dt = dt_arr[i];
        // willnanosecondconvertis tm
        struct tm tm = datetime_to_tm(dt);
        int wday = tm.tm_wday;  // 0=Sunday, 1=Monday, ..., 6=Saturday
        // convert so Monday=0, Sunday=6 index (our mask order: Monday to Sunday)
        int idx = (wday == 0) ? 6 : wday - 1;
        bool bus = (mask[idx] == '1');
        if (bus && is_holiday(dt, holidays)) bus = 0;
        out[i] = bus ? 1 : 0;
    }
    return res;
}