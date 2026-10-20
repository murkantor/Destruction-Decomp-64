#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;

typedef struct {
    u16 x0;
    u8 pad2[2];
    s32 x4;
    Vec3f x8;
    Vec3f x14;
} Spawn;

typedef struct {
    s8 x0;
    u8 pad1[0x32];
} Slot;

extern Vec3f D_800BAF98;
extern void *gUi;
extern s32 D_800958C0;
extern s32 D_800964F0;
extern s32 D_800958B0[];
extern s32 D_80097350;
extern void *D_800970F4;
extern void *D_800970F8;
extern char gSaveSlotText[];
extern Slot D_80123922[];
void Ui_ResetScreen(s32, s32);
s32 func_8003E644(s32, s32);
void Ui_AddElement(void *, s32, s32);
char *strcpy(char *, const char *);
void *Mem_Alloc(s32);
void Mem_Free(void *);
void *Obj_CreateFromSpawn(Spawn *);
void func_80064100(void *);
void Msg_Queue(s32, s32, s32, s32);

void func_8004BD0C(void) {
    Vec3f pos[4] = {
        { 48.0f, -15.0f, -90.0f },
        { 43.0f, -9.0f, -80.0f },
        { 43.0f, -9.0f, -80.0f },
        { 48.0f, -4.5f, -90.0f },
    };
    Vec3f zero = D_800BAF98;
    s32 ids1[3] = { 0xEBF3, 0xEBF2, 0xEBF1 };
    s32 ids2[4] = { 0, 0xEBF4, 0xEBF5, 0xEBF6 };
    char names[4][21] = { "NOVICE CIRCUIT", "AMATEUR CIRCUIT", "PROFESSIONAL CIRCUIT", "LEGEND CIRCUIT" };
    s32 elems[3] = { 0x68, 0x69, 0x6A };
    s32 msgs[3] = { 0xEA, 0xEC, 0xEB };
    s32 i;
    Spawn *sp;

    Ui_ResetScreen(0x112F4, 0x112F5);
    D_800964F0 = func_8003E644(D_800958C0, 0);
    for (i = 0; i < 12; i++) {
        if (D_80123922[i].x0 == 0) {
            break;
        }
    }
    Ui_AddElement(gUi, 2, elems[i]);
    strcpy(gSaveSlotText, names[D_800958B0[0]]);
    D_80097350 = 0;
    sp = Mem_Alloc(0x20);
    sp->x0 = 0x21;
    sp->x4 = ids1[i];
    sp->x8 = pos[D_800958B0[0]];
    sp->x14 = zero;
    func_80064100(D_800970F4 = Obj_CreateFromSpawn(sp));
    if (ids2[D_800958B0[0]] != 0) {
        sp->x0 = 0x20;
        sp->x4 = ids2[D_800958B0[0]];
        sp->x8 = pos[D_800958B0[0]];
        sp->x14 = zero;
        func_80064100(D_800970F8 = Obj_CreateFromSpawn(sp));
    } else {
        D_800970F8 = NULL;
    }
    Mem_Free(sp);
    Msg_Queue(msgs[i], 0, 1, 0);
}
