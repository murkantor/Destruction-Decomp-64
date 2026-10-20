#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0x10]; s8 x10; } CarInfo;
typedef struct {
    u8 pad0[0x3C];
    CarInfo *x3C;
    u8 pad40[0x290 - 0x40];
    u8 x290[0x2B4 - 0x290];
    u8 x2B4[0x31C - 0x2B4];
    u8 x31C[4];
} Car;
typedef struct { u8 pad[0x74]; Car *cars[12]; s32 xA4; } CarList;
typedef struct { u8 pad[0x1100]; CarList *x1100; } Game;
typedef struct {
    s32 x0;
    s32 x4;
    Car *x8;
    f32 xC;
    u8 x10[4];
} Trigger;

extern Game *gGame;
extern void *gUi[];
extern void *D_80096080;
extern void *D_80096084;
extern s32 D_800958A4;

u8 Obb_OverlapsSphere(void *, void *, void *, f32, void *);
u8 Obj_HasValue(void *, Car *);
void Ui_AddElement(void *, s32, s32);
void Ui_SetElementPos(void *, s32, s32, f32, f32);

void Team_CheckFlagPickup(Trigger *t) {
    s32 i;
    Car *c;

    if (t->x8 != NULL) {
        return;
    }
    for (i = 0; i < gGame->x1100->xA4; i++) {
        c = gGame->x1100->cars[i];
        if (Obb_OverlapsSphere(c->x31C, c->x2B4, c->x290, t->xC, t->x10)) {
            if (Obj_HasValue(D_80096080, c) && t->x0 == 0) {
                t->x4 = 1;
                t->x8 = c;
                Ui_AddElement(gUi[0], 2, 0x134);
                if (D_800958A4 == 2) {
                    Ui_SetElementPos(gUi[0], 2, 0x134, 28.0f, 20.0f);
                } else if (c->x3C->x10 == 0) {
                    Ui_SetElementPos(gUi[0], 2, 0x134, 125.0f, 20.0f);
                } else {
                    Ui_SetElementPos(gUi[0], 2, 0x134, 170.0f, 20.0f);
                }
            }
            if (Obj_HasValue(D_80096084, c) && t->x0 == 1) {
                t->x4 = 1;
                t->x8 = c;
                Ui_AddElement(gUi[0], 2, 0x135);
                if (D_800958A4 == 2) {
                    Ui_SetElementPos(gUi[0], 2, 0x135, 28.0f, 200.0f);
                } else if (c->x3C->x10 == 2) {
                    Ui_SetElementPos(gUi[0], 2, 0x135, 125.0f, 200.0f);
                } else {
                    Ui_SetElementPos(gUi[0], 2, 0x135, 170.0f, 200.0f);
                }
            }
        }
    }
}
