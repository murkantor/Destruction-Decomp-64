#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0xA8C]; s32 xA8C; u8 padA90[0xAB4 - 0xA90]; s32 xAB4; u8 padAB8[0xAC8 - 0xAB8]; s32 xAC8; u8 padACC[0xAF0 - 0xACC]; s32 xAF0; } Obj;
void func_80009C90(Obj *, u8);

void Car_SetPartsForMode(Obj *o, u32 mode) {
    u8 i;
    switch (mode) {
    case 0:
        func_80009C90(o, 2);
        func_80009C90(o, 3);
        func_80009C90(o, 0);
        break;
    case 1:
        func_80009C90(o, 2);
        func_80009C90(o, 6);
        if (o->xAC8) func_80009C90(o, 0);
        break;
    case 2:
        func_80009C90(o, 8);
        break;
    case 3:
        func_80009C90(o, 10);
        func_80009C90(o, 4);
        if (o->xAF0) func_80009C90(o, 1);
        break;
    case 4:
        func_80009C90(o, 3);
        func_80009C90(o, 7);
        if (o->xA8C) func_80009C90(o, 0);
        break;
    case 5:
        func_80009C90(o, 9);
        break;
    case 6:
        func_80009C90(o, 11);
        func_80009C90(o, 5);
        if (o->xAB4) func_80009C90(o, 1);
        break;
    case 7:
        func_80009C90(o, 4);
        func_80009C90(o, 5);
        func_80009C90(o, 1);
        break;
    case 8:
    case 9:
        break;
    case 10:
        for (i = 0; i < 12; i++) {
            func_80009C90(o, i);
        }
        break;
    }
}
