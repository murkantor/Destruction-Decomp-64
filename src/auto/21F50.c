#include "common.h"
#include "m2c.h"

typedef struct { void *x0; u8 pad4[0x64]; } PlayerSlot;
typedef struct { u8 pad[0x40C]; void *x40C[0x100]; void *x80C[0x141]; u8 padD10[2]; s16 xD12; s16 xD14; } ObjList;
extern s8 D_800D31CC[];
extern PlayerSlot D_800958DC[];

void func_80021350(ObjList *l) {
    u8 i;

    for (i = 0; i < D_800D31CC[0]; i++) {
        l->x80C[l->xD14++] = D_800958DC[i].x0;
        l->x40C[l->xD12++] = D_800958DC[i].x0;
    }
}
