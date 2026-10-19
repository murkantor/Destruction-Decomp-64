#include "common.h"
#include "m2c.h"

extern void *D_800CCAC0;
extern void *D_800CCAC4;
s32 Seq_StartVoices(s32);

s32 func_80066194(u8 *obj, s32 arg) {
    s32 r;

    if (obj != NULL && *(s32 *)(obj + 0x10) < 0) {
        D_800CCAC0 = obj;
    }
    r = Seq_StartVoices(arg);
    D_800CCAC0 = D_800CCAC4;
    return r;
}
