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
#ifdef TARGET_PC
extern s32 D_80124364;
#endif

s8 Announcer_GetQueueCount(void) {
#ifdef TARGET_PC
    /* the low byte of the queue's count, the s32 D_80124364 (byte 3 of it on the N64) */
    return (s8)D_80124364;
#else
    return D_80124367;
#endif
}
