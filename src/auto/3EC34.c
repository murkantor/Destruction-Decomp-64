#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0x10]; s32 value; } MenuItem;
extern char gSaveSlotText[2][16][0x32];
void Timer_Start(void *, s64);
void *Ui_LookupTextId(char *);
void Ui_SetElementColor(void *, s32);

s32 PakMenu_NewPakPrev(MenuItem *item) {
    Ui_SetElementColor(Ui_LookupTextId(gSaveSlotText[0][item->value]), 0x111F6);
    item->value--;
    if (item->value < 0) {
        item->value = 2;
    }
    Ui_SetElementColor(Ui_LookupTextId(gSaveSlotText[0][item->value]), 0x111F4);
    Timer_Start(item, 250000000LL);
    return 1;
}

extern s32 D_80097350[];

s32 PakMenu_NewPakNext(MenuItem *item) {
    Ui_SetElementColor(Ui_LookupTextId(gSaveSlotText[0][item->value]), 0x111F6);
    item->value++;
    if (item->value > D_80097350[0] - 1) {
        item->value = 0;
    }
    Ui_SetElementColor(Ui_LookupTextId(gSaveSlotText[0][item->value]), 0x111F4);
    Timer_Start(item, 250000000LL);
    return 1;
}
