#include "common.h"
#include "m2c.h"

typedef struct LNode {
    s32 value;
    struct LNode *next;
} LNode;

s32 List_Contains(LNode **list, s32 value) {
    LNode *n = *list;
    u8 found = 0;

    if (n != NULL) {
        found = n->value == value;
        while (n->next != NULL) {
            n = n->next;
            if (n->value == value) {
                found = 1;
                break;
            }
        }
    }
    return found;
}
