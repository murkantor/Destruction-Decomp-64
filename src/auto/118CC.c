#include "common.h"
#include "m2c.h"

typedef struct Node {
    struct Node *next;
    s32 x4;
    s32 x8;
    f32 xC;
    f32 x10;
    s32 x14;
    f32 x18;
    f32 x1C;
    f32 x20;
    f32 x24;
    f32 x28;
    f32 x2C;
    s32 x30;
    s32 x34;
    f32 x38;
    f32 x3C;
} Node;
typedef struct { u8 pad[8]; Node *head; s32 count; } List;
typedef struct { u8 pad[0x10]; u32 free; } Pool;
extern Pool gParticlePool;
Node *Pool_Alloc(Pool *);

Node *List_AddNode(List *l, s32 a4, s32 a8, f32 aC, f32 a10, s32 a14, f32 a18, f32 a1C, f32 a20, f32 a24,
                    f32 a28, f32 a2C, s32 a30, s32 a34, f32 a38, f32 a3C) {
    Node *n;

    if (gParticlePool.free <= 0x2000) {
        return NULL;
    }
    n = Pool_Alloc(&gParticlePool);
    if (n != NULL) {
        n->x4 = a4;
        n->x8 = a8;
        n->xC = aC;
        n->x10 = a10;
        n->x14 = a14;
        n->x18 = a18;
        n->x1C = a1C;
        n->x20 = a20;
        n->x24 = a24;
        n->x28 = a28;
        n->x2C = a2C;
        n->x30 = a30;
        n->x34 = a34;
        n->x38 = a38;
        n->x3C = a3C;
        n->next = l->head;
        l->head = n;
        l->count++;
    }
    return n;
}
