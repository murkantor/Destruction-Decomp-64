#include "common.h"
#include "m2c.h"

typedef struct {
    u8 pad[0x10];
    s32 unk10;
} Ctx;
extern Ctx *D_800CCAC0;
extern Ctx *D_800CCAC4;
s32 Sound_PlayFindVoice(s32);

s32 func_800661D8(Ctx *ctx, s32 arg1) {
    s32 ret;

    if (ctx != NULL && ctx->unk10 < 0) {
        D_800CCAC0 = ctx;
    }
    ret = Sound_PlayFindVoice(arg1);
    D_800CCAC0 = D_800CCAC4;
    return ret;
}
