#include "common.h"
#include "m2c.h"

typedef struct {
    u8 pad0[0x10];
    s8 x10;
} Info;

typedef struct {
    u8 pad0[0x10];
    s32 x10;
    u8 pad14[0x28 - 0x14];
    Info *x28;
} Obj;

extern u32 D_800958A8[];
extern s32 D_800958A4;
extern s32 D_800958B0;
extern u32 D_800958B4[];
extern s32 D_80096068[];
extern s32 D_80097A14;
extern u8 D_801253F9;
extern s32 gControllers;
extern s32 D_80125D30;
u8 AnyEntryFlag10(void);
void func_800505DC(void);
void Game_SetNextMode(s32, s32);
void func_8002D0E0(s32);
s32 rand(void);

s32 func_8003F330(Obj *o) {
    s32 next = -1;
    s32 i;
    s32 r;
    s32 a;

    if (AnyEntryFlag10()) {
        return 0;
    }
    D_800958A8[0] = o->x10;
    func_800505DC();
    switch (D_800958A8[0]) {
    case 0:
        if (o->x28->x10 > 0) {
            return 0;
        }
        for (i = 0; i < 3; i++) {
            r = rand() % 3;
            switch (i) {
            case 0:
                if (r == 0) {
                    r = 1;
                }
                break;
            case 1:
                r += 4;
                break;
            case 2:
                r += 8;
                break;
            }
            D_80096068[i] = r;
        }
        D_800958A4 = 1;
        Game_SetNextMode(0x14, 1);
        next = D_800958B0;
        break;
    case 1:
        if (o->x28->x10 > 0) {
            return 0;
        }
        for (i = 0; i < 3; i++) {
            r = rand() % 3;
            switch (i) {
            case 0:
                if (r == 0) {
                    r = 1;
                }
                break;
            case 1:
                r += 4;
                break;
            case 2:
                r += 8;
                break;
            }
            D_80096068[i] = r;
        }
        D_800958A4 = 1;
        Game_SetNextMode(0x14, 1);
        next = D_800958B0;
        break;
    case 2:
        if (o->x28->x10 > 0) {
            return 0;
        }
        a = D_80097A14;
        D_800958A4 = 1;
        D_800958B0 = a;
        if (D_801253F9 != 0) {
            a = 3;
        }
        func_8002D0E0(a);
        Game_SetNextMode(0x17, 1);
        if (D_800958B4[0] < 4) {
            next = 0;
            D_800958B4[0] = 4;
        } else {
            next = D_800958B4[0] - 4;
        }
        break;
    case 3:
        if (gControllers != 1) {
            return 0;
        }
        if (D_80125D30 != gControllers) {
            return 0;
        }
        D_800958A4 = 4;
        Game_SetNextMode(0x16, 1);
        if (D_801253F9 != 0) {
            D_800958B0 = 3;
        } else {
            D_800958B0 = D_80097A14;
        }
        func_8002D0E0(D_800958B0);
        next = 0;
        break;
    }
    o->x10 = next;
    return 1;
}
