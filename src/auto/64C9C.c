#include "common.h"
#include "m2c.h"

typedef struct LNode { s32 value; struct LNode *next; } LNode;
void List_MoveNodeById(LNode **src, LNode **dst, s32 value) {
LNode *n; LNode *prev; LNode *h;
h = *src; if (h == *dst) return; prev = h; if (h->value == value) { n = h; *src = h->next; } else {
while (prev->next->value != value) prev = prev->next;
n = prev->next; prev->next = n->next;
}
    n->next = NULL;
    prev = *dst;
    if (prev != NULL) n->next = prev;
    *dst = n;
}
