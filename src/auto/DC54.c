#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y; } Pos2;
typedef struct { Pos2 p[2]; } Pos2x2;
typedef struct { Pos2 p[4]; } Pos2x4;
typedef struct { u8 pad[0x10]; s8 x10; } Info;
typedef struct { u8 pad[0x30]; s32 x30; u8 pad2[8]; Info *x3C; } Player;
extern Player **D_80096074;
extern Pos2x2 D_800B8200;
extern Pos2x4 D_800B8210;
extern s32 D_800958A4;
extern void *gUi[];
void Ui_SetElementPos(void *, s32, s32, f32, f32);

s32 func_8000D054(void) {
    Pos2x2 a = D_800B8200;
    Pos2x4 b = D_800B8210;
    Player *p = *D_80096074;
    Pos2 *tbl;
    Pos2 *e;

    if (p == NULL) {
        Ui_SetElementPos(gUi[0], 2, 0xE0, -40.0f, 0.0f);
    } else if (p->x30 == 1) {
        Ui_SetElementPos(gUi[0], 2, 0xE0, -40.0f, 0.0f);
    } else {
        tbl = (D_800958A4 == 2) ? a.p : b.p;
        e = (Pos2 *)((*D_80096074)->x3C->x10 * 8 + (u32)tbl);
        Ui_SetElementPos(gUi[0], 2, 0xE0, e->x, e->y);
    }
    return 0;
}
