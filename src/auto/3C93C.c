#include "common.h"
#include "m2c.h"

typedef struct {
    u8 pad[0x10];
    s32 value;
} MenuItem;
typedef struct {
    s32 used;
    u8 pad4[0x1C];
} SaveSlot;
extern u32 D_80097350;
extern SaveSlot gSaveSlots[16];
extern char gSaveSlotText[2][16][0x32];
void Timer_Start(void *, s64);
void *Ui_LookupTextId(char *);
void Ui_SetElementColor(void *, s32);

s32 SaveMenu_NextSlot(MenuItem *item) {
    s32 i;
    u8 found = 0;

    if (D_80097350 < 2) {
        return 0;
    }
    Timer_Start(item, 250000000LL);
    i = item->value;
    do {
        if (++i == 16) {
            i = 0;
        }
        if (gSaveSlots[i].used != 0) {
            found = 1;
        }
    } while (!found);
    Ui_SetElementColor(Ui_LookupTextId(gSaveSlotText[0][item->value]), 0x11200);
    Ui_SetElementColor(Ui_LookupTextId(gSaveSlotText[1][item->value]), 0x11200);
    Ui_SetElementColor(Ui_LookupTextId(gSaveSlotText[0][i]), 0x111FE);
    Ui_SetElementColor(Ui_LookupTextId(gSaveSlotText[1][i]), 0x111FE);
    item->value = i;
    return 1;
}
