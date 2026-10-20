#include "common.h"
#include "m2c.h"

M2C_UNK func_800648D4(void *);                      /* extern */
s16 Obj_NextId();                                /* extern */

s32 func_80020F00(void *arg0, s8 arg1) {
    M2C_FIELD(arg0, s16 *, 0) = 1;
    M2C_FIELD(arg0, s16 *, 2) = Obj_NextId();
    M2C_FIELD(arg0, void **, 4) = arg0;
    M2C_FIELD(arg0, s32 *, 8) = 0;
    M2C_FIELD(arg0, s8 *, 0x10) = arg1;
    M2C_FIELD(arg0, s32 *, 0x14) = 0;
    M2C_FIELD(arg0, s8 *, 0x18) = 0;
    M2C_FIELD(arg0, s8 *, 0x19) = 0;
    M2C_FIELD(arg0, s8 *, 0x1A) = 0;
    M2C_FIELD(arg0, s8 *, 0x1B) = 0;
    M2C_FIELD(arg0, s8 *, 0x1C) = 0;
    M2C_FIELD(arg0, s32 *, 0x20) = 0;
    func_800648D4(arg0);
    return 0;
}

M2C_UNK func_80024760(s32, s32);                    /* extern */

void func_80020F70(void *arg0) {
    func_80024760(M2C_FIELD(arg0, s32 *, 0x2C), M2C_FIELD(arg0, s32 *, 0x3C));
}

M2C_UNK func_800247FC();                            /* extern */

void func_80020F94(void) {
    func_800247FC();
}
