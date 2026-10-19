#include "common.h"
#include "m2c.h"

typedef struct {
    u8 *x0;
    u8 pad4[0x3C];
    s32 x40;
    s32 x44;
    u8 pad48[0x30];
    u8 *x78;
    u8 pad7C[0x1A];
    s16 x96;
    u8 pad98[6];
    s16 x9E;
    u8 padA0[8];
    s16 xA8;
} Voice;
extern s32 D_800CCAB8;
extern u8 **D_800CCAA4;
void Seq_ResetVoice(Voice *);

s32 func_800678AC(Voice *v, s32 a, s16 b, s16 c, s32 e) {
    Seq_ResetVoice(v);
    v->x9E = a;
    v->x96 = b;
    v->xA8 = c;
    v->x40 = D_800CCAB8;
    v->x44 = e;
    D_800CCAB8++;
    v->x0 = v->x78 = D_800CCAA4[a];
    return v->x40;
}
