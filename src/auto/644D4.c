#include "common.h"
#include "m2c.h"

typedef struct { s8 h; s8 m; s8 s; s32 frac; } TimeHMS;
extern u64 D_800B4D60;
extern u64 D_800B4D68;
static inline TimeHMS Time_Make(s32 secs, s32 rem) {
    TimeHMS r;
    r.h = 0; r.m = 0; r.s = 0; r.frac = 0;
    r.h = secs / 3600;
    secs -= r.h * 3600;
    r.m = secs / 60;
    r.s = secs % 60;
    r.frac = (s64)rem / D_800B4D68;
    return r;
}
TimeHMS Time_Split(u64 t) {
    return Time_Make(t / D_800B4D60, t % D_800B4D60);
}
