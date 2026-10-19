#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0x40]; s32 key; u8 pad44[0x52]; s16 vol; u8 pad98[0x88]; } Voice;
extern s32 gNumVoices;
extern Voice *gVoices;
s32 Sound_SetVolume(s32 key, s16 value) {
    s32 count; s32 i; Voice *p;
    if (key == 0) return 0;
    count = 0; i = 0; p = gVoices;
    while (i < gNumVoices) {
        if (p->key == key) {
            p->vol = value;
            count++;
        }
        i++;
        p++;
    }
    return count;
}
