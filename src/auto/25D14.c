#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0x68]; s32 x68; } UnkC;
typedef struct { u8 pad[0xC]; UnkC *xC; } Unk70;
typedef struct { u8 pad[0x10]; s32 x10; u8 pad14[0x70 - 0x14]; Unk70 *x70; } Unk1100;
typedef struct { u8 pad[0x1100]; Unk1100 *x1100; } Game;
extern Game *gGame;
extern s32 D_801245A4;
s32 Res_Get(s32);
void Race_LoadCarResources(void);
void func_80001F60(void);
s32 func_80010730(void);
void func_80064100(s32);

void Race_LoadTrackResources(u32 idx) {
    s32 a[12] = { 40000, 40100, 40200, 40300, 50000, 50100, 50300, 50400, 50200, 50600, 50500, 50700 };
    s32 b[12] = { 40080, 40190, 40280, 40370, 50050, 50143, 50360, 50450, 50250, 50680, 50550, 50750 };
    Unk1100 *u;
    Res_Get(a[idx]);
    Res_Get(gGame->x1100->x70->xC->x68);
    Res_Get(b[idx]);
    u = gGame->x1100;
    if (idx < 4) {
        u->x10 = 1;
    } else {
        u->x10 = 0;
    }
    Race_LoadCarResources();
    func_80001F60();
    D_801245A4 = func_80010730();
    func_80064100(D_801245A4);
}
