#include "common.h"
#include "m2c.h"

typedef struct {
    u8 pad[0x10];
    s32 value;
} MenuItem;
extern s32 D_80125BE4[];
extern s32 D_80097350;
void Timer_Start(void *, s64);
void PakMenu_SelectSlot(s32, s32);

s32 Menu_Next(MenuItem *item) {
    s32 i;
    s32 j;

    Timer_Start(item, 300000000LL);
    i = item->value;
    if (i == D_80097350 - 1) {
        return 0;
    }
    j = i + 1;
    PakMenu_SelectSlot(D_80125BE4[i], D_80125BE4[j]);
    item->value = j;
    return 1;
}
