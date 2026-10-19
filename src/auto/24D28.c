#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0x74]; void *x74[12]; s32 xA4; } Sub1100;
typedef struct { u8 pad[0x1100]; Sub1100 *x1100; } Game;
extern Game *gGame;
extern s32 Array_Find(void *, void **, s32);
extern void Array_SwapRemove(void **, s32, s32);
extern void Car_Destroy(void *);
typedef struct { u8 pad[0x40C]; void *x40C[(0xD12 - 0x40C) / 4]; u8 pad2[2]; s16 xD12; } Owner;
void Car_RemoveFromLists(Owner *o, void *e) {
    s32 i;
    i = Array_Find(e, gGame->x1100->x74, gGame->x1100->xA4);
    if (i >= 0) {
        Array_SwapRemove(gGame->x1100->x74, i, gGame->x1100->xA4);
        gGame->x1100->xA4--;
    }
    i = Array_Find(e, o->x40C, o->xD12);
    if (i >= 0) {
        Array_SwapRemove(o->x40C, i, o->xD12);
        o->xD12--;
    }
    Car_Destroy(e);
}
