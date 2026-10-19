#include "common.h"
#include "m2c.h"

typedef struct {
    s32 x0;
    s32 x4;
    u32 x8;
    s32 xC;
    u8 pad10[0x14 - 0x10];
    u32 x14;
    u8 pad18[0x2C - 0x18];
    f32 x2C;
    s32 x30;
    s32 x34;
    u32 x38;
    u32 x3C;
    u8 pad40[0x8C - 0x40];
    u32 x8C;
    u8 pad90[0x92 - 0x90];
    u16 x92;
    u16 x94;
    u8 pad96[0xA2 - 0x96];
    u16 xA2;
    u8 padA4[0xBB - 0xA4];
    u8 xBB;
    u8 padBC[0xC1 - 0xBC];
    u8 xC1;
    u8 padC2[0xC6 - 0xC2];
    u8 xC6;
    u8 padC7[0xCC - 0xC7];
    u8 xCC;
    u8 xCD;
    u8 padCE[0x120 - 0xCE];
} Voice;

typedef struct {
    u8 pad[0x1C];
} ALVoice;

extern s32 gNumVoices;
extern Voice *gVoices;
extern ALVoice *D_800CCA9C;
extern u8 D_8011F2B8[];
extern s32 D_800CCAB0;
void func_800668C4(Voice *, s32);
void func_80066530(Voice *, s32);
void Seq_AdvanceDelta(Voice *);
void Envelope_Step(Voice *);
s32 func_80064D90(Voice *, s32);
void alSynStopVoice(void *, ALVoice *);
void Env_Update(Voice *);
void Snd_UpdateVolumeRamp(Voice *);
f32 func_80066F7C(Voice *);
f32 func_80066F24(Voice *);
void AudioVoice_UpdatePitch(Voice *, s32, f32);
void func_80066950(Voice *, s32);

s32 Audio_UpdateVoices(void) {
    Voice *v = gVoices;
    s32 i;
    f32 pitch;

    for (i = 0; i < gNumVoices; i++, v++) {
        if (v->x0 == 0) {
            continue;
        }
        if (v->x4 != 0) {
            func_800668C4(v, i);
        }
        v->x8 += v->x94;
        if (v->x92 != 0x7FFF) {
            while (v->x38 < v->x8) {
                if (v->x0 == 0) {
                    break;
                }
                func_80066530(v, i);
            }
            if (v->x0 == 0) {
                continue;
            }
        }
        if (v->x34 != 0 && v->x14 < v->x8) {
            Seq_AdvanceDelta(v);
        }
        if (v->x30 != 0 && v->x14 < v->x8) {
            Envelope_Step(v);
        }
        if (v->xC != -1 && --v->xC == -1) {
            v->x0 = func_80064D90(v, 0);
            if (v->xC1) {
                v->xC1 = 0;
                alSynStopVoice(D_8011F2B8, &D_800CCA9C[i]);
            }
        }
        if (v->xC1) {
            if (v->xBB) {
                Env_Update(v);
            }
            if (v->xCC && v->x8C < v->x8) {
                Snd_UpdateVolumeRamp(v);
            }
            pitch = v->x2C;
            if (v->xCD) {
                pitch += func_80066F7C(v);
            }
            if (v->xC6) {
                pitch += func_80066F24(v);
            }
            if (v->x4 == 0) {
                AudioVoice_UpdatePitch(v, i, pitch);
                func_80066950(v, i);
            }
        }
        v->xA2 = (v->x8 - v->x3C) >> 8;
    }
    return D_800CCAB0;
}
