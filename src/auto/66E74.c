#include "common.h"
#include "m2c.h"

extern s32 D_800CCA98;
extern u8 *D_800CCAA0;

s32 func_80066274(s32 key) {
    u8 *p;
    s32 i;

    if (key == 0) {
        return 0;
    }
    p = D_800CCAA0;
    for (i = 0; i < D_800CCA98; i++) {
        if (*(s32 *)(p + 0x40) == key) {
            return *(s32 *)(p + 0x74);
        }
        p += 0x120;
    }
    return 0;
}
