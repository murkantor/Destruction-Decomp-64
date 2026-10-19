#include "common.h"
#include "m2c.h"

extern s32 gNumVoices;
extern u8 *gVoices;
s32 Sound_CountVoices(s32 key) {
    s32 i; u8 *p; s32 count;
    if (key == 0) return 0;
    count = 0; i = 0; p = gVoices;
    while (i < gNumVoices) {
        if (*(s32 *)(p + 0x40) == key) count++;
        i++;
        p += 0x120;
    }
    return count;
}
