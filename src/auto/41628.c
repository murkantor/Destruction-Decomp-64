#include "common.h"
#include "m2c.h"

typedef struct {
    u8 pad[0x10];
    s32 value;
} MenuItem;
extern s32 D_80125BE4[];
void Timer_Start(void *, s64);
void TrackMenu_Highlight(s32, s32);

s32 Menu_Prev(MenuItem *item) {
    s32 i;
    s32 j;

    Timer_Start(item, 300000000LL);
    i = item->value;
    if (i == 0) {
        return 0;
    }
    j = i - 1;
    TrackMenu_Highlight(D_80125BE4[i], D_80125BE4[j]);
    item->value = j;
    return 1;
}
