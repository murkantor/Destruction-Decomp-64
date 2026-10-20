#include "common.h"
#include "m2c.h"

s32 Gfx_SetViewportScissor(s32, s32, s32);

/* Returns the display list pointer Gfx_SetViewportScissor returns (Render_Frame uses it) */
s32 Gfx_SetViewportNoClip(s32 arg0, s32 arg1) {
    return Gfx_SetViewportScissor(arg0, arg1, 0);
}
