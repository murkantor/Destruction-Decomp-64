#include "common.h"
#include "m2c.h"

typedef struct { s32 x0; s32 x4; s32 x8; s32 xC; } ResEntry;
typedef struct { u8 pad[0x60]; s32 x60; void *x64; } Unk238E8;
extern ResEntry *D_801255E4;
extern s32 D_80125F50;
extern Unk238E8 *D_801238E8;
u32 Res_GetSize(s32);
void *Mem_AllocAligned(u32 size, s32 alignMask, s32 alignOffset);
void Res_Load(s32, void *);
s32 Res_Get(s32);

ResEntry *ResTable_Load(s32 id) {
    u32 size = Res_GetSize(id);
    ResEntry *p = Mem_AllocAligned(size, 0x3F, 0);
    s32 i;
    D_801255E4 = p;
    D_80125F50 = size >> 4;
    D_801238E8->x64 = p;
    D_801238E8->x60 = size >> 4;
    Res_Load(id, p);
    for (i = 0; i < D_80125F50; i++) {
        D_801255E4[i].x4 = Res_Get(D_801255E4[i].x8);
    }
    return D_801255E4;
}
