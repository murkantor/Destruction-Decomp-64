typedef struct {
    u8 pad[0x1C];
} ALVoice;

typedef struct {
    u8 pad0[0xC];
    s32 xC;
    u8 pad10[8];
    u32 x18;
    u8 pad1C[0x7A];
    s16 x96;
    u16 x98;
    u8 pad9A[4];
    u16 x9E;
    u8 padA0[8];
    s16 xA8;
    u8 padAA[9];
    u8 xB3;
    u8 xB4;
    u8 xB5;
    u8 xB6;
    u8 padB7[5];
    u8 xBC;
} Snd66950;

extern u8 D_8011F2B8[];
extern ALVoice *D_800CCA9C;
extern s32 D_800CCAB0;
extern u16 D_800CCAB4;
extern u16 D_800CCAB6;

void alSynSetVol(void *, ALVoice *, s16, s32);
void alSynSetPan(void *, ALVoice *, u8);

static inline void SetVol(Snd66950 *s, s32 idx, u16 vol) {
    s->x98 = vol;
    alSynSetVol(D_8011F2B8, &D_800CCA9C[idx], vol, D_800CCAB0);
}

static inline void SetPan(Snd66950 *s, s32 idx, u8 pan) {
    s->xB6 = pan;
    alSynSetPan(D_8011F2B8, &D_800CCA9C[idx], pan);
}

void func_80066950(Snd66950 *s, s32 idx) {
    u32 vol;

    vol = (u32)(s->xB4 * s->xBC * s->xB3 * s->x96) >> 13;
    if (vol > 0x7FFF) {
        vol = 0x7FFF;
    }
    vol *= (s->x9E == 0 ? D_800CCAB6 : D_800CCAB4);
    vol >>= 15;
    if (s->xC != -1) {
        vol = vol * s->xC / s->x18;
    }
    if (vol != s->x98) {
        SetVol(s, idx, vol);
    }
    if (s->xB5 != s->xB6) {
        SetPan(s, idx, (s->xB5 * s->xA8 >> 7) & 0x7F);
    }
}
