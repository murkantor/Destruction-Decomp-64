#include "common.h"
#include "m2c.h"

typedef struct {
    s32 x0;
} Elem;
typedef struct Link {
    Elem *x0;
    struct Link *x4;
} Link;
typedef struct {
    u8 pad0[0x8B54];
    Link *x8B54;
} Mgr;
Elem *Ui_FindElement(Mgr *, s32, s32);
void *Mem_Alloc(s32);

void Ui_InsertElementAfter(Mgr *m, s32 kind, s32 after, s32 id) {
    Elem *e;
    Link *p;
    Link *node;
    Elem *q;

    if (m->x8B54 == NULL) {
        Elem *e2 = Ui_FindElement(m, kind, id);
        node = Mem_Alloc(8);
        if (node != NULL) {
            node->x0 = e2;
            node->x4 = m->x8B54;
            m->x8B54 = node;
        }
    } else {
        e = Ui_FindElement(m, kind, id);
        p = m->x8B54;
        q = p->x0;
        while (q->x0 != after) {
            p = p->x4;
            q = p->x0;
        }
        node = Mem_Alloc(8);
        if (node != NULL) {
            node->x0 = e;
            node->x4 = p->x4;
            p->x4 = node;
        }
    }
}
