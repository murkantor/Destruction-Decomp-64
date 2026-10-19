#include "common.h"
#include "m2c.h"

void Debug_DrawPerfStats(s32, s32, s32);

void func_800572C8(s32 arg0) {
    Debug_DrawPerfStats(arg0, 1, 0);
}


void func_800572E8(s32 arg0, s32 arg1) {
    Debug_DrawPerfStats(arg0, 1, arg1);
}
