#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0x10A8]; u64 x10A8; } Game;
typedef struct { u8 pad[8]; void *x8; } Slot;
typedef struct { u8 pad[0x18]; s32 x18; } Target;
typedef struct { u8 pad[0x38]; s32 x38; } Rec;

extern Game *gGame;
extern void *gUi[];
extern Slot *D_80096078;
extern Slot *D_8009607C;
extern Rec *D_80096080;
extern Rec *D_80096084;
extern u64 D_800960A8;
extern u64 D_800960A0;
extern f64 D_800B91B0;
extern f64 D_800B91B8;
void func_800266C8(Slot *);
void Ui_AddElement(void *, s32, s32);
void Ui_SetElementPos(void *, s32, s32, f32, f32);
u64 Time_Get(u64 *);

void Team_ScoreFlagAtBase(s32 *arg0, Target *t) {
    if (*arg0 == 0) {
        if (D_80096078->x8 == t) {
            t->x18++;
            D_80096084->x38 += 10;
            func_800266C8(D_80096078);
            Ui_AddElement(gUi[0], 2, 0x138);
            Ui_SetElementPos(gUi[0], 2, 0x135, 143.0f, 133.0f);
            D_800960A8 = Time_Get(&gGame->x10A8);
            D_800960A8 = D_800960A8 + D_800B91B0;
        }
    } else {
        if (D_8009607C->x8 == t) {
            t->x18++;
            D_80096080->x38 += 10;
            func_800266C8(D_8009607C);
            Ui_AddElement(gUi[0], 2, 0x136);
            Ui_SetElementPos(gUi[0], 2, 0x134, 143.0f, 20.0f);
            D_800960A0 = Time_Get(&gGame->x10A8) + D_800B91B8;
        }
    }
}
