#include "common.h"
#include "m2c.h"

M2C_UNK Game_RunStartupChecks();                            /* extern */
M2C_UNK Timer_InitRate();                            /* extern */
M2C_UNK Clock_Init(void *, M2C_UNK);             /* extern */
u64 Time_Get(M2C_UNK *);                       /* extern */
M2C_UNK Res_ComputeSizes();                            /* extern */
s16 Obj_NextId();                                /* extern */
M2C_UNK srand(u32);                         /* extern */
extern M2C_UNK gGameTime;

s32 Game_Init(void *arg0) {
    M2C_FIELD(arg0, s16 *, 0) = 0x3E8;
    M2C_FIELD(arg0, s16 *, 2) = Obj_NextId();
    M2C_FIELD(arg0, void **, 4) = arg0;
    M2C_FIELD(arg0, s8 *, 0x4BD3C) = 1;
    M2C_FIELD(arg0, s8 *, 0x38) = 0;
    Timer_InitRate();
    Clock_Init(arg0 + 0x10, 0);
    Res_ComputeSizes();
    srand((u32) Time_Get(&gGameTime));
    Game_RunStartupChecks();
    return 0;
}
