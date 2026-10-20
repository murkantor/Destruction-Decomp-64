#include "common.h"
#include "m2c.h"

typedef struct Node {
    s32 data;
    struct Node *next;
    struct Node *prev;
} Node;

typedef struct {
    Node *head;
    s32 x4;
    u8 (*cmp)(s32, s32);
} List;

typedef struct {
    u8 pad0[0x74];
    s32 x74[RACE_CARS];
    s32 xA4;
} Track;

typedef struct {
    u8 pad0[0x1100];
    Track *x1100;
} Game;

extern Game *gGame[];
void *Mem_Alloc(u32 size);

void List_BuildSorted(List *list) {
    Node *new;
    Node *cur;
    Node *p;
    s32 d;
    s32 j;
    s32 i;

    new = Mem_Alloc(0xC);
    d = gGame[0]->x1100->x74[0];
    new->next = NULL;
    new->prev = NULL;
    new->data = d;
    list->head = new;
    for (i = 1; i < gGame[0]->x1100->xA4; i++) {
        new = Mem_Alloc(0xC);
        d = gGame[0]->x1100->x74[i];
        new->next = NULL;
        new->prev = NULL;
        new->data = d;
        cur = list->head;
        for (j = 0; j < i; j++) {
            if (list->cmp(new->data, cur->data)) {
                p = cur->prev;
                if (p == NULL) {
                    new->next = cur;
                    cur->prev = new;
                    list->head = new;
                } else {
                    cur->prev = new;
                    new->prev = p;
                    new->next = cur;
                    p->next = new;
                }
                break;
            }
            if (cur->next == NULL) {
                cur->next = new;
                new->prev = cur;
                break;
            }
            cur = cur->next;
        }
    }
}
