#include "common.h"
#include "m2c.h"

typedef struct {
    u8 pad[0xC];
    s32 xC;
    s32 x10;
    u8 pad14[4];
    u64 x18;
    u8 x20;
    u8 x21;
    u8 x22;
    u8 x23;
    u8 x24;
    u8 x25;
    u8 x26;
    u8 pad27[0x41];
} Player;
extern Player D_800958F8[4];
extern u8 D_80097344[4];
extern s32 D_80097340;

void Players_Reset(void) {
    s32 i;

    for (i = 0; i < 4; i++) {
        D_800958F8[i].x18 = 0;
        D_800958F8[i].xC = 2;
        D_800958F8[i].x10 = 0;
        D_800958F8[i].x21 = 0;
        D_800958F8[i].x22 = 0;
        D_800958F8[i].x24 = 0;
        D_800958F8[i].x23 = 0;
        D_800958F8[i].x26 = 0;
        D_800958F8[i].x25 = 0;
        D_800958F8[i].x20 = 0;
        D_80097344[i] = 0;
    }
    D_80097340 = 0;
}
