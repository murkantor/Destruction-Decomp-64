#include "common.h"
#include "m2c.h"

typedef struct { s32 x0; s32 x4; s32 x8; s32 xC; s32 x10; } CarPart;
typedef struct {
    u8 pad0[0x10];
    s32 x10;
    u8 pad14[0x1C];
    s32 x30;
    u8 pad34[0xA6C - 0x34];
    CarPart parts[7];
    u8 padB08[0xBE0 - 0xA6C - 7 * 20];
    u8 xBE0;
    u8 xBE1;
    u8 xBE2;
} Car;
typedef struct { u8 pad[0x10]; s32 x10; } Track;
typedef struct { u8 pad[0x1100]; Track *x1100; } Game;
extern Game *gGame;
extern void *gUi;
extern s32 D_800958A8;
extern s32 D_800958AC;
extern u32 D_800958B0;
extern u8 D_800958A1;
void func_800067E0(Car *);
void func_8000F1C4(void *, Car *, s32, s32);
void func_8002A5CC(Car *, s32);

void Car_ApplyDamage(Car *c, s32 idx, f32 dmg, s32 type) {
    s32 div;
    s32 v;
    s32 amount;
    s32 side;

    if (D_800958A8 == 3 && (D_800958AC == 1 || D_800958AC == 2)) return;
    if (D_800958A8 == 2) return;
    if (gGame->x1100->x10 == 0 && (idx == 0 || idx == 4 || idx == 1)) {
        func_800067E0(c);
    }
    if (dmg < 0.5f) return;
    if (c->xBE2) div = 100;
    else div = c->parts[idx].x0;
    switch (type) {
        case 0:
            v = 50;
            break;
        case 1:
            v = 50;
            break;
        case 2:
            v = 50;
            break;
        case 3:
            v = 50;
            break;
        case 4:
            v = 50;
            break;
        case 5:
            v = 50;
            break;
        case 6:
            v = 50;
            break;
        case 7:
            v = 50;
            break;
        case 8:
            v = 50;
            break;
        case 9:
            v = 50;
            break;
        case 10:
            v = 50;
            break;
        default:
            v = 50;
            break;
    }
    if (c->x30 == 0) {
        switch (D_800958B0) {
            case 0:
                v = v * 1.0;
                break;
            case 1:
                v = v * 1.5;
                break;
            case 2:
                v = v * 1.75;
                break;
            case 3:
                v = v * 2.0;
                break;
        }
    }
    amount = ((f32)v * dmg / (f32)div) * 0.5;
    if (!c->xBE0) {
        c->x10 -= amount;
        c->parts[idx].x8 -= amount;
    }
    if (D_800958A1) {
        switch (idx) {
            case 1: side = 4; break;
            case 2: side = 5; break;
            case 3: side = 6; break;
            case 4: side = 1; break;
            case 5: side = 2; break;
            case 6: side = 3; break;
            default: side = idx; break;
        }
    } else {
        side = idx;
    }
    func_8000F1C4(gUi, c, amount, side);
    if (c->x10 < 0) c->x10 = 0;
    if (c->parts[idx].x8 < 0) c->parts[idx].x8 = 0;
    func_8002A5CC(c, idx);
}
