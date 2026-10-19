#include "common.h"
#include "m2c.h"

typedef struct Obj {
    u16 x0;
    u16 x2;
    struct Obj *x4;
    u8 pad8[0xFC - 0x8];
    void *xFC;
    u8 pad100[0x4C4 - 0x100];
    s32 x4C4[4];
    u8 pad4D4[0x504 - 0x4D4];
    s32 x504;
    u8 pad508[4];
    s32 x50C;
    s32 x510;
    f32 x514;
    f32 x518;
    f32 x51C;
    s32 x520;
    u8 x524;
    u8 pad525[3];
    s8 x528;
    u8 pad529[3];
    s32 x52C;
    u8 x530;
} Obj;
u16 func_80064D70(void);
void *Mem_Alloc(s32);
s32 func_80008694(void);
s32 func_80008688(void);

s32 func_8005C5EC(Obj *obj) {
    s32 i;

    obj->x0 = 0x3EB;
    obj->x2 = func_80064D70();
    obj->x4 = obj;
    obj->xFC = Mem_Alloc(0x14A0);
    obj->x504 = 0;
    for (i = 3; i >= 0; i--) {
        obj->x4C4[i] = 0;
    }
    obj->x50C = func_80008694();
    obj->x510 = func_80008688();
    obj->x524 = 1;
    obj->x520 = 0;
    obj->x528 = -1;
    obj->x52C = 0;
    obj->x530 = 0;
    obj->x514 = 0.78125f;
    obj->x518 = 1.953125f;
    obj->x51C = 1.0f;
    return 0;
}
