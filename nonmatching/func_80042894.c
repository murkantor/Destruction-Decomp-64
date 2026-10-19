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
extern s32 D_800958AC;
extern s32 D_800958A4;
extern s32 D_80097340;
extern s32 D_800958BC;

void func_8005AAC0(void *);
void func_80063AAC(s32);
void Mem_Free(void *);
void func_80063564(s32, s32);
void Players_Reset(void);
u8 func_800429F4(s32);

static inline void FreeItem(Item42894 *item) {
    Inner42894 *in = item->xC;
    func_8005AAC0(in->x98);
    func_8005AAC0(in->xC0);
    func_8005AAC0(in->xE8);
    func_8005AAC0(in->x110);
    func_8005AAC0(in->x138);
    func_80063AAC(D_800919F4[in->x34]);
    Mem_Free(in);
    Mem_Free(item);
}

s32 func_80042894(Obj42894 *obj) {
    s32 i;
    Item42894 **p;
    s32 limit;

    p = D_80096500;
    do {
        if (*p != NULL) {
            FreeItem(*p);
            *p = NULL;
        }
        p++;
    } while ((s32)p < (s32)(D_80096500 + 4));
    limit = (D_800958AC == 2 && D_800958A4 == 4) ? 2 : D_800958A4;
    if (D_80097340 == limit) {
        D_800958BC = 0;
        func_80063564(0xF, 1);
        Players_Reset();
        return 1;
    }
    if (obj->x28->x10 < D_800958A4) {
        if (limit == 1) {
            return func_800429F4(obj->x10);
        }
    }
    return 0;
}
