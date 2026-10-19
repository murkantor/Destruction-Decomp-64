#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0x10]; s32 value; } MenuItem;
extern char gSaveSlotText[2][16][0x32];
void Timer_Start(void *, s64);
void *func_8001007C(char *);
void func_800100BC(void *, s32);

s32 func_8003E034(MenuItem *item) {
    func_800100BC(func_8001007C(gSaveSlotText[0][item->value]), 0x111F6);
    item->value--;
    if (item->value < 0) {
        item->value = 2;
    }
    func_800100BC(func_8001007C(gSaveSlotText[0][item->value]), 0x111F4);
    Timer_Start(item, 250000000LL);
    return 1;
}

extern s32 D_80097350[];

s32 func_8003E0F4(MenuItem *item) {
    func_800100BC(func_8001007C(gSaveSlotText[0][item->value]), 0x111F6);
    item->value++;
    if (item->value > D_80097350[0] - 1) {
        item->value = 0;
    }
    func_800100BC(func_8001007C(gSaveSlotText[0][item->value]), 0x111F4);
    Timer_Start(item, 250000000LL);
    return 1;
}
