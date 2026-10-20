#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0x1C]; } ALVoice;
typedef struct { u8 b0; u8 b1; u8 b2; u8 b3; } DrumEnt;
typedef struct { u8 pad[0x10]; u8 *x10; } Bank70;
typedef struct { u8 pad[0x28]; f32 *x28; s32 *x2C; } Inst74;
typedef struct Voice {
    u8 *x0;
    s32 x4;
    s32 x8;
    u8 padC[0x28 - 0xC];
    f32 x28;
    u8 pad2C[0x38 - 0x2C];
    u32 x38;
    u32 x3C;
    u8 pad40[0x48 - 0x40];
    f32 x48;
    f32 x4C;
    s32 x50;
    u8 pad54[0x70 - 0x54];
    Bank70 *x70;
    Inst74 *x74;
    u8 pad78[4];
    DrumEnt *x7C;
    u8 pad80[0x92 - 0x80];
    u16 x92;
    u8 pad94[0x98 - 0x94];
    u16 x98;
    u8 pad9A[0xA2 - 0x9A];
    u16 xA2;
    u16 xA4;
    u16 xA6;
    u8 padA8[0xAF - 0xA8];
    u8 xAF;
    u8 padB0[3];
    u8 xB3;
    u8 padB4;
    u8 xB5;
    u8 padB6[0xBB - 0xB6];
    u8 xBB;
    u8 xBC;
    u8 xBD;
    u8 padBE[0xC1 - 0xBE];
    u8 xC1;
    u8 xC2;
    u8 xC3;
    u8 xC4;
    u8 xC5;
    u8 xC6;
    u8 xC7;
    u8 xC8;
    u8 xC9;
    u8 xCA;
    u8 xCB;
    u8 xCC;
    u8 xCD;
    u8 xCE;
    u8 xCF;
} Voice;

typedef u8 *(*SeqCmd)(Voice *, u8 *);
extern SeqCmd D_800B4D90[];
extern ALVoice *D_800CCA9C;
extern u8 D_8011F2B8[];
extern s32 D_800CCAB0;

void Seq_ReadEnvelope(Voice *, u8 *);
void Env_Start(Voice *);
void Snd_StartAutoPan(Voice *);
void func_800668C4(Voice *, s32);
void alSynSetVol(void *, ALVoice *, s16, s32);
void alSynSetFXMix(void *, ALVoice *, u8);
void alSynStopVoice(void *, ALVoice *);

#define READ_DELTA(v) { c = *(v)->x0++; if (c < 0x80) { (v)->x92 = c; } else { (v)->x92 = *(v)->x0++ + ((c & 0x7F) << 8); } }

void func_80066530(Voice *v, s32 idx) {
    u8 *p;
    s32 note;
    Inst74 *inst;
    u16 k;
    u8 m;
    u8 c;

    p = v->x0;
    if (p != NULL) {
    loop:
        c = *p;
        if (c >= 0x80) {
            p = D_800B4D90[c & 0x7F](v, p + 1);
            if (p != NULL) {
                goto loop;
            }
        }
    }
    v->x0 = p;
    if (p != NULL) {
        v->x48 = v->x4C;
        note = *v->x0++;
        if (v->xCA) {
            v->xB3 = *v->x0++;
        } else {
            v->xB3 = v->xCB;
        }
        if (v->xA4 != 0) {
            if (v->xAF != 0) {
                v->xAF = 0;
                READ_DELTA(v);
            } else {
                v->x92 = v->xA4;
            }
        } else {
            READ_DELTA(v);
        }
        v->x3C = v->x38;
        v->xA2 = 0;
        v->xC9 = 0;
        v->x38 += v->x92 << 8;
        v->xC8 = v->xC7;
        if (note != 0x60) {
            inst = v->x74;
            if (v->x7C != NULL) {
                v->xA6 = v->x7C[note].b0;
                v->xB5 = v->x7C[note].b2 >> 1;
                Seq_ReadEnvelope(v, v->x70->x10 + v->x7C[note].b1 * 7);
                note = v->x7C[note].b3;
            }
            if (v->xCE == 0) {
                Env_Start(v);
            }
            if (v->xCC) {
                Snd_StartAutoPan(v);
            }
            k = v->xA6;
            if (v->xCF == 0) {
                v->x4 = inst->x2C[k];
                if (v->xC1 && v->x98) {
                    v->x98 = 0;
                    alSynSetVol(D_8011F2B8, &D_800CCA9C[idx], 0, D_800CCAB0);
                } else {
                    func_800668C4(v, idx);
                }
            }
            v->x28 = note + inst->x28[k];
            m = v->xC2;
            if (m != v->xC4) {
                v->xC4 = m;
                alSynSetFXMix(D_8011F2B8, &D_800CCA9C[idx], v->xC3 + ((0x80 - v->xC3) * m >> 7));
            }
        } else if (v->xBB < 4) {
            v->xBB = 4;
            v->xBD = 1;
            v->x50 = v->x8;
            v->xC5 = v->xBC;
        }
    } else if (v->xC1) {
        v->xC1 = 0;
        alSynSetVol(D_8011F2B8, &D_800CCA9C[idx], 0, D_800CCAB0);
        alSynStopVoice(D_8011F2B8, &D_800CCA9C[idx]);
    }
}
