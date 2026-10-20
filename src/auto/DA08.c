#include "common.h"
#include "m2c.h"

typedef struct { s8 h; s8 m; s8 s; s32 frac; } TimeHMS;
typedef struct { u8 pad[0x10]; s8 x10; } Unk3C;
typedef struct { u8 pad[0x30]; s32 x30; u8 pad34[0x3C - 0x34]; Unk3C *x3C; } Player;
typedef struct { Player *x0; u8 pad4[0xC]; u64 x10; } Lead;
typedef struct {
    u8 pad0[0x1074];
    u8 paused;
    u8 pad1075[0x10A8 - 0x1075];
    u64 x10A8;
    u8 pad10B0[0x10FC - 0x10B0];
    s16 x10FC;
} Game;
typedef struct { s32 x0; } Obj;
extern Game *gGame[];
extern Lead *D_80096074;
extern u8 D_8007EED0[];
extern char D_8007F83C[];
extern char D_8007F83E;
extern char D_800B81A0[];
extern f64 D_800B81F8;
u64 Time_Get(u64 *);
TimeHMS Time_Split(u64 t);
void Ui_SetElementColor(s32, s32);
int sprintf(char *, const char *, ...);

s32 Hud_UpdateCountdown(Obj *o, s8 id) {
    u64 rem;
    TimeHMS hms;
    s16 mode = gGame[0]->x10FC;

    if (mode == 0x1A) {
        if (D_8007EED0[0]) {
            D_8007EED0[0] = 0;
        }
        return 0;
    }
    if (gGame[0]->paused) {
        return 0;
    }
    if (D_80096074->x0 == NULL) {
        return 0;
    }
    if (mode != 0x1B) {
        rem = D_80096074->x10 - Time_Get(&gGame[0]->x10A8);
    } else {
        rem = D_80096074->x10;
    }
    if (D_80096074->x0->x30 != 1 && D_80096074->x0->x3C->x10 == id && (s32)(rem * D_800B81F8) < 6) {
        if (D_8007EED0[0] != 0) {
            D_8007EED0[0] = 0;
            Ui_SetElementColor(o->x0, 0x111D6);
        } else {
            D_8007EED0[0] = 1;
            Ui_SetElementColor(o->x0, 0x111D4);
        }
    } else {
        Ui_SetElementColor(o->x0, 0x111D6);
    }
    hms = Time_Split((s32)(rem / 1000000000) > 0 ? rem : 0);
    sprintf(D_8007F83C, D_800B81A0, hms.s);
    if (hms.s < 10) {
        D_8007F83C[0] = '0';
    }
    D_8007F83E = 0;
    return 0;
}
