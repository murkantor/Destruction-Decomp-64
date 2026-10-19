#include "common.h"
#include "m2c.h"

typedef struct {
    u8 pad[0x10];
    s32 unk10;
} Ctx;
extern Ctx *D_800CCAC0;
extern Ctx *D_800CCAC4;
s32 Sound_AllocVoice(s32, s32, s32, s32, s32);

s32 func_8006621C(Ctx *ctx, s32 a1, s32 a2, s32 a3, s32 a4, s32 a5) {
    s32 ret;

    if (ctx != NULL && ctx->unk10 < 0) {
        D_800CCAC0 = ctx;
    }
    ret = Sound_AllocVoice(a1, a2, a3, a4, a5);
    D_800CCAC0 = D_800CCAC4;
    return ret;
}
