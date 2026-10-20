#include "common.h"
#include "m2c.h"

extern s32 D_800958A8;
extern s32 D_800958AC;
#ifdef TARGET_PC
#define D_800D1C20 gFieldCarIds /* RACE_CARS long (port/game/field_part0.c) */
extern s32 D_800D1C20[RACE_CARS];
#else
extern s32 D_800D1C20[12];
#endif

s32 Race_FindCarSlot(s32 v) {
    s32 i;

    if (D_800958A8 == 3 && D_800958AC == 2) {
        i = v;
    } else {
#ifdef TARGET_PC
        /* the doubled field: every car's type */
        for (i = 0; i < RACE_CARS; i++) {
#else
        for (i = 0; i < 12; i++) {
#endif
            if (D_800D1C20[i] == v) {
                break;
            }
        }
    }
    return i;
}
