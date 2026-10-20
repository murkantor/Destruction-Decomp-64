#include "common.h"
#include "m2c.h"

typedef struct { void *x0; u8 pad4[0x20]; void *x24; u8 pad28[0x18]; } Ent40;
typedef struct Node { struct Node *next; } Node;
typedef struct { u8 pad0[4]; u8 x4; u8 x5; u8 pad6[0x1E]; u8 x24; u8 pad25[3]; Ent40 *x28; Node **x2C; } Lvl;
typedef struct { u8 pad0[0xC]; Lvl *xC; u8 pad10[8]; void *x18; } Obj;
typedef struct { u8 pad0[0x10]; s32 x10; u8 pad14[0x18]; s32 x2C; } Sub;
typedef struct { u8 pad0[0x1100]; Sub *x1100; } Game;
extern Game *gGame;
void Track_FreeGridObjects(void);
void Surface_FreeTable(void);
void Res_ReleaseInstance(void *);
void func_8000217C(s32);
void Track_FreeNodes(void);
void Track_FreeBranches(void);
void Track_FreeAnims(void);
void Track_FreeProps(void);
void List_Remove(Node **, Node *);
void Mem_Free(void *);

void Track_FreeResources(Obj *arg0) {
    s32 i;
    Sub *s;

    Track_FreeGridObjects();
    Surface_FreeTable();
    for (i = 0; i < arg0->xC->x24; i++) {
        if (arg0->xC->x28[i].x24 != NULL) {
            Res_ReleaseInstance(arg0->xC->x28[i].x24);
        }
        Res_ReleaseInstance(arg0->xC->x28[i].x0);
    }
    s = gGame->x1100;
    if (s->x10 == 1) {
        func_8000217C(s->x2C);
    } else {
        Track_FreeNodes();
        Track_FreeBranches();
    }
    Track_FreeAnims();
    Track_FreeProps();
    for (i = 0; i < arg0->xC->x4 * arg0->xC->x5; i++) {
        while (arg0->xC->x2C[i] != NULL) {
            List_Remove(&arg0->xC->x2C[i], arg0->xC->x2C[i]->next);
        }
    }
    Mem_Free(arg0->xC->x2C);
    arg0->xC->x2C = NULL;
    Mem_Free(arg0->x18);
    arg0->x18 = NULL;
    Mem_Free(arg0);
}
