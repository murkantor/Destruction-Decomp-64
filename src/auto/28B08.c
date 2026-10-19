#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0xC]; u8 xC; u8 padD[0x27]; s32 x34; } Item;
typedef struct ItemNode { Item *item; struct ItemNode *next; } ItemNode;
typedef struct { u8 pad[0xA4]; s32 xA4; } Race;
typedef struct { u8 pad[0x1100]; Race *x1100; } Game;
extern Game *gGame;

s8 func_80027F08(ItemNode **list, Item *x) {
    ItemNode *n = *list;
    s8 i;

    for (i = 0; i < gGame->x1100->xA4; i++) {
        if (n->item->xC) {
            if (n->item->x34 == x->x34) {
                return i + 1;
            }
            n = n->next;
        }
    }
    return 99;
}
