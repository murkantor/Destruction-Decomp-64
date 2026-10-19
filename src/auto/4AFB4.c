#include "common.h"
#include "m2c.h"

typedef struct {
    u8 pad[0x10];
    s8 owner;
} MenuParent;
typedef struct {
    u8 pad[0x10];
    s32 value;
    u8 pad14[0x14];
    MenuParent *parent;
} MenuItem;
extern s32 D_80097D48;
extern s32 D_800970FC;
extern u8 D_80097D5C[];
extern u8 D_80123C88[];
void Timer_Start(void *, s64);

s32 NameEntry_Backspace(MenuItem *item) {
    Timer_Start(item, 300000000LL);
    if (item->parent->owner != D_80097D48) {
        return 0;
    }
    if (D_800970FC < 0) {
        return 0;
    }
    D_80097D5C[D_800970FC] = 0;
    if (D_800970FC < 6) {
        D_80123C88[D_800970FC] = D_80123C88[D_800970FC + 1];
        D_80123C88[D_800970FC + 1] = 0;
    } else {
        D_80123C88[D_800970FC] = D_80123C88[D_800970FC + 2];
    }
    D_800970FC--;
    return 1;
}
