#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0x8B54]; s32 x8B54; u8 pad8B58[4]; s16 x8B5C; s16 x8B5E; s16 x8B60; s16 x8B62; s16 x8B64; } Ui;
typedef struct { s32 a; s32 b; } Pair;
void Gfx_SetUcodeSwap(s32);
void Hud_InitForMode(Ui *, Pair *);
void Ui_LoadResourceFile(Ui *, s32);

void func_8005D81C(Ui *ui) {
    Pair p;

    ui->x8B54 = 0;
    Gfx_SetUcodeSwap(0);
    ui->x8B5C = 0;
    ui->x8B5E = 0;
    ui->x8B60 = 0;
    ui->x8B62 = 0;
    ui->x8B64 = 0;
    p.b = 0;
    p.a = 0;
    Hud_InitForMode(ui, &p);
    Ui_LoadResourceFile(ui, p.a);
    Ui_LoadResourceFile(ui, p.b);
}
