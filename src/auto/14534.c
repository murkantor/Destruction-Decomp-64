#include "common.h"
#include "m2c.h"

typedef struct { s32 x0; s32 x4[5]; s32 x18[5]; s8 x2C; u8 pad2D[3]; } ResEntry30;
typedef struct { u8 pad[0x68]; s32 x68; void *x6C; } Unk238E8;
extern ResEntry30 *D_80125BD0;
extern s32 D_80124358;
extern Unk238E8 *D_801238E8;
extern s32 D_80131F28;
u32 Res_GetSize(s32);
void *Mem_AllocAligned(u32 size, s32 alignMask, s32 alignOffset);
void Res_Load(s32, void *);
s32 Res_Get(s32);

ResEntry30 *Res_LoadTable30(s32 id) {
    u32 size = Res_GetSize(id);
    ResEntry30 *p = Mem_AllocAligned(size, 0x3F, 0);
    s32 i;
    s32 j;
    D_80125BD0 = p;
    D_801238E8->x6C = p;
    D_80124358 = size / 48;
    D_801238E8->x68 = size / 48;
    Res_Load(id, p);
    for (i = 0; i < D_80124358; i++) {
        for (j = 0; j < D_80125BD0[i].x2C; j++) {
            D_80125BD0[i].x4[j] = Res_Get(D_80125BD0[i].x18[j]);
        }
    }
    D_80131F28 = 0;
    return D_80125BD0;
}
