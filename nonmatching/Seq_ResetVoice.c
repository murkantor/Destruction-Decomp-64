typedef struct {
    s32 x0;
    u8 pad4[8];
    s32 xC;
    u8 pad10[0x14];
    f32 x24;
    u8 pad28[0x2C];
    f32 x54;
    f32 x58;
    f32 x5C;
    u8 pad60[8];
    f32 x68;
    u8 pad6C[8];
    s32 x74;
    u8 pad78[0x18];
    u16 x90;
    u16 x92;
    s16 x94;
    u16 x96;
    u16 x98;
    u16 x9A;
    u16 x9C;
    u8 pad9E[2];
    s16 xA0;
    u8 padA2[6];
    u16 xA8;
    u8 padAA[0xA];
    u8 xB4;
    u8 xB5;
    u8 xB6;
    u8 xB7;
    u8 padB8;
    u8 xB9;
    u8 xBA;
    u8 padBB[3];
    u8 xBE;
    u8 xBF;
    u8 xC0;
    u8 xC1;
    u8 padC2[2];
    u8 xC4;
    u8 padC5[5];
    u8 xCA;
    u8 xCB;
} Voice;
extern s32 D_800CCAAC;
extern s32 D_800CCAC0;

static inline void clr(u8 *p, u32 n) {
    u32 i;
    for (i = 0; i < n; i++) {
        *p++ = 0;
    }
}

void Seq_ResetVoice(Voice *v) {
    u8 saved = v->xC1;
    s32 q;

    v->x0 = 0;
    clr((u8 *)v, 0x120);
    v->x24 = 99.9f;
    v->xC4 = 0xFF;
    v->x98 = 0xFFFF;
    v->x68 = 0.03125f;
    v->xB6 = 0xFF;
    q = 0x6000 / D_800CCAAC;
    v->xB5 = 0x40;
    v->xCA = 0;
    v->xB7 = 1;
    v->xCB = 0x7F;
    v->xBE = 1;
    v->xA0 = q;
    v->x54 = 1.0f;
    v->xB4 = 0x7F;
    v->xB9 = 0x7F;
    v->xC1 = saved;
    v->x92 = 1;
    v->x9A = 1;
    v->x9C = 1;
    v->xC = -1;
    v->x94 = q;
    v->x96 = 0x80;
    v->xC0 = 0xF;
    v->xBA = 0x7F;
    v->x58 = 1.0f / 255.0f;
    v->xA8 = 0x80;
    v->x74 = D_800CCAC0;
    v->x90 = 0x80;
    v->xBF = 0xFF;
    v->x5C = 1.0f / 15.0f;
}
