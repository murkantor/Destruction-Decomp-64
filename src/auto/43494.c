#include "common.h"
#include "m2c.h"

typedef struct {
    u8 pad0[0x34];
    s32 x34;
    u8 pad38[0x60];
    u8 x98[0x28];
    u8 xC0[0x28];
    u8 xE8[0x28];
    u8 x110[0x28];
    u8 x138[0x28];
} Inner42894;

typedef struct {
    u8 pad0[0xC];
    Inner42894 *xC;
} Item42894;

typedef struct {
    u8 pad0[0x10];
    s8 x10;
} Sub42894;

typedef struct {
    u8 pad0[0x10];
    s32 x10;
    u8 pad14[0x14];
    Sub42894 *x28;
} Obj42894;

extern Item42894 *D_80096500[4];
extern s32 D_800919F4[];
extern s32 D_800958A4;
extern s32 D_800958AC;

extern s32 D_80097340;
extern s32 D_800958BC;

void ModelInst_Free(void *);
void Res_Release(s32);
void Mem_Free(void *);
void Game_SetNextMode(s32, s32);
void Players_Reset(void);
u8 Menu_ToggleHelpOverlay(s32);

static inline void FreeItem(Item42894 *item, s32 *tbl) {
    Inner42894 *in = item->xC;
    ModelInst_Free(in->x98);
    ModelInst_Free(in->xC0);
    ModelInst_Free(in->xE8);
    ModelInst_Free(in->x110);
    ModelInst_Free(in->x138);
    Res_Release(tbl[in->x34]);
    Mem_Free(in);
    Mem_Free(item);
}

s32 CarSelect_Update(Obj42894 *obj) {
    Item42894 **p;
    s32 *tbl;
    register s32 limit ASM_REG("$3");
    register s32 n ASM_REG("$2");

    tbl = D_800919F4;
    p = D_80096500;
    do {
        if (*p != NULL) {
            FreeItem(*p, tbl);
            *p = NULL;
        }
        p++;
    } while ((s32)p < (s32)(D_80096500 + 4));

    if (D_800958AC == 2) {
        limit = 2;
        n = D_800958A4;
        if (n == 4) {
            goto check;
        }
    }
    limit = D_800958A4;
check:
    if (D_80097340 == limit) {
        D_800958BC = 0;
        Game_SetNextMode(0xF, 1);
        Players_Reset();
        return 1;
    }
    {
        register s32 c ASM_REG("$2");
        register Sub42894 *sub ASM_REG("$2");
        s32 v;

        sub = obj->x28;
        v = sub->x10;
        n = D_800958A4;
        c = v < n;
        if (c) {
            if (limit == 1) {
                goto call;
            }
        }
    }
    return 0;
call:
    return Menu_ToggleHelpOverlay(obj->x10);
}
/* agent62: matched with hard-register locals. limit in $3 keeps v1 live
 * across the A4 == 4 test (limit = 2 is set before it); n/sub/c in $2 force
 * the D_800958A4 loads and the slt result into v0, so the const 4 and the lb
 * value take a0 as in the original (plain pseudos lose the local-alloc
 * priority order: the slt result ties to the lb operand). */
