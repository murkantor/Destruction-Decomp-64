#include "common.h"
#include "m2c.h"

M2C_UNK Handler_CallX8ById(void *, M2C_UNK);             /* extern */
s32 Array_Find(M2C_UNK, void *, s16);            /* extern */
M2C_UNK Array_SwapRemove(void *, s32, s16);            /* extern */

void ObjList_Remove(void *arg0, M2C_UNK arg1) {
    s32 temp_v0;
    void *temp_s1;

    temp_s1 = arg0 + 0xC;
    temp_v0 = Array_Find(arg1, temp_s1, M2C_FIELD(arg0, s16 *, 0xD10));
    if (temp_v0 >= 0) {
        Array_SwapRemove(temp_s1, temp_v0, M2C_FIELD(arg0, s16 *, 0xD10));
        M2C_FIELD(arg0, s16 *, 0xD10) = (s16) ((u16) M2C_FIELD(arg0, s16 *, 0xD10) - 1);
    }
    Handler_CallX8ById(arg0, arg1);
}

s16 Obj_NextId();                                /* extern */

s32 ObjList_Init(void *arg0) {
    M2C_FIELD(arg0, s16 *, 2) = Obj_NextId();
    M2C_FIELD(arg0, s16 *, 0xD10) = 0;
    M2C_FIELD(arg0, s16 *, 0xD12) = 0;
    M2C_FIELD(arg0, s16 *, 0xD14) = 0;
    M2C_FIELD(arg0, s32 *, 0xA0C) = 0;
    M2C_FIELD(arg0, s16 *, 0xD16) = 0;
    M2C_FIELD(arg0, s16 *, 0xD18) = 0;
    M2C_FIELD(arg0, s16 *, 0xD1A) = 0;
    M2C_FIELD(arg0, void **, 4) = arg0;
    return 0;
}
