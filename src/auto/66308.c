#include "common.h"
#include "m2c.h"

typedef struct {
    s32 x0;
    s32 x4;
    s32 x8;
    s32 xC;
    s32 x10;
    s32 x14;
    s32 x18;
    s32 x1C;
    s32 x20;
    s32 x24;
    s32 x28;
    s32 x2C;
    s32 x30;
    s32 x34;
    s32 x38;
    s32 x3C;
} AudioConfig;

typedef struct {
    s32 x0;
    s32 x4;
    s32 x8;
    s32 xC;
    s32 x10;
    void *x14;
    s32 x18;
    u8 x1C;
    s32 x20;
    s32 x24;
} SynCfg;

typedef struct {
    s16 priority;
    s16 fxBus;
    u8 unityPitch;
} VoiceCfg;

typedef struct {
    u8 pad[0xC1];
    u8 xC1;
    u8 pad2[0x120 - 0xC2];
} Voice;

typedef struct {
    u8 pad[0x1C];
} SynVoice;

extern s32 osTvType;
extern s32 D_801260AC;
extern s32 gNumVoices;
extern s32 D_800CCAA4;
extern s32 D_800CCAA8;
extern s32 D_800CCAAC;
extern s32 D_800CCAB0;
extern s32 D_800CCA88;
extern s32 D_800CCA8C;
extern SynVoice *D_800CCA9C;
extern Voice *gVoices;
extern s32 D_800CCAC0;
extern s32 D_800CCAC4;
extern s32 D_800CCAB8;
extern s32 D_800CCABC;
extern s32 sMainStackTop;
extern void *D_800CCA78;
extern void *D_800CCA74;
extern u8 D_8011F2B8[];
extern void Audio_UpdateVoices(void);

void Mem_Fill(void *p, s32 v, s32 n);
void func_8006B110(void *heap, s32 base, s32 len);
void *alHeapDBAlloc(u8 *, s32, void *, s32, s32);
void func_80067368(s32, s32);
void func_80067950(s32, SynCfg *, s32, s32 *, s32, s32, s32);
void func_80065A2C(s32, s16);
void func_8006B840(void *, void *);
void Seq_ResetVoice(Voice *);
void alSynAllocVoice(void *, SynVoice *, VoiceCfg *);

s32 func_80065708(AudioConfig *c) {
    VoiceCfg vc;
    SynCfg sc;
    s32 fx[3];
    s32 i;
    s32 *p;
    s32 t;

    D_801260AC = c->x0;
    gNumVoices = c->x4;
    D_800CCAA4 = c->x20;
    D_800CCAA8 = c->x24;
    if (osTvType == 0) {
        D_800CCAAC = 50;
    } else {
        D_800CCAAC = 60;
    }
    D_800CCAB0 = 1000000 / D_800CCAAC;
    Mem_Fill((void *)c->x10, 0, c->x14);
    func_8006B110(&D_800CCA88, c->x10, c->x14);
    D_800CCA9C = alHeapDBAlloc(0, 0, &D_800CCA88, 1, gNumVoices * sizeof(SynVoice));
    gVoices = alHeapDBAlloc(0, 0, &D_800CCA88, 1, gNumVoices * sizeof(Voice));
    Mem_Fill(D_800CCA9C, 0, gNumVoices * sizeof(SynVoice));
    Mem_Fill(gVoices, 0, gNumVoices * sizeof(Voice));
    func_80067368(c->x18, c->x1C);
    t = c->x18;
    sc.x0 = gNumVoices;
    sc.x4 = gNumVoices;
    D_800CCAC4 = D_800CCAC0 = t;
    sc.x8 = c->x28;
    sc.x10 = 0;
    sc.x1C = 2;
    sc.x18 = 0;
    sc.x14 = &D_800CCA88;
    fx[0] = c->x2C;
    fx[1] = c->x34;
    fx[2] = c->x30;
    func_80067950(c->x8, &sc, c->xC, fx, c->x38, c->x3C, D_800CCAAC);
    func_80065A2C(3, 0x7FFF);
    D_800CCAB8 = 1;
    D_800CCABC = 0x12345678;
    p = &sMainStackTop;
    *p = 0;
    D_800CCA78 = Audio_UpdateVoices;
    D_800CCA74 = p;
    func_8006B840(D_8011F2B8, p);
    for (i = 0; i < gNumVoices; i++) {
        gVoices[i].xC1 = 0;
        Seq_ResetVoice(&gVoices[i]);
        vc.unityPitch = 0;
        vc.priority = c->xC;
        vc.fxBus = 0;
        alSynAllocVoice(D_8011F2B8, &D_800CCA9C[i], &vc);
    }
    return D_800CCA8C - D_800CCA88;
}
