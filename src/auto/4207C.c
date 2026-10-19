#include "common.h"
#include "m2c.h"

typedef struct { s8 h; s8 m; s8 s; s32 frac; } TimeHMS;

typedef struct {
    s32 x0;
    u8 pad4[0x8 - 0x4];
    u64 x8;
    u64 x10;
    u8 pad18[0x38 - 0x18];
} Record;

extern s32 D_800958A8;
extern s32 D_800958AC;
extern s32 D_80097CD8;
extern s32 D_80097CEC;
extern s32 D_80097D00;
extern s32 D_80097D14;
extern Record D_80097A48[];
extern char gSaveSlotText[][0x32];
TimeHMS Time_Split(u64 t);
void Time_Format(TimeHMS t, char *buf, s32 len);
char *strcpy(char *, const char *);
s32 sprintf(char *, const char *, ...);

void func_8004147C(s32 idx) {
    char empty[] = "";
    char buf[] = "-:--:--";
    TimeHMS t;
    Record *r;

    if (D_800958A8 == 3 && (D_800958AC == 1 || D_800958AC == 2)) {
        if (D_800958AC == 1) {
            sprintf(gSaveSlotText[0], "%d", D_80097CD8);
            strcpy(gSaveSlotText[1], empty);
            strcpy(gSaveSlotText[2], empty);
        } else {
            sprintf(gSaveSlotText[0], "%d", D_80097CEC);
            sprintf(gSaveSlotText[1], "%d", D_80097D00);
            sprintf(gSaveSlotText[2], "%d", D_80097D14);
        }
        return;
    }
    r = &D_80097A48[idx];
    sprintf(gSaveSlotText[0], "%d", r->x0);
    if (idx < 4) {
        strcpy(gSaveSlotText[1], empty);
        strcpy(gSaveSlotText[2], empty);
    } else {
        t = Time_Split(r->x8);
        Time_Format(t, buf, 8);
        strcpy(gSaveSlotText[1], buf);
        t = Time_Split(r->x10);
        Time_Format(t, buf, 8);
        strcpy(gSaveSlotText[2], buf);
    }
}
