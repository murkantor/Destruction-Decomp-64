#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0x5C]; s32 x5C; s32 x60; } Screen;
typedef struct { u8 pad[0x8C]; Screen *x8C; } Ctx;
extern u8 D_800BD1F0[];
Ctx *Fb_GetContext(void);
s32 Gfx_LoadTlut(void *, s32);
s32 Font_DrawString(void *, s32, s32, s32, s32);

/* DebugMenu_Draw uses the result Font_DrawString returns */
s32 Font_DrawScaled(s32 a, s32 x, s32 y, s32 d) {
    Ctx *c = Fb_GetContext();
    x = x * (1.0f / 320.0f) * c->x8C->x5C;
    y = y * (1.0f / 240.0f) * c->x8C->x60;
    return Font_DrawString(D_800BD1F0, x, y, d, Gfx_LoadTlut(D_800BD1F0, a));
}
