#include "common.h"
#include "m2c.h"

typedef struct {
    u8 pad0[0x2];
    s16 x2;
    u8 pad4[0x8];
    s32 xC;
    u8 pad10[0x3C];
    s8 x4C;
} Item;

typedef struct {
    s32 x0;
    s32 x4;
    s32 x8;
    s32 xC;
    u8 x10;
} Entry;

typedef struct {
    u8 pad0[0x104];
    Item *x104[4][60];
    u8 pad4C4[0x10];
    s32 x4D4[4];
    u8 pad4E4[0x24];
    s32 x508;
    u8 pad50C[0x1C];
    s8 x528;
} Foo;

typedef struct {
    u8 pad0[0x10FC];
    s16 x10FC;
} Game;

extern Game *gGame;
void func_80009A34(s32, s32);

void func_80009200(Foo *arg0) {
    Entry list[240];
    Entry tmp;
    s32 n;
    s32 i;
    s32 j;
    s32 gap;
    s32 swapped;
    s32 cnt;
    s32 limit;
    s32 k;
    s32 lim;
    s32 m;
    s16 mode;

    n = 0;
    for (k = 0; k < 4; k++) {
        for (i = 0; i < arg0->x4D4[k]; i++) {
            Item *p = arg0->x104[k][i];
            list[n].x0 = p->x2;
            list[n].x4 = p->xC;
            list[n].x8 = k;
            list[n].x10 = 1;
            list[n].xC = p->x4C;
            n++;
        }
    }
    mode = gGame->x10FC;
    if (mode == 25 || mode == 26) goto all;
    if (mode == 31) {
    all:
        for (i = 0; i < n; i++) {
            list[i].x10 = 0;
            func_80009A34(list[i].x8, list[i].x0);
        }
        return;
    }
    gap = n;
    do {
        gap /= 2;
        lim = n - gap;
        do {
            swapped = 0;
            for (m = 0; m < lim; m++) {
                if (list[m + gap].xC > list[m].xC) {
                    swapped = 1;
                    tmp = list[m];
                    list[m] = list[m + gap];
                    list[m + gap] = tmp;
                }
            }
        } while (swapped);
    } while (gap >= 2);
    cnt = 0;
    for (i = 0; i < n - 1; i++) {
        if (list[i].x4 == 3) {
            cnt++;
            if (cnt >= 4) {
                list[i].x10 = 0;
            }
        }
        for (j = i + 1; j < n; j++) {
            if (list[i].x0 == list[j].x0) {
                list[j].x10 = 0;
            }
        }
    }
    i = 0;
    j = 0;
    limit = 0x18 - arg0->x528 - arg0->x508;
    for (; j < n; j++) {
        if (list[j].x10 == 1) {
            i++;
            if (i < limit) {
                continue;
            }
        }
        list[j].x10 = 0;
        func_80009A34(list[j].x8, list[j].x0);
    }
}
