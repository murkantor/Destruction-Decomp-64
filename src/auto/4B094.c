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
extern u8 D_80097102[][7];

s32 NameEntry_AddLetter(MenuItem *item) {
    if (item->parent->owner != D_80097D48) {
        return 0;
    }
    if (gNameCursor < 6) {
        gNameCursor++;
        D_80097D5C[gNameCursor] = D_80097102[item->value][0];
        if (gNameCursor < 6) {
            D_80123C88[gNameCursor + 1] = D_80123C88[gNameCursor];
        } else {
            D_80123C88[gNameCursor + 2] = D_80123C88[gNameCursor];
        }
        D_80123C88[gNameCursor] = D_80097102[item->value][0];
        return 1;
    }
    return 0;
}
