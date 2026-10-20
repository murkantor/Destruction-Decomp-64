#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0x8B54]; s32 x8B54; u8 pad8B58[4]; s16 x8B5C; s16 x8B5E; s16 x8B60; s16 x8B62; s16 x8B64; } Ui;
typedef struct { s32 a; s32 b; } Pair;
void func_800601D0(s32);
void Hud_InitForMode(Ui *, Pair *);
void func_8005DD20(Ui *, s32);

void func_8005D81C(Ui *ui) {
    Pair p;

    ui->x8B54 = 0;
    func_800601D0(0);
    ui->x8B5C = 0;
    ui->x8B5E = 0;
    ui->x8B60 = 0;
    ui->x8B62 = 0;
    ui->x8B64 = 0;
    p.b = 0;
    p.a = 0;
    Hud_InitForMode(ui, &p);
    func_8005DD20(ui, p.a);
    func_8005DD20(ui, p.b);
}
