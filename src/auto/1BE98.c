#include "common.h"
#include "m2c.h"

extern s32 D_800958A8;
extern s32 D_800958AC;
extern s32 D_800D1C20[12];

s32 Race_FindCarSlot(s32 v) {
    s32 i;

    if (D_800958A8 == 3 && D_800958AC == 2) {
        i = v;
    } else {
        for (i = 0; i < 12; i++) {
            if (D_800D1C20[i] == v) {
                break;
            }
        }
    }
    return i;
}
