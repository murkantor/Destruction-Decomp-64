#include "common.h"
#include "m2c.h"

typedef struct { s32 x0; s32 x4; s32 x8; s32 xC; u8 x10; u8 pad11[3]; s32 x14; s32 x18; f32 x1C; f32 x20; f32 x24_[5]; u8 x38; u8 pad39[3]; u8 x3C; u8 pad3D[3]; s32 x40; s32 x44; } Menu48; /* the real layout (as UiElemDesc in 3E0D4.c, names as this file uses them): the PC port converts the static descriptors at boot by their declared widths */
typedef struct { s32 a; s32 b; } Pair;
typedef struct { Pair p[3]; } Pair3;
typedef struct { s32 n; u8 pad[0x18]; void *x1C[4]; } Players;
extern Menu48 D_800BA704;
extern Pair3 D_800BA74C;
extern Players D_800958A4;
extern void *gUi[];
void Ui_ResetScreen(s32, s32);
void Ui_AddSpriteElement(void *, Menu48 *);
void func_8005D8B8(void *, s32, s32);
void CarSelect_SetCar(void *, s32, s32);

void CarSelect_ShowMulti(void) {
    Menu48 m = D_800BA704;
    Pair3 t = D_800BA74C;
    s32 i;

    Ui_ResetScreen(t.p[D_800958A4.n - 2].a, t.p[D_800958A4.n - 2].b);
    m.x14 = D_800958A4.n + 0x11506;
    Ui_AddSpriteElement(gUi[0], &m);
    func_8005D8B8(gUi[0], 2, 0x3B);
    for (i = 0; i < D_800958A4.n; i++) {
        CarSelect_SetCar(D_800958A4.x1C[i], -1, i);
    }
}
