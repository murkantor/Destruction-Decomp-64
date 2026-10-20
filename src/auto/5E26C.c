#include "common.h"
#include "m2c.h"

typedef struct { s32 x0; u8 pad[0x40]; } EntB;
typedef struct { s32 x0; s32 x4; u8 pad[0x14]; } EntC;
typedef struct { s32 x0; u8 pad[0xC]; s32 x10; u8 pad2[0x10]; } EntA;
typedef struct {
    u8 pad0[0x20];
    EntB b[1];
    u8 pad1[0x7588 - 0x64];
    EntC c[1];
    u8 pad2[0x8938 - 0x75A4];
    EntA a[1];
    u8 pad3[0x8B5C - 0x895C];
    s16 x8B5C;
    s16 x8B5E;
    s16 x8B60;
    s16 x8B62;
    s16 x8B64;
    u8 pad4[2];
    u8 x8B68;
} Ctx;
extern void Ui_ClearDrawList(Ctx *);
extern void func_8005DCC4(Ctx *);
extern void Res_Release(s32);
extern void Sprite_Destroy(s32);

void Ctx_FreeAll(Ctx *ctx) {
    s16 i;

    Ui_ClearDrawList(ctx);
    func_8005DCC4(ctx);
    ctx->x8B68 = 0;
    for (i = 0; i < ctx->x8B62; i++) {
        Res_Release(ctx->a[i].x0);
        ctx->a[i].x10 = 0;
    }
    for (i = 0; i < ctx->x8B5E; i++) {
        Sprite_Destroy(ctx->b[i].x0);
    }
    for (i = 0; i < ctx->x8B5C; i++) {
        Res_Release(ctx->c[i].x0);
        ctx->c[i].x4 = 0;
    }
    ctx->x8B5C = 0;
    ctx->x8B5E = 0;
    ctx->x8B60 = 0;
    ctx->x8B62 = 0;
    ctx->x8B64 = 0;
}
