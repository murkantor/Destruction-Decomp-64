#include "common.h"
#include "m2c.h"

typedef struct {
    s32 unk0;
    u8 pad4[0x10];
    s32 unk14;
    s32 unk18;
    s32 unk1C;
    s32 unk20;
} Desc;
void func_80060810(Desc *);

Desc *func_80060688(Desc *out, Desc *in) {
    s32 a, b, c, d;
    s32 z;

    func_80060810(in);

    z = in->unk0;
    a = in->unk14;
    b = in->unk18;
    c = in->unk1C;
    d = in->unk20;
    out->unk0 = z;
    out->unk14 = a;
    out->unk18 = b;
    out->unk1C = c;
    out->unk20 = d;
    return out;
}
