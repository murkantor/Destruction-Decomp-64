#include "common.h"
#include "m2c.h"

extern u64 gGameTime; extern u64 D_801244D8; u64 Time_Get(u64 *); s32 Game_TimeBefore(void) { if (Time_Get(&gGameTime) < D_801244D8) return 1; return 0; }
