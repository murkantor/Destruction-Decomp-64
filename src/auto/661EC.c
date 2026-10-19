#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0x40]; s32 x40; s32 x44; u8 pad48[0x74 - 0x48]; s32 x74; u8 pad78[0xB4 - 0x78]; u8 xB4; u8 xB5; } Chan;
typedef struct { u8 pad0[0x40]; s32 id; u8 pad44[0x74 - 0x44]; s32 x74; u8 pad78[0x120 - 0x78]; } Voice;
extern s32 gNumVoices;
extern Voice *gVoices;
s32 Sound_AllocVoice(s32, s32, s32, s32, s32);

u8 *Seq_CmdPlaySound(Chan *c, u8 *p) {
    s32 v;
    s32 id;
    s32 i;
    Voice *vc;

    v = *p++;
    if (v >= 0x80) {
        v = ((v & 0x7F) << 8) + *p++;
    }
    id = Sound_AllocVoice(v, c->xB4, c->xB5, 0, c->x44++);
    c->x44--;
    if (id != 0) {
        vc = gVoices;
        for (i = 0; i < gNumVoices; i++, vc++) {
            if (vc->id == id) {
                vc->id = c->x40;
                vc->x74 = c->x74;
            }
        }
    }
    return p;
}
