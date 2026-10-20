#include "common.h"
#include "m2c.h"

typedef struct { s8 h; s8 m; s8 s; s32 frac; } TimeHMS;

typedef struct {
    u8 pad0[0x1074];
    u8 x1074;
    u8 pad1075[0x10A8 - 0x1075];
    u64 x10A8;
    u8 pad10B0[0x10FC - 0x10B0];
    s16 x10FC;
} Game;

typedef struct {
    u8 pad0[0xD];
    u8 xD;
    u8 padE[0x50 - 0xE];
    u64 x50;
} Racer;

typedef struct {
    Racer *x0;
    u8 pad4[0x68 - 0x4];
} RacerSlot;

extern Game *gGame[];
extern u8 D_8007EED0[];
extern RacerSlot D_800958F0[];
extern char D_8007F834[][3];
extern char D_800B81A0[];
extern f64 D_800B8250;
extern f64 D_800B8258;
extern f64 D_800B8260;
u64 Time_Get(u64 *);
TimeHMS Time_Split(u64 t);
s32 sprintf(char *, const char *, ...);
void Ui_SetElementColor(s32, s32);

s32 Hud_UpdateRacerTimers(s32 a0, s8 idx) {
    u64 now;
    u64 rem;
    TimeHMS ts;
    RacerSlot *slot;
    Racer *r;

    if (gGame[0]->x10FC == 0x1B && D_8007EED0[idx] != 0) {
        D_8007EED0[idx] = 0;
    }
    if (gGame[0]->x10FC == 0x1F) {
        return 0;
    }
    slot = &D_800958F0[idx]; /* unused, but needed to match */
    now = Time_Get(&gGame[0]->x10A8);
    r = D_800958F0[idx].x0;
    rem = r->x50 - now;
    if (gGame[0]->x10FC == 0x1B) {
        rem = r->x50;
    }
    if (!(now < r->x50 && r->xD == 0)) {
        rem = 0;
    }
    if ((f64)rem > (f64)now + D_800B8250) {
        D_800958F0[idx].x0->x50 = (f64)now + D_800B8258;
        rem = D_800958F0[idx].x0->x50;
    }
    ts = Time_Split(rem);
    sprintf(D_8007F834[idx], D_800B81A0, ts.s + ts.m * 60);
    if (ts.s < 10 && ts.m == 0) {
        D_8007F834[idx][0] = '0';
    }
    D_8007F834[idx][2] = 0;
    if (gGame[0]->x1074 != 0) {
        return 0;
    }
    if (rem != 0 && (f64)rem < D_800B8260) {
        if (D_8007EED0[idx] == 0) {
            D_8007EED0[idx] = 1;
            Ui_SetElementColor(idx + 0x200, 0x111D4);
        } else {
            D_8007EED0[idx] = 0;
            Ui_SetElementColor(idx + 0x200, 0x111D6);
        }
    } else if (D_8007EED0[idx] != 0) {
        D_8007EED0[idx] = 0;
        Ui_SetElementColor(idx + 0x200, 0x111D6);
    }
    return 0;
}
