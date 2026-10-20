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

s32 Race_FreeItemsCheckQualify(Obj42894 *obj) {
    s32 i;
    Item42894 **p;
    s32 limit;
    s32 ret;
    s32 *tbl;

tbl = D_800919F4; p = D_80096500; do { if (*p != NULL) { FreeItem(*p, tbl); *p = NULL; } p++; } while ((s32)p < (s32)(D_80096500 + 4));
limit = (D_800958AC == 2 && D_800958A4 == 4) ? 2 : D_800958A4; if (D_80097340 == limit) { D_800958BC = 0; Game_SetNextMode(0xF, 1); Players_Reset(); return 1; } if (obj->x28->x10 < D_800958A4) { if (limit == 1) goto call; } return 0; call: return Menu_ToggleHelpOverlay(obj->x10);
}
/* agent45: diff is only register choice: original gives `limit` v1 (const 4 in a0, final lb in a0);
   ours gives limit a0 which leaves the bne delay slot empty (+4 bytes). Tried if/else, ternary,
   nested ifs, volatile A4, temps for x10, switch, reordering; none changed it. */
/* agent59: unchanged. In the original the A4 load / const 4 of the second test
 * get v0/a0, so v1 must be busy in that block at local-alloc time (limit = v1
 * live there, i.e. limit = 2 set between the two tests?). Tried: limit = 2
 * first with || / !(&&) / empty-then forms, if (AC == 2) { limit = 2; if (A4 == 4)
 * goto l2; } limit = A4;, comma "(limit = 2, A4 != 4)", s32 two local. All
 * still give limit a0 (0x164) or 0x16c. */
