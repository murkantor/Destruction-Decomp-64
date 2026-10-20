#include "common.h"
#include "m2c.h"

void Gfx_SubmitTask(s32, s32, s32);

void Gfx_SubmitFrame(s32 arg0) {
    Gfx_SubmitTask(arg0, 1, 0);
}


void Gfx_SubmitFrameWait(s32 arg0, s32 arg1) {
    Gfx_SubmitTask(arg0, 1, arg1);
}
