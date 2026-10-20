#include "common.h"
#include "m2c.h"

s32 Gfx_SubmitTask(s32, s32, s32);

/* Render_Frame checks the result for a failed submit */
s32 Gfx_SubmitFrame(s32 arg0) {
    return Gfx_SubmitTask(arg0, 1, 0);
}


s32 Gfx_SubmitFrameWait(s32 arg0, s32 arg1) {
    return Gfx_SubmitTask(arg0, 1, arg1);
}
