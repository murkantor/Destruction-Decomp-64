#include "common.h"
#include "m2c.h"

typedef struct Node {
    struct Node *next;
    u8 pad4[0x2C];
    u8 x30;
    u8 pad31[8];
    u8 x39;
    u8 pad3A[6];
} Node;
typedef struct {
    Node *head;
    s32 count;
} List;
extern u32 D_80125C58[];
void *Pool_Alloc(void *);

Node *List_PushCopy(List *list, Node *src) {
    Node *n;

    if (D_80125C58[0] <= 0x2000) {
        return NULL;
    }
    n = Pool_Alloc((u8 *)D_80125C58 - 0x10);
    if (n != NULL) {
        *n = *src;
        n->x39 = n->x30;
        n->next = list->head;
        list->head = n;
        list->count++;
    }
    return n;
}
