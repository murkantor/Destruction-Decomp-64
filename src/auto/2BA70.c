#include "common.h"
#include "m2c.h"

typedef struct {
    u8 pad0[0x10];
    s32 x10;
    u8 pad14[4];
    s32 x18;
    s32 x1C;
    s32 x20;
    u8 pad24[0xC];
    s32 x30;
    s32 x34;
    u8 pad38[0x30];
    u64 laps[3];
    u64 x80;
} Car;
typedef struct Node {
    Car *car;
    struct Node *next;
} Node;
typedef struct {
    u8 pad0[0x10];
    s32 x10;
    Node *x14;
    u8 pad18[0x5C];
    Car *cars[12];
    s32 xA4;
} Race;
typedef struct { u8 pad[0x1100]; Race *x1100; } Game;

extern Game *gGame[];
extern s32 D_800958A8;
extern s32 D_800958AC;
extern u32 D_800958B4[];
extern s32 D_80095D04[];
extern s32 D_80095D34[];
extern s32 D_80095D64[];
extern s32 D_80095DF4[];
extern s32 D_80095FF8[];
extern u64 D_80095E28[][5];
extern s32 func_8001B298(s32);
extern void Standings_Resort(Node **, Car *);

void Results_TallyStandings(void) {
    Race *race = gGame[0]->x1100;
    u8 again;
    s32 i, j;
    s32 sa, sb;
    Node *node;
    s32 id;
    u64 total;

    again = 1;
    if (D_800958A8 == 3) {
        again = !(D_800958AC == 1 || D_800958AC == 2);
    }
    while (again) {
        again = 0;
        for (i = 0; i < gGame[0]->x1100->xA4; i++) {
            Car *a = gGame[0]->x1100->cars[i];
            sa = a->x18 + a->x20;
            if (D_800958B4[0] >= 4) {
                sa += a->x1C;
            }
            for (j = 0; j < gGame[0]->x1100->xA4; j++) {
                Car *b = gGame[0]->x1100->cars[j];
                sb = b->x18 + b->x20;
                if (D_800958B4[0] >= 4) {
                    sb += gGame[0]->x1100->cars[i]->x1C;
                }
                if (sa == sb && i != j && gGame[0]->x1100->cars[i]->x30 == 1) {
                    gGame[0]->x1100->cars[i]->x18++;
                    Standings_Resort(&race->x14, gGame[0]->x1100->cars[i]);
                    again = 1;
                }
            }
        }
    }
    node = race->x14;
    for (i = 0; i < gGame[0]->x1100->xA4; i++) {
        id = func_8001B298(node->car->x34);
        if (D_80095D04[id] == -1) {
            D_80095D04[id] = 0;
        }
        if (D_800958A8 == 3 && D_800958AC == 1) {
            D_80095D04[id] = node->car->x18;
        } else {
            D_80095D04[id] += node->car->x18;
        }
        if (D_80095D34[id] == -1) {
            D_80095D34[id] = 0;
        }
        D_80095D34[id] += node->car->x20;
        if (D_80095D64[id] == -1) {
            D_80095D64[id] = 0;
        }
        D_80095D64[id] += node->car->x1C;
        if (race->x10 == 0) {
            total = 0;
            for (j = 0; j < D_80095FF8[D_800958B4[0]]; j++) {
                if (node->car->laps[j] == 0 || (f64)total == 10000.0) {
                    total = 10000;
                } else {
                    total += node->car->laps[j];
                }
                D_80095E28[id][j] = node->car->laps[j];
            }
            D_80095E28[id][3] = node->car->x80;
            D_80095E28[id][4] = total;
        } else if (node->car->x10 > 0) {
            D_80095DF4[id] = node->car->x10 * 100;
        }
        node = node->next;
    }
}
