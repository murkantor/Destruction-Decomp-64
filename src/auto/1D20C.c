#include "common.h"
#include "m2c.h"

typedef struct { s32 x0; s32 x4; } ResPair;
typedef struct {
    u8 pad0[0x16];
    u8 x16;
    u8 pad17;
    ResPair *x18;
    s32 x1C;
    s32 x20;
    s32 x24;
} ResHdr;
typedef struct {
    s32 a[5];
    s32 b[5];
    s8 n;
    u8 pad29[7];
} ResTbl;
typedef struct { u8 pad[0xC]; s16 xC; } Game;
extern Game *gGame;
extern u8 D_80091B28;
extern ResTbl D_80091B30[];
extern u8 D_800958A1;
extern ResPair *D_8012EE24;
u32 Res_GetSize(s32);
void *Mem_AllocAligned(u32, s32, s32);
void Res_Load(s32, void *);
s32 Res_Get(s32);
s32 func_80063D8C(s32);
s32 func_80063DAC(s32);
s32 func_80063E0C(s32, s32, void *);
void func_8001C944(ResHdr *, s32, s32);
void func_80063F88(void);

u8 *Res_LoadModel(s32 id) {
    u8 *buf;
    s32 base;
    ResHdr *hdr;
    s32 x24;
    s32 i, j;
    s32 flag;

    buf = Mem_AllocAligned(Res_GetSize(id), 0x3F, 0);
    Res_Load(id, buf);
    base = func_80063D8C(id);
    hdr = (ResHdr *)(func_80063DAC(id) - base + (s32)buf);
    hdr->x20 = func_80063E0C(hdr->x20, base, buf);
    x24 = hdr->x24;
    hdr->x24 = func_80063E0C(x24, base, buf);
    hdr->x18 = (ResPair *)func_80063E0C((s32)hdr->x18, base, buf);
    if (hdr->x1C) {
        hdr->x1C = func_80063E0C(hdr->x1C, base, buf);
    }
    for (i = 0; i < hdr->x16; i++) {
        hdr->x18[i].x4 = func_80063D8C(hdr->x18[i].x4);
        hdr->x18[i].x0 = Res_Get(hdr->x18[i].x0);
    }
    if (D_80091B28 && gGame->xC == 2) {
        D_80091B28 = 0;
        for (j = 0; j < 7; j++) {
            for (i = 0; i < D_80091B30[j].n; i++) {
                D_80091B30[j].a[i] = Res_Get(D_80091B30[j].b[i]);
            }
        }
    }
    flag = hdr->x1C ? 0 : D_800958A1;
    if (id == 0x4650) {
        D_8012EE24 = hdr->x18;
    }
    func_8001C944(hdr, x24, flag);
    func_80063F88();
    return buf;
}
