#include "common.h"
#include "m2c.h"

typedef struct {
    u16 *src;
    s32 unk4;
    u8 unk8;
    u8 unk9;
    u8 unkA;
    s32 unkC;
    s32 unk10;
    s32 unk14;
    s32 unk18;
    s32 unk1C;
    s32 unk20;
    f32 unk24;
} Seq;

u16 func_8005AA68(Seq *s, u16 *src) {
    s->unk8 = 1;
    s->src = src;
    s->unk4 = 0;
    s->unkC = 0;
    s->unk10 = 0;
    s->unk14 = 0;
    s->unk18 = 0;
    s->unk1C = 0;
    s->unk20 = 0;
    s->unk9 = 0xFF;
    s->unkA = 0;
    s->unk24 = 1.0f;
    if (src != NULL) {
        return *src;
    }
    return 0;
}
