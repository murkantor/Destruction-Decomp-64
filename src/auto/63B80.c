#include "common.h"
#include "m2c.h"

/* Wrappers that hand back their callee's result (Race_Start uses func_80062F9C's) */

M2C_UNK Camera_CycleView();                            /* extern */

s32 func_80062F80(void *arg0) {
    return Camera_CycleView(arg0);
}

M2C_UNK Obj_Create();                            /* extern */

s32 func_80062F9C(void *arg0) {
    return Obj_Create(arg0);
}

M2C_UNK func_80021334();                            /* extern */

s32 func_80062FB8(void *arg0) {
    return func_80021334(arg0);
}

M2C_UNK func_80021350();                            /* extern */

s32 func_80062FD4(void *arg0) {
    return func_80021350(arg0);
}

M2C_UNK Camera_UpdateChase();                            /* extern */

s32 func_80062FF0(void *arg0) {
    return Camera_UpdateChase(arg0);
}
