#include "common.h"
#include "m2c.h"

typedef struct { s16 x0; u8 pad[0xA]; u32 xC; s32 x10; } Obj;
typedef struct { u8 pad[0x34]; u32 x34; Obj *x38; } Slot;
typedef struct { u8 pad[0x10]; s32 x10; u8 pad2[0x74 - 0x14]; Slot *x74[RACE_CARS]; s32 xA4; } Race;
typedef struct { u8 pad[0x340]; Obj *x340[(0x1044 - 0x340) / 4]; s16 x1044; u8 pad2[0x1100 - 0x1046]; Race *x1100; } Game;
extern Game *gGame;
extern s32 D_800958A8[];
extern void InitLookupTables(void);
extern void Ai_PickTarget(void *);
#ifdef TARGET_PC
/* The doubled field (port/src/field.c): the extra cars have driver objects of their
   own (AiDriver_LoadAll). A car takes a matching object no earlier car has, else
   (there is none) one an earlier car has, as on the N64; up to 12 cars of distinct
   types this is the N64's choice. */
static s32 DriverTaken(Obj *o, s32 i) {
    s32 j;

    for (j = 0; j < i; j++) {
        if (gGame->x1100->x74[j]->x38 == o) {
            return 1;
        }
    }
    return 0;
}
#endif

void Race_AssignSlotObjects(void) {
#ifdef TARGET_PC
    Obj *list[RACE_CARS];
    s32 found;
    s32 pass;
#else
    Obj *list[12];
#endif
    s32 i;
    s32 k;
    s32 n;
    Slot *s;
    Obj *o;

    InitLookupTables();
    n = 0;
    for (k = 0; k < gGame->x1044; k++) {
        if (gGame->x340[k]->x0 == 7) {
            list[n++] = gGame->x340[k];
        }
#ifdef TARGET_PC
        if (n == RACE_CARS) {
#else
        if (n == 12) {
#endif
            break;
        }
    }
#ifdef TARGET_PC
    found = n;
#endif
    for (i = 0; i < gGame->x1100->xA4; i++) {
        s = gGame->x1100->x74[i];
        s->x38 = NULL;
#ifdef TARGET_PC
        for (pass = 0; pass < 2 && s->x38 == NULL; pass++)
        for (n = 0; n < found; n++) {
            o = list[n];
            if (pass == 0 && DriverTaken(o, i)) {
                continue;
            }
#else
        for (n = 0; n < 12; n++) {
            o = list[n];
#endif
            if (D_800958A8[0] == 3 && D_800958A8[1] == 2 && D_800958A8[-1] == 2) {
                if (i == o->xC) {
                    s->x38 = o;
                    break;
                }
            } else if (s->x34 == o->xC || (s->x34 >= 12 && s->x34 - 12 == o->xC)) {
                s->x38 = o;
                break;
            }
        }
        switch (gGame->x1100->x10) {
        case 0:
            o->x10 = 7;
            break;
        case 1:
            Ai_PickTarget(s);
            o->x10 = 0;
            break;
        }
    }
}
