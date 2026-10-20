#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0x30]; s32 x30; } Player;
typedef struct { u8 pad[4]; s32 x4; s32 x8; s32 xC; s32 x10; u8 pad14[0x74 - 0x14]; Player *x74; } Race;
typedef struct {
    u8 pad[0x10A8]; u64 x10A8; u8 pad10B0[0x10C9 - 0x10B0]; s8 x10C9; s8 x10CA;
    u8 pad10CB[0x1100 - 0x10CB]; Race *x1100;
} Game;
typedef struct { u8 pad[0x8]; void *x8; } Slot;
typedef struct { u8 pad[0x38]; s32 x38; } Car;
typedef struct { u8 pad[0x10]; s32 x10; u8 pad14[0x27 - 0x14]; u8 x27; } Obj;
extern Game *gGame;
extern void *gUi[];
extern u64 D_80097A00;
extern s32 D_800958A8;
extern s32 D_800958AC;
extern s32 D_800958B4;
extern s32 D_800958A4;
extern s32 D_80095FF8[];
extern s8 D_800D31CC;
extern s16 D_8011EEEA;
extern u64 D_800960A0;
extern u64 D_800960A8;
extern Slot *D_80096078;
extern Slot *D_8009607C;
extern Car *D_80096080;
extern Car *D_80096084;
extern void *D_800959C0;
extern void *D_800958F0;
extern u8 D_80091B14[];
extern u8 D_800970EC;
extern char D_80123D1E[];
extern char D_800BAB20[];
extern char D_800BAB2C[];
extern const f64 D_800BAB38;
extern const f64 D_800BAB40;
u64 Time_Get(void *);
void Race_AwardPot(void);
void Ui_AddElement(void *, s32, s32);
void Ui_RemoveElement(void *, s32);
void Ui_SetElementPos(void *, s32, s32, f32, f32);
u8 Obj_HasValue(void *, void *);
void func_8005F884(void *);
void func_80063380(s32);
void Game_SetNextMode(s32, s32);
s32 sprintf(char *, const char *, ...);

s32 Race_CheckFinishHud(Obj *obj) {
    Race *race = gGame->x1100;
    s32 ret = 0;
    s8 a;
    s8 b;
    s32 i;
    f32 x;

    if (race->x10 == 1) {
        if (race->x8 == 0 || (race->x8 == 1 && race->x4 == 0)) {
            if (D_80097A00 == 0) {
                Race_AwardPot();
                if (D_800958A8 == 3 && D_800958AC == 1) {
                    D_80097A00 = Time_Get(&gGame->x10A8);
                } else {
                    D_80097A00 = (f64)Time_Get(&gGame->x10A8) + D_800BAB38;
                }
            }
        }
    } else {
        if (race->xC == D_80095FF8[D_800958B4] * D_800D31CC || race->x8 == 0) {
            if (D_80097A00 == 0) {
                Race_AwardPot();
                if (D_800958A8 == 2) {
                    gGame->x1100->x74->x30 = 1;
                }
                D_80097A00 = (f64)Time_Get(&gGame->x10A8) + D_800BAB38;
            }
        }
    }
    if (D_8011EEEA != 0x1D && D_80097A00 != 0) {
        if (D_80097A00 < Time_Get(&gGame->x10A8)) {
            obj->x10 = 1;
            Game_SetNextMode(0x1D, 1);
            ret = 1;
        }
    }
    if (obj->x27) {
        a = -1;
        b = -1;
        if (D_800960A0 != 0) {
            Ui_RemoveElement(gUi[0], 0x134);
            Ui_RemoveElement(gUi[0], 0x136);
        }
        if (D_800960A8 != 0) {
            Ui_RemoveElement(gUi[0], 0x135);
            Ui_RemoveElement(gUi[0], 0x138);
        }
        if (D_80096078 && D_80096078->x8 && Obj_HasValue(D_80096084, D_80096078->x8)) {
            Ui_RemoveElement(gUi[0], 0x135);
            a = 0;
        }
        if (D_8009607C && D_8009607C->x8 && Obj_HasValue(D_80096080, D_8009607C->x8)) {
            Ui_RemoveElement(gUi[0], 0x134);
            b = 0;
        }
        for (i = 0; i < D_800958A4; i++) {
            if (D_80091B14[i]) {
                Ui_RemoveElement(gUi[0], i + 0xE2);
            }
        }
        func_8005F884(gUi[0]);
        if (D_800960A0 != 0) {
            Ui_AddElement(gUi[0], 2, 0x136);
            Ui_AddElement(gUi[0], 2, 0x134);
            Ui_SetElementPos(gUi[0], 2, 0x134, 143.0f, 20.0f);
        }
        if (D_800960A8 != 0) {
            Ui_AddElement(gUi[0], 2, 0x138);
            Ui_AddElement(gUi[0], 2, 0x135);
            Ui_SetElementPos(gUi[0], 2, 0x135, 143.0f, 133.0f);
        }
        if (a >= 0) {
            Ui_AddElement(gUi[0], 2, 0x135);
            if (D_800958A4 == 2) {
                Ui_SetElementPos(gUi[0], 2, 0x135, 28.0f, 200.0f);
            } else if (D_80096078->x8 == D_800959C0) {
                Ui_SetElementPos(gUi[0], 2, 0x135, 125.0f, 200.0f);
            } else {
                Ui_SetElementPos(gUi[0], 2, 0x135, 170.0f, 200.0f);
            }
        }
        if (b >= 0) {
            Ui_AddElement(gUi[0], 2, 0x134);
            if (D_800958A4 == 2) {
                Ui_SetElementPos(gUi[0], 2, 0x134, 28.0f, 20.0f);
            } else if (D_8009607C->x8 == D_800958F0) {
                Ui_SetElementPos(gUi[0], 2, 0x134, 125.0f, 20.0f);
            } else {
                Ui_SetElementPos(gUi[0], 2, 0x134, 170.0f, 20.0f);
            }
        }
        for (i = 0; i < D_800958A4; i++) {
            if (D_80091B14[i]) {
                Ui_AddElement(gUi[0], 2, i + 0xE2);
            }
        }
    }
    if (D_800958A8 == 3 && D_800958AC == 2) {
        if (gGame->x10C9 == 5) {
            if (!D_800970EC) {
                if (D_80096080->x38 == D_80096084->x38) {
                    gGame->x10A8 = (f64)gGame->x10A8 - D_800BAB40;
                    Ui_AddElement(gUi[0], 0, 0x181);
                    Ui_AddElement(gUi[0], 0, 0x182);
                    sprintf(D_80123D1E, D_800BAB20);
                    sprintf(D_80123D1E + 0x32, D_800BAB2C);
                    Ui_SetElementPos(gUi[0], 0, 0x181, 126.0f, 104.0f);
                    Ui_SetElementPos(gUi[0], 0, 0x182, 122.0f, 123.0f);
                    D_800970EC = 1;
                } else {
                    ret = 1;
                    Race_AwardPot();
                    func_80063380(1);
                    Game_SetNextMode(0x2C, 1);
                }
            }
        }
        if (D_800970EC) {
            if (gGame->x10CA == 3) {
                D_800970EC = 0;
                Ui_SetElementPos(gUi[0], 0, 0x181, -126.0f, -104.0f);
                Ui_SetElementPos(gUi[0], 0, 0x182, -122.0f, -123.0f);
                Ui_RemoveElement(gUi[0], 0x181);
                Ui_RemoveElement(gUi[0], 0x182);
            }
        }
    }
    return ret;
}
