#include "common.h"
#include "m2c.h"

extern s32 D_800B0B58;

s32 Fb_GetLastFramebuffer(void) {
    return D_800B0B58;
}
