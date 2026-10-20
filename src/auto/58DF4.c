#include "common.h"
#include "m2c.h"

M2C_UNK Gfx_DedupeTexCmds();                            /* extern */
extern s32 D_800BD20C;
extern s32 D_800BD210;
extern s32 D_800BD214;
extern s32 D_800BD218;

void Gfx_RunDedupeTexCmds(void *arg0, s32 arg1) {
    D_800BD20C = 0;
    D_800BD210 = 0;
    D_800BD214 = 0;
    D_800BD218 = 0;
    Gfx_DedupeTexCmds(arg0, arg1);
}
