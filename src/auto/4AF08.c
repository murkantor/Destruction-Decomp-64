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
typedef struct {
    u8 pad[0x1C];
    f32 angle;
} Dial;
typedef struct {
    u8 pad[0x38];
    Dial *dial;
} Screen;
extern Screen *D_800970F0;
extern s32 D_80097D48;
void Timer_Start(void *, s64);

s32 LetterWheel_Prev(MenuItem *item) {
    s32 n;

    if (item->parent->owner != D_80097D48) {
        return 0;
    }
    if (--item->value < 0) {
        item->value = 25;
    }
    Timer_Start(item, 320000000LL);
    if (item->value == 0) {
        n = 0;
    } else {
        n = 26 - item->value;
    }
    D_800970F0->dial->angle = n * (360.0 / 26);
    return 1;
}
