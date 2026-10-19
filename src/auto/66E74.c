#include "common.h"
#include "m2c.h"

extern s32 gNumVoices;
extern u8 *gVoices;

s32 func_80066274(s32 key) {
    u8 *p;
    s32 i;

    if (key == 0) {
        return 0;
    }
    p = gVoices;
    for (i = 0; i < gNumVoices; i++) {
        if (*(s32 *)(p + 0x40) == key) {
            return *(s32 *)(p + 0x74);
        }
        p += 0x120;
    }
    return 0;
}
