#include "common.h"
#include "m2c.h"

typedef struct LNode {
    s32 value;
    struct LNode *next;
} LNode;
void Mem_Free(void *);

void List_Remove(LNode **list, s32 value) {
    LNode *n = *list;
    LNode *del;

    if (n != NULL) {
        if (n->value == value) {
            del = n;
            *list = del->next;
        } else {
            while (n->next->value != value) {
                n = n->next;
            }
            del = n->next;
            n->next = del->next;
        }
        Mem_Free(del);
    }
}
