#include "common.h"
#include "m2c.h"

typedef struct {
    u8 pad0[4];
    s16 x4;
    u16 x6;
    u8 pad8[4];
    s16 xC;
    s16 xE;
    u8 pad10[6];
    u8 x16;
    u8 x17;
    u8 pad18[0x28];
    s32 x40;
} Sprite;
void guS2DInitBg(Sprite *);

void Sprite_SetSize(Sprite *s, s32 w, s32 h, s32 tw, s32 th) {
    u8 changed = 0;
    Sprite *t = s;

    if (tw != 0 && s->x16 != 0x33) {
        changed = s->x6 != tw;
    }
    t->x4 = w * 4;
    t->xC = h * 4;
    if (tw != 0) {
        t->x6 = tw * 4;
    }
    if (th != 0) {
        t->xE = th * 4;
    }
    if (changed && t->x17 != 3 && !(s->x40 & 1)) {
        guS2DInitBg(s);
    }
}
