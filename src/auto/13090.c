#include "common.h"
#include "m2c.h"

M2C_UNK Obj_FreeResources(M2C_UNK *);                   /* extern */
M2C_UNK func_8001A174();                            /* extern */
M2C_UNK func_80063AAC(s32);                         /* extern */
extern u8 D_80083AE8;
extern M2C_UNK D_80083AEC;
extern s32 D_800958A8;
extern s32 D_801244D0;
extern M2C_UNK D_80125460;
extern s32 D_80130248;

void func_80012490(void) {
    Obj_FreeResources(&D_80125460);
    if (D_80083AE8 != 0) {
        if (D_800958A8 != 3) {
            func_8001A174();
        }
        func_80063AAC(*((__typeof__(D_80083AEC) *)((s8 *)&D_80083AEC + (D_80130248 * 4))));
        D_80083AE8 = 0;
        D_801244D0 = 0;
    }
}
