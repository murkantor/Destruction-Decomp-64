#include "common.h"
#include "m2c.h"

s32 __osDisableInt();                                /* extern */
M2C_UNK __osRestoreInt(s32);                         /* extern */
extern void *__osViNext;

void osViSetEvent(s32 arg0, s32 arg1, s16 arg2) {
    s32 temp_a0;

    temp_a0 = __osDisableInt();
    M2C_FIELD(__osViNext, s32 *, 0x10) = arg0;
    M2C_FIELD(__osViNext, s32 *, 0x14) = arg1;
    M2C_FIELD(__osViNext, s16 *, 2) = arg2;
    __osRestoreInt(temp_a0);
}
