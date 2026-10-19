#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0x34]; s32 x34; } Car;
typedef struct Node { Car *car; struct Node *next; struct Node *prev; } Node;
typedef struct { Node *head; s32 x4; u8 (*cmp)(Car *, Car *); } List;
typedef struct { u8 pad[0xA4]; s32 xA4; } Race;
typedef struct { u8 pad[0x1100]; Race *x1100; } Game;
extern Game *gGame[];

static inline void List_MoveDown(List *list, Car *car) {
    Node *p = list->head;
    s32 k;

    for (k = 0; k < gGame[0]->x1100->xA4; k++) {
        if (car->x34 == p->car->x34) {
            Node *nx = p->next;
            Node *pv = p->prev;
            Node *nn = nx->next;
            if (nx == NULL) {
                return;
            }
            if (nn == NULL) {
                p->next = NULL;
            } else {
                p->next = nn;
                nn->prev = p;
            }
            p->prev = nx;
            nx->next = p;
            if (pv == NULL) {
                nx->prev = NULL;
                list->head = nx;
            } else {
                nx->prev = pv;
                pv->next = nx;
            }
            break;
        }
        p = p->next;
    }
}

static inline void List_MoveUp(List *list, Car *car) {
    Node *p = list->head;
    s32 k;

    for (k = 0; k < gGame[0]->x1100->xA4; k++) {
        if (car->x34 == p->car->x34) {
            Node *pv = p->prev;
            Node *nx = p->next;
            Node *pp = pv->prev;
            if (pv == NULL) {
                return;
            }
            if (nx == NULL) {
                pv->next = NULL;
            } else {
                pv->next = nx;
                nx->prev = pv;
            }
            pv->prev = p;
            p->next = pv;
            if (pp == NULL) {
                p->prev = NULL;
                list->head = p;
            } else {
                p->prev = pp;
                pp->next = p;
            }
            break;
        }
        p = p->next;
    }
}

void Standings_Resort(List *list, Car *car) {
    Node *n;
    Node *m;
    s32 i;
    s32 j;
    u8 moved = 0;

    n = list->head;
    for (i = 0; i < gGame[0]->x1100->xA4; i++) {
        if (n->car->x34 == car->x34) {
            break;
        }
        n = n->next;
    }
    m = n->next;
    if (m != NULL) {
        for (j = i + 1; j < gGame[0]->x1100->xA4; j++) {
            if (!list->cmp(car, m->car)) {
                m = m->next;
                List_MoveDown(list, car);
                moved = 1;
            }
        }
        if (moved) {
            return;
        }
    }
    m = n->prev;
    if (m == NULL) {
        return;
    }
    for (j = 0; j < i; j++) {
        if (!list->cmp(car, m->car)) {
            return;
        }
        m = m->prev;
        List_MoveUp(list, car);
    }
}
