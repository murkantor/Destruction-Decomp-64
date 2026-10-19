#include "common.h"
#include "m2c.h"

typedef struct { s32 x0; u8 pad4[0x44 - 0x4]; s32 x44; u8 pad48[0x9E - 0x48]; u16 x9E; u8 padA0[0x120 - 0xA0]; } Voice;
extern s32 gNumVoices;
extern Voice *gVoices;
extern s32 *D_800CCAA8;
s32 func_800678AC(Voice *, s32, s32, s32, s32);

s32 Sound_AllocVoice(s32 id, s32 b, s32 c, s32 unique, s32 prio) {
    Voice *v;
    Voice *best;
    s32 i;
    s32 bestPrio;

    if (prio == -1) {
        prio = D_800CCAA8[id];
    }
    if (unique) {
        for (i = 0, v = gVoices; i < gNumVoices; i++, v++) {
            if (v->x9E == id) {
                return func_800678AC(v, id, b, c, prio);
            }
        }
    }
    bestPrio = prio + 1;
    for (i = 0, v = gVoices; i < gNumVoices; i++, v++) {
        if (v->x0 == 0) {
            return func_800678AC(v, id, b, c, prio);
        }
        if (v->x9E != 0 && v->x44 < bestPrio) {
            bestPrio = v->x44;
            best = v;
        }
    }
    if (bestPrio < prio) {
        return func_800678AC(best, id, b, c, prio);
    }
    return 0;
}
