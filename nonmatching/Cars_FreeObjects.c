typedef struct {
    u8 pad0[0x34];
    s32 x34;
    u8 pad38[0x98 - 0x38];
    u8 x98[0x28];
    u8 xC0[0x28];
    u8 xE8[0x28];
    u8 x110[0x28];
    u8 x138[0x28];
} Inner;

typedef struct {
    u8 pad0[0xC];
    Inner *xC;
} Outer;

typedef struct {
    u8 pad0[0xC];
    s16 xC;
} Game;

typedef struct {
    u8 *x0;
    s16 x4;
} Cam;

extern Game *gGame;
extern Cam D_8011EEE4;
extern s32 D_800919F4[];
extern Outer *D_800964F0[];
extern Outer *D_80096500[];
extern u8 D_80124360;
extern void *D_800970F0;
extern void *D_800970F4;
extern void *D_800970F8;
void func_8006417C(void *, Outer *);
void func_8005AAC0(void *);
void func_80063AAC(s32);
void Mem_Free(void *);
void Obj_Free(void *);

static inline Outer *FreeOuter(Outer *o) {
    Inner *in;

    if (o != NULL) {
        in = o->xC;
        func_8005AAC0(in->x98);
        func_8005AAC0(in->xC0);
        func_8005AAC0(in->xE8);
        func_8005AAC0(in->x110);
        func_8005AAC0(in->x138);
        func_80063AAC(D_800919F4[in->x34]);
        Mem_Free(in);
        Mem_Free(o);
        return NULL;
    }
    return NULL;
}

void Cars_FreeObjects(void) {
    s32 i;
    Outer *o;
    Inner *in;

    for (i = 0; i < 4; i++) {
        if (D_800964F0[i] != NULL) {
            if (D_8011EEE4.x4 == gGame->xC) {
                func_8006417C(D_8011EEE4.x0 + 0x334, D_800964F0[i]);
            }
            D_800964F0[i] = FreeOuter(D_800964F0[i]);
        }
        o = D_80096500[i];
        if (o != NULL) {
            in = o->xC;
            func_8005AAC0(in->x98);
            func_8005AAC0(in->xC0);
            func_8005AAC0(in->xE8);
            func_8005AAC0(in->x110);
            func_8005AAC0(in->x138);
            func_80063AAC(D_800919F4[in->x34]);
            Mem_Free(in);
            Mem_Free(o);
            D_80096500[i] = NULL;
        }
    }
    if (D_80124360) {
        D_80124360 = 0;
        func_80063AAC(0x4650);
    }
    if (D_800970F0 != NULL) {
        Obj_Free(D_800970F0);
        D_800970F0 = NULL;
    }
    if (D_800970F4 != NULL) {
        Obj_Free(D_800970F4);
        D_800970F4 = NULL;
    }
    if (D_800970F8 != NULL) {
        Obj_Free(D_800970F8);
        D_800970F8 = NULL;
    }
}
