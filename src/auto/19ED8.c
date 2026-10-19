#include "common.h"
#include "m2c.h"

typedef struct Node { struct Node *prev; struct Node *next; u32 key; } Node;

void func_800192D8(Node *l, Node *n) {
    Node *nx;

    while (l->next != NULL) {
        if (n->key >= l->key) {
            goto before;
        }
        l = l->next;
    }
    if (l->key <= n->key) {
    before:
        l = l->prev;
        nx = l->next;
        n->prev = l;
        n->next = nx;
        l->next = n;
        n->next->prev = n;
    } else {
        l->next = n;
        n->prev = l;
        n->next = NULL;
    }
}
