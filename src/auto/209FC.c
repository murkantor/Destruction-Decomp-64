#include "common.h"
#include "m2c.h"

extern s32 D_800958A8[];
extern s32 D_80096038[];
extern s32 D_80091AB4[];
extern s8 D_800D31CC;
extern u8 D_80124360;
extern s32 D_80095A70;

s32 Res_Get(s32);
void Res_SetForceLoad(void);
void Res_ClearForceLoad(void);
#ifdef TARGET_PC
s32 Port_FieldExtraCars(s32 cars);
s32 Port_FieldCarType(s32 type);
#endif

void Race_LoadCarResources(void) {
    u8 used[12];
    s32 i;
    s32 k;
    s32 j;
    s32 n;
    s32 count;
    u32 idx;
    u8 *p;

    k = 11;
    p = &used[11];
    for (; k >= 0; k--) {
        *p-- = 0;
    }
    D_80124360 = 0;
    n = D_80096038[D_800958A8[3]];
    if (D_800958A8[0] == 3) {
        switch ((u32)D_800958A8[1]) {
        case 0:
            n = 8;
            break;
        case 1:
            n = 8;
            break;
        case 2:
            n = (-(D_800958A8[-1] == 2) & 6) | 4;
            break;
        case 3:
            n = 8;
            break;
        }
    }
    for (i = 0; i < D_800D31CC; i++) {
        if (D_800958A8[0] == 3 && D_800958A8[1] == 2) {
            Res_SetForceLoad();
        }
        Res_Get(D_80091AB4[D_800958A8[6 + i]]);
        idx = D_800958A8[6 + i];
        if (idx < 12) used[idx] = 1; else used[idx - 12] = 1;
        if (D_800958A8[0] == 3 && D_800958A8[1] == 2 && D_800958A8[-1] == 2) {
            for (k = 1; k < n / 2; k++) {
                Res_SetForceLoad();
                Res_Get(D_80091AB4[D_800958A8[6 + i]]);
            }
        }
    }
    count = D_800D31CC;
    if (D_800958A8[0] == 3 && D_800958A8[1] == 2) {
        Res_ClearForceLoad();
        count = n;
    }
    if (D_800958A8[0] != 2) if (count < n) {
        for (i = 0; count < n; i++) {
            if (used[i] == 0) {
#ifdef TARGET_PC
                Res_Get(D_80091AB4[Port_FieldCarType(i)]); /* maybe its unlockable twin */
#else
                Res_Get(D_80091AB4[i]);
#endif
                count++;
            }
        }
    }
#ifdef TARGET_PC
    /* The doubled field (port/src/field.c): as many computer cars again, each a
       new car (force-loaded) of the computer cars' types in turn. Never a player's
       type: the game tells the players' cars by their type. */
    if (D_800958A8[0] != 2 && !(D_800958A8[0] == 3 && D_800958A8[1] == 2)) {
        s32 extra = Port_FieldExtraCars(count);
        for (i = 0; extra > 0; i = (i + 1) % 12) {
            if (used[i]) {
                continue;
            }
            Res_SetForceLoad();
            Res_Get(D_80091AB4[Port_FieldCarType(i)]);
            count++;
            extra--;
        }
        Res_ClearForceLoad();
    }
#endif
    D_80095A70 = count;
}
