#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef struct { u8 pad[0x1058]; u64 x1058; } Game;

typedef struct {
    u16 x0;
    u8 pad2[2];
    s32 x4;
    Vec3f x8;
    Vec3f x14;
} Spawn;

typedef struct {
    Vec3f x0;
    Vec3f xC;
    Vec3f x18;
    s32 x24;
} Cam;

typedef struct {
    u8 pad0[0x38];
    Cam *x38;
} Obj;

typedef struct { char s[25]; } Str25;
typedef struct { char s[20]; } Str20;

extern Vec3f D_800BAF8C;
extern Vec3f D_800BAF98;
extern char D_800BAF80[];
extern Str25 D_800BAFA4;
extern Str25 D_800BAFC0;
extern Str20 D_800BAFDC;
extern Str25 D_80123BF2;
extern Str25 D_80123C24;
extern Str20 D_80123C56;
extern char D_80123C88[];
extern char D_80097D5C[];
extern Obj *D_800970F0;
extern s32 D_800958DC[];
extern s32 D_800958A8;
extern s32 D_80097D48;
extern s32 D_80097350;
extern s32 gNameCursor;
extern u64 D_80131298;
extern void *gUi;
extern Game *gGame;
extern char gSaveSlotText[];
void *Mem_Alloc(s32);
void Mem_Free(void *);
Obj *func_8001325C(Spawn *);
void func_80064100(void *);
void func_8006300C(s32, f32, f32, f32, f32, f32, f32);
void Ui_ResetScreen(s32, s32);
void Ui_AddElement(void *, s32, s32);
s32 sprintf(char *, const char *, ...);
u64 Time_Get(u64 *);

void func_8004A6E8(void) {
    Vec3f pos = D_800BAF8C;
    Vec3f zero = D_800BAF98;
    Spawn *sp;
    s32 i;

    if (D_800970F0 == NULL) {
        sp = Mem_Alloc(0x20);
        sp->x0 = 0x1E;
        sp->x4 = 0xEB3C;
        sp->x8 = pos;
        sp->x14 = zero;
        D_800970F0 = func_8001325C(sp);
        Mem_Free(sp);
    } else {
        Cam *c = D_800970F0->x38;
        c->x0 = pos;
        c->xC = zero;
        c->x18 = zero;
        c->x24 = 0;
    }
    func_80064100(D_800970F0);
    func_8006300C(D_800958DC[0], 0.0f, 40.0f, 300.0f, 0.0f, 0.0f, 150.0f);
    Ui_ResetScreen(0x112EE, 0x112EF);
    {
    void **ui = &gUi;
    Ui_AddElement(*ui, 2, 0x73);
    Ui_AddElement(*ui, 2, 0x42);
    if (D_800958A8 == 3) {
        Ui_AddElement(*ui, 0, 0x17A);
        sprintf(gSaveSlotText, D_800BAF80, D_80097D48 + 1);
    }
    }
    D_80123BF2 = D_800BAFA4;
    D_80123C24 = D_800BAFC0;
    D_80123C56 = D_800BAFDC;
    for (i = 0; i < 9; i++) {
        D_80123C88[i] = 0;
        if (i == 0) {
            D_80123C88[i] = '_';
        }
        D_80097D5C[i] = 0;
    }
    D_80131298 = Time_Get(&gGame->x1058);
    Ui_AddElement(gUi, 0, 0x17B);
    Ui_AddElement(gUi, 0, 0x17C);
    Ui_AddElement(gUi, 0, 0x17D);
    Ui_AddElement(gUi, 0, 0x17E);
    D_80097350 = 0;
    gNameCursor = -1;
}
