#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0x10]; s16 x10; } Obj;

void func_8000821C(Obj *o, u32 n) {
    switch (n) {
        case 0:
            o->x10 = 0x1D;
            break;
        case 1:
            o->x10 = 0xC;
            break;
        case 2:
            o->x10 = 0xD;
            break;
        case 4:
            o->x10 = 0xF;
            break;
        case 5:
            o->x10 = 0x10;
            break;
        case 6:
            o->x10 = 0x11;
            break;
        case 7:
            o->x10 = 0x12;
            break;
        case 8:
            o->x10 = 0x1E;
            break;
        case 9:
            o->x10 = 0x1F;
            break;
        case 10:
            o->x10 = 0x1B;
            break;
        case 3:
        default:
            o->x10 = 0xE;
            break;
    }
}
