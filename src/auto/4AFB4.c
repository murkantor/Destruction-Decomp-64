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
extern s32 gNameCursor;
extern u8 D_80097D5C[];
extern u8 D_80123C88[];
void Timer_Start(void *, s64);

s32 NameEntry_Backspace(MenuItem *item) {
    Timer_Start(item, 300000000LL);
    if (item->parent->owner != D_80097D48) {
        return 0;
    }
    if (gNameCursor < 0) {
        return 0;
    }
    D_80097D5C[gNameCursor] = 0;
    if (gNameCursor < 6) {
        D_80123C88[gNameCursor] = D_80123C88[gNameCursor + 1];
        D_80123C88[gNameCursor + 1] = 0;
    } else {
        D_80123C88[gNameCursor] = D_80123C88[gNameCursor + 2];
    }
    gNameCursor--;
    return 1;
}
