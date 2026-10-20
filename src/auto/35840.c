#include "common.h"
#include "m2c.h"

typedef struct Node {
    u8 pad[0x58];
    struct Node *prev;
    struct Node *next;
} Node;

typedef struct {
    u8 pad[0x164];
    Node *head;
    s32 count;
} NodeList;

void Body_LinkContact(NodeList *list, Node *item) {
    Node *p;

    list->count++;
    p = list->head;
    if (p == NULL) {
        item->next = NULL;
        item->prev = NULL;
        list->head = item;
    } else {
        while (p->next != NULL) {
            p = p->next;
        }
        item->next = NULL;
        item->prev = p;
        p->next = item;
    }
}
