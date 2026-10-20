#include "common.h"
#include "m2c.h"

typedef struct {
    s32 x0;
    u8 pad4[0x20];
    s32 x24;
    u8 pad28[0x18];
} Item24CC8;

typedef struct {
    u8 pad0[4];
    u8 x4;
    u8 x5;
    u8 pad6[0x1E];
    u8 x24;
    u8 pad25[3];
    Item24CC8 *x28;
    s32 *x2C;
} Track24CC8;

typedef struct Node24CC8 {
    s16 x0;
    s16 x2;
    struct Node24CC8 *x4;
    u8 pad8[4];
    Track24CC8 *xC;
    u8 pad10[8];
    s32 x18;
} Node24CC8;

typedef struct {
    u8 pad0[0x70];
    Node24CC8 *x70;
} Sub24CC8;

typedef struct {
    u8 pad0[0x1100];
    Sub24CC8 *x1100;
} Game;

extern Game *gGame[];

void *Mem_Alloc(s32);
void Surface_InitTable(s32);
void Surface_AddTexture(s32);
s32 Res_Get(s32);
s16 Obj_NextId(void);
void Track_ResetAnimTables(void);

s32 Track_LoadResources(Track24CC8 *t, s32 arg1) {
    Node24CC8 *n;
    s32 i;

    n = Mem_Alloc(0x1C);
    Surface_InitTable(t->x24);
    for (i = 0; i < t->x24; i++) {
        Surface_AddTexture(t->x28[i].x0);
        t->x28[i].x0 = Res_Get(t->x28[i].x0);
        if (t->x28[i].x24 != 0) {
            t->x28[i].x24 = Res_Get(t->x28[i].x24);
        } else {
            t->x28[i].x24 = 0;
        }
    }
    n->x0 = 2;
    n->x2 = Obj_NextId();
    n->x4 = n;
    n->xC = t;
    n->x18 = arg1;
    gGame[0]->x1100->x70 = n;
    t->x2C = Mem_Alloc(t->x4 * 4 * t->x5);
    for (i = 0; i < t->x4 * t->x5; i++) {
        t->x2C[i] = 0;
    }
    Track_ResetAnimTables();
    return 0;
}
