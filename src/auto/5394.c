#include "common.h"
#include "m2c.h"

typedef struct Car Car;
typedef struct {
    u8 pad0[0x10];
    s32 x10;
    u8 pad14[4];
    Car *x18;
    s32 x1C;
    u8 pad20[0x10];
    s32 x30;
    u8 pad34[0x3C];
    u64 x70;
} CarAI;
struct Car {
    u8 pad0[0xD];
    u8 xD;
    u8 padE[0x2A];
    CarAI *x38;
};
typedef struct { u8 pad[0x10]; s32 x10; } Track;
typedef struct { u8 pad[0x10A8]; u64 x10A8; u8 pad10B0[0x50]; Track *x1100; } Game;
extern Game *gGame;
void Ai_PickTarget(Car *);
u64 Time_Get(u64 *);

void CarAI_OnCollision(Car *a, s32 dmgA, s32 partA, Car *b, s32 dmgB, s32 partB) {
    CarAI *aiA = a->x38;
    CarAI *aiB = b->x38;
    Track *track;
    Car *tB;

    if (a->xD || b->xD) return;
    track = gGame->x1100;
    tB = aiB->x18;
    if (aiA->x18 == b) {
        aiA->x30 += dmgB;
        if (aiA->x30 >= aiA->x1C * 2) {
            aiA->x30 = 0;
            Ai_PickTarget(a);
        }
        if (aiB->x1C < 10 && !a->xD) {
            aiB->x18 = a;
            aiB->x30 = 0;
            aiB->x1C += 2;
        }
        if (track->x10 == 1 && (partA == 0 || partA == 4 || partA == 1)) {
            aiA->x70 = Time_Get(&gGame->x10A8) + 3000000000.0;
            aiA->x10 = 5;
        }
    }
    if (tB == a) {
        aiB->x30 += dmgA;
        if (aiB->x30 >= aiB->x1C * 2) {
            aiB->x30 = 0;
            Ai_PickTarget(b);
        }
        if (aiA->x1C < 10 && !b->xD) {
            aiA->x18 = b;
            aiA->x1C += 2;
        }
        if (track->x10 == 1 && (partB == 0 || partB == 4 || partB == 1)) {
            aiB->x70 = Time_Get(&gGame->x10A8) + 3000000000.0;
            aiB->x10 = 5;
        }
    }
}
