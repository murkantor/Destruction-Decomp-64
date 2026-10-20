#include "common.h"
#include "m2c.h"

void Gfx_SetViewportScissor(s32, s32, s32);

void Gfx_SetViewportNoClip(s32 arg0, s32 arg1) {
    Gfx_SetViewportScissor(arg0, arg1, 0);
}
