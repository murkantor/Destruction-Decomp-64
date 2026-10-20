#include "common.h"
#include "m2c.h"

typedef struct { s8 h; s8 m; s8 s; s32 frac; } TimeHMS;
typedef struct { char s[8]; } Str8;
typedef struct { u8 pad0[4]; s32 x14; u8 pad8[0xC]; s32 x24; s32 x28; f32 x2C; f32 x30; u8 pad24[0x24]; } UiDesc;
typedef struct { s32 a; s32 b; f32 c; f32 d; } UiPos;
typedef struct { UiPos p[2]; } UiPos2;
typedef struct { s32 x0; s32 x4; u64 x8; u64 x10; } Slot;
typedef struct { u64 best; u8 pad8[0x30]; } TrackRecord;
typedef struct { u8 pad[0x68]; } Timer68;

extern Str8 D_800BAF34;
extern UiDesc D_800BAE38;
extern UiPos2 D_800BAE80;
extern void *gUi[];
extern s32 D_800958B4;
extern s32 D_80095FF8[];
extern u64 D_80095E28[];
extern u64 D_80095E48;
extern void *D_8009722C[];
extern Slot D_80130518[];
extern char gSaveSlotText[][0x32];
extern TrackRecord D_80097A58[];
extern s8 D_800D31CC;
extern Timer68 D_800958F8[];
extern f64 D_800BB278;

void Ui_ResetScreen(s32, s32);
void Ui_AddElement(void *, s32, s32);
void Ui_AddSpriteElement(void *, UiDesc *);
TimeHMS Time_Split(u64 t);
void Time_Format(TimeHMS t, char *buf, s32 len);
char *strcpy(char *, const char *);
s32 sprintf(char *, const char *, ...);
s32 atoi(const char *);
void Ui_SetElementColor(void *, s32);
void Announcer_QueueRandom(s32 *list, s32 n, s32 arg, u8 force, u8 unique);
void Msg_Queue(s32, s32, s32, s32);
void Timer_Start(void *, u64);

static inline void addTitle(s32 mode) {
    UiDesc d;
    UiPos2 pos;
    d = D_800BAE38;
    pos = D_800BAE80;
    d.x24 = mode + 0x112D0;
    d.x14 = pos.p[0].a;
    d.x28 = pos.p[0].b;
    d.x2C = pos.p[0].c;
    d.x30 = pos.p[0].d;
    Ui_AddSpriteElement(gUi[0], &d);
    Ui_AddElement(gUi[0], 2, 0x12E);
    d.x24 = mode + 0x112DD;
    d.x14 = pos.p[1].a;
    d.x28 = pos.p[1].b;
    d.x2C = pos.p[1].c;
    d.x30 = pos.p[1].d;
    Ui_AddSpriteElement(gUi[0], &d);
    Ui_AddElement(gUi[0], 2, 0x12F);
}

void Results_ShowLapTimes(void) {
    Str8 buf = D_800BAF34;
    TimeHMS t;
    s32 total = 0;
    s32 mode;
    s32 i;
    u8 found;

    Ui_ResetScreen(0x112F6, 0x112F7);
    Ui_AddElement(gUi[0], 2, 0x23);
    Ui_AddElement(gUi[0], 2, 0x40);
    mode = D_800958B4;
    addTitle(mode);
    Ui_AddElement(gUi[0], 0, 0x17C);
    Ui_AddElement(gUi[0], 0, 0x17D);
    Ui_AddElement(gUi[0], 0, 0x17E);
    Ui_AddElement(gUi[0], 0, 0x17F);
    found = 0;
    for (i = 0; i <= D_80095FF8[mode] - 1; i++) {
        t = Time_Split(D_80095E28[i]);
        Time_Format(t, buf.s, 8);
        strcpy(gSaveSlotText[i + 2], buf.s);
        if (D_80095E28[i] == D_80130518[mode].x8) {
            Ui_SetElementColor(D_8009722C[i], 0x111FC);
            found = 1;
        } else {
            Ui_SetElementColor(D_8009722C[i], 0x111F6);
        }
    }
    t = Time_Split(D_80095E48);
    Time_Format(t, buf.s, 8);
    for (i = 0; i <= D_80095FF8[mode] - 1; i++) {
        total += atoi(&gSaveSlotText[i + 2][7]);
    }
    sprintf(&gSaveSlotText[i + 2][7], "%1d", total);
    strcpy(gSaveSlotText[i + 2], buf.s);
    if (D_80130518[mode].x10 != 0) {
        Ui_SetElementColor(D_8009722C[i], 0x111FC);
        found = 1;
    } else {
        Ui_SetElementColor(D_8009722C[i], 0x111F6);
    }
    if (found) {
        s32 list[3] = { 0xE3, 0xE4, 0xE5 };
        Announcer_QueueRandom(list, 3, 0, 1, 0);
    } else if ((u64)(D_80097A58[mode].best - D_80095E48) <= D_800BB278) {
        Msg_Queue(0xE2, 0, 1, 0);
    }
    for (i = 0; i < D_800D31CC; i++) {
        Timer_Start(&D_800958F8[i], 1000000000);
    }
}
