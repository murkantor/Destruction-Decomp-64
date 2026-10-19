#include "common.h"
#include "m2c.h"

typedef struct { s8 h; s8 m; s8 s; s32 frac; } TimeHMS;
typedef struct {
    u8 pad0[0x60];
    struct { u64 t; } x60[4];
    u64 x80;
    s32 x88;
} Racer;
typedef struct {
    Racer *r;
    u8 pad4[0x64];
} RacerSlot;
typedef struct { u64 x0; u8 pad8[0x10]; } BestTime;
typedef struct {
    u8 pad0[0xC];
    s16 xC;
    u8 padE[0x10A8 - 0xE];
    u64 x10A8;
    u8 pad10B0[0x10FC - 0x10B0];
    s16 x10FC;
} Game;

extern Game *gGame;
extern void *gUi[];
extern RacerSlot D_800958F0[];
extern u8 D_8007EF30[];
extern char D_8007F714[][3][9];
extern char D_80124298[][3][3];
extern u8 D_80097335;
extern s32 D_800958B4;
extern BestTime D_80130520[];
extern s8 D_800D31CC;
extern s8 D_800D31CC_v __asm__("D_800D31CC");
extern s32 D_80095FF8[];

TimeHMS Time_Split(u64 t);
void Time_Format(TimeHMS t, char *buf, s32 len);
u64 Time_Get(u64 *);
char *strcpy(char *, const char *);
char *strcat(char *, const char *);
s32 sprintf(char *, const char *, ...);
void *func_8001007C(char *);
void func_800100BC(void *, s32);
void Ui_AddElement(void *, s32, s32);
void func_8005DBDC(void *);

s32 Hud_UpdateLapTimes(void *arg0, s8 player) {
    char buf[16];
    TimeHMS t;
    u64 time = 0;
    char dot[] = ".";
    s32 color = -1;
    s32 ids[2] = { 0xAA, 0xAB };
    Racer *r;
    s8 lap;
    s8 i;
    s8 n;
    s8 k;
    u8 best;

    if (gGame->xC != 2) {
        return 0;
    }
    if (gGame->x10FC == 0x1F || gGame->x10FC == 0x21) {
        return 0;
    }
    r = D_800958F0[player].r;
    if (r->x88 < 0) {
        lap = 1;
        D_8007EF30[0] = 1;
        for (k = 1; k < 3; k++) {
            D_8007EF30[k] = 0;
        }
    } else {
        lap = r->x88 + 1;
    }
    for (i = 0; i < 3; i++) {
        if (lap - 1 < 3) {
            n = i + 1;
        } else {
            n = lap + (s8)(i - 3);
        }
        sprintf(D_80124298[player][i], "%1d", n);
        strcat(D_80124298[player][i], dot);
        if (r->x60[n].t != 0) {
            time = r->x60[n].t;
            best = 0;
            if (D_80097335) {
                if (D_80130520[D_800958B4].x0 != 0) {
                    best = 1;
                    k = 0;
                    if (k < D_800D31CC_v) {
                        do {
                            if (D_800958F0[k].r->x80 < r->x60[n].t) {
                                best = 0;
                                break;
                            }
                            k++;
                        } while (k < D_800D31CC);
                    }
                }
            }
            if (best) {
                for (k = i - 1; k >= 0; k--) {
                    func_800100BC(func_8001007C(D_8007F714[player][k]), 0x111F8);
                }
                color = 0x111FC;
            } else {
                color = 0x111F8;
            }
            if (i < 2 && D_8007EF30[i + 1] == 0) {
                Ui_AddElement(gUi[0], 2, ids[i]);
                Ui_AddElement(gUi[0], 0, (s32)func_8001007C(D_80124298[player][i + 1]));
                Ui_AddElement(gUi[0], 0, (s32)func_8001007C(D_8007F714[player][i + 1]));
                D_8007EF30[i + 1] = 1;
            }
        } else if (n == lap) {
            if (gGame->x10FC != 0x1B) {
                time = Time_Get(&gGame->x10A8) - r->x60[0].t;
            } else {
                time = 0;
            }
            color = 0x111F6;
        }
        t = Time_Split(time);
        Time_Format(t, buf, 9);
        if (n <= lap) {
            strcpy(D_8007F714[player][i], buf);
        } else {
            D_8007F714[player][i][0] = 0;
        }
        if (D_8007EF30[i]) {
            func_800100BC(func_8001007C(D_8007F714[player][i]), color);
        }
    }
    if (r->x88 >= D_80095FF8[D_800958B4]) {
        func_8005DBDC(arg0);
    }
    return 0;
}

/* fills the rest of this file's 8-byte rodata block */
const s32 D_800B8244 = 0;
