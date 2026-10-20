typedef struct { s16 x0; u8 pad[0xA]; u32 xC; s32 x10; } Obj;
typedef struct { u8 pad[0x34]; u32 x34; Obj *x38; } Slot;
typedef struct { u8 pad[0x10]; s32 x10; u8 pad2[0x74 - 0x14]; Slot *x74[12]; s32 xA4; } Race;
typedef struct { u8 pad[0x340]; Obj *x340[(0x1044 - 0x340) / 4]; s16 x1044; u8 pad2[0x1100 - 0x1046]; Race *x1100; } Game;
extern Game *gGame;
extern s32 D_800958A8[];
extern void InitLookupTables(void);
extern void Ai_PickTarget(void *);

void Race_AssignSlotObjects(void) {
    Obj *list[12];
    s32 i;
    s32 j;
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
        if (n == 12) {
            break;
        }
    }
    for (i = 0; i < gGame->x1100->xA4; i++) {
        s = gGame->x1100->x74[i];
        do { s->x38 = NULL; } while (0);
        do { j = 0; } while (0);
        for (; j < 12; j++) {
            o = list[j];
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
/* agent60: 5 words off. Key fix: Ai_PickTarget takes the slot (Ai_PickTarget(s), s already in a0),
 * which fixed the s/xC register swap. do { s->x38 = NULL; } while (0); do { j = 0; } while (0); fixes
 * the j/ptr swap (a2/a3) but moves sw zero,56(a0) up before the two moves. Without the do-wraps
 * (Race_AssignSlotObjects.c in build/agent60) it is 10 words: j/ptr swapped. Both versions still
 * have k=0/n=0 swapped at the first blez: combine merges k=0 into the copied exit test (insn after
 * the loads), so n=0 (earlier luid) is scheduled first and reorg puts k=0 in the delay slot; the
 * original has n=0 in the slot. Tried k=0;n=0 orders, n=k, chained, do-while with if pre-test,
 * n==12 inside the if, condition forms. */
