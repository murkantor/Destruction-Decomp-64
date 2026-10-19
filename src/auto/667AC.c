#include "common.h"
#include "m2c.h"

typedef struct {
    s32 x0;
    u8 pad4[0x40];
    s32 x44;
    u8 pad48[0x56];
    u16 x9E;
    u8 padA0[0x80];
} Voice;
extern s32 gNumVoices;
extern Voice *gVoices;
extern s32 *D_800CCAA8;
s32 func_800678AC(Voice *, s32, s32, s32, s32);

s32 Sound_PlayFindVoice(s32 id) {
    s32 i;
    Voice *v;
    Voice *found;
    s32 pri;
    s32 best;

    pri = D_800CCAA8[id];
    v = gVoices;
    best = pri + 1;
    for (i = 0; i < gNumVoices; i++, v++) {
        if (v->x0 == 0) {
            return func_800678AC(v, id, 0x80, 0x80, pri);
        }
        if (v->x9E != 0 && v->x44 < best) {
            best = v->x44;
            found = v;
        }
    }
    if (best < pri) {
        return func_800678AC(found, id, 0x80, 0x80, pri);
    }
    return 0;
}
