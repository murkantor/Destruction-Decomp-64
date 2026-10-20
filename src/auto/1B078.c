#include "common.h"
#include "m2c.h"

void Grid_PosToCell(void *arg0, s32 arg1, s32 arg2, s32 *arg3, s32 *arg4) {
    s32 temp_v1;
    s32 temp_v1_2;

    temp_v1 = M2C_FIELD(arg0, s32 *, 8);
    *arg3 = (s32) ((arg1 - temp_v1) * M2C_FIELD(arg0, s32 *, 0)) / (s32) (M2C_FIELD(arg0, s32 *, 0x10) - temp_v1);
    temp_v1_2 = M2C_FIELD(arg0, s32 *, 0xC);
    *arg4 = (s32) ((arg2 - temp_v1_2) * M2C_FIELD(arg0, s32 *, 4)) / (s32) (M2C_FIELD(arg0, s32 *, 0x14) - temp_v1_2);
}
