#include "common.h"
#include "m2c.h"

extern M2C_UNK D_8007EE90;

M2C_UNK *func_80008688(void) {
    return &D_8007EE90;
}

extern M2C_UNK D_8007EE60;

M2C_UNK *func_80008694(void) {
    return &D_8007EE60;
}

extern s8 D_80124367;

s8 Announcer_GetQueueCount(void) {
    return D_80124367;
}
