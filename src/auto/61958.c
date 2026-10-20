#include "common.h"
#include "m2c.h"

void Sprite_InitTlut(u8 *obj, s32 *src) {
    u8 *d = *(u8 **)(obj + 0x20);

    if (src != NULL) {
        *(s32 *)(d + 0x0) = 0x30;
        *(s32 *)(d + 0x4) = src[0];
        *(s16 *)(d + 0x8) = src[1];
        *(s16 *)(d + 0xA) = src[2];
        *(s16 *)(d + 0xC) = 0;
        *(s16 *)(d + 0xE) = 0;
        *(s32 *)(d + 0x10) = -1;
        *(s32 *)(d + 0x14) = 0;
    } else {
        *(s32 *)(d + 0x0) = 0;
        *(s32 *)(d + 0x4) = 0;
    }
}
