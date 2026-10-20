#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0x34]; s32 x34; } Item;
typedef struct ItemNode { Item *item; struct ItemNode *next; struct ItemNode *prev; } ItemNode;
typedef struct { u8 pad[0x74 + 4 * RACE_CARS]; s32 xA4; } Race;
typedef struct { u8 pad[0x1100]; Race *x1100; } Game;
extern Game *gGame;

void ItemList_MoveUp(ItemNode **list, Item *x) {
    ItemNode *n = *list;
    ItemNode *p;
    ItemNode *nx;
    ItemNode *pp;
    s32 i;

    for (i = 0; i < gGame->x1100->xA4; i++) {
        if (x->x34 == n->item->x34) {
            p = n->prev;
            nx = n->next;
            pp = p->prev;
            if (p == NULL) {
                return;
            }
            if (nx == NULL) {
                p->next = NULL;
            } else {
                p->next = nx;
                nx->prev = p;
            }
            p->prev = n;
            n->next = p;
            if (pp == NULL) {
                n->prev = NULL;
                *list = n;
            } else {
                n->prev = pp;
                pp->next = n;
            }
            return;
        }
        n = n->next;
    }
}


void ItemList_MoveDown(ItemNode **list, Item *x) {
    ItemNode *n = *list;
    ItemNode *p;
    ItemNode *nx;
    ItemNode *nn;
    s32 i;

    for (i = 0; i < gGame->x1100->xA4; i++) {
        if (x->x34 == n->item->x34) {
            nx = n->next;
            p = n->prev;
            nn = nx->next;
            if (nx == NULL) {
                return;
            }
            if (nn == NULL) {
                n->next = NULL;
            } else {
                n->next = nn;
                nn->prev = n;
            }
            n->prev = nx;
            nx->next = n;
            if (p == NULL) {
                nx->prev = NULL;
                *list = nx;
            } else {
                nx->prev = p;
                p->next = nx;
            }
            return;
        }
        n = n->next;
    }
}
