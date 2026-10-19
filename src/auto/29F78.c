#include "common.h"
#include "m2c.h"

typedef struct { s32 x0; s32 x4; u8 pad[0xC]; } Part;
typedef struct Car {
    u8 pad[0xD];
    u8 xD;
    u8 padE[0x20 - 0xE];
    s32 x20;
    u8 pad24[0x30 - 0x24];
    s32 x30;
    u8 pad34[0xA6C - 0x34];
    Part parts[8];
    u8 padB0C[0xBE2 - 0xA6C - 8 * 0x14];
    u8 xBE2;
} Car;
typedef struct { u8 pad[0x10A8]; u64 x10A8; u8 pad10B0[0x10FC - 0x10B0]; s16 x10FC; } Game;
typedef struct { u8 pad[8]; Car *x8; } Slot;
typedef struct { u8 pad[0x38]; s32 x38; } Rec;

extern Game *gGame;
extern s32 D_800958A8;
extern s32 D_800958AC;
extern Car **D_80096074;
extern Slot *D_80096078;
extern Slot *D_8009607C;
extern Rec *D_80096080;
extern Rec *D_80096084;
extern void *gUi;
extern void Car_ApplyDamage(Car *, s32, f32, s32);
extern void func_800259A8(Car *);
extern u8 func_800267B4(Rec *, Car *);
extern void func_800266C8(Slot *);
extern void Ui_RemoveElement(void *, s32);
extern void Announcer_QueueRandom(s32 *list, s32 n, s32 arg, s32 force, s32 unique);
extern void Announcer_Request(s32 id, s32 arg1, s32 check);
extern void func_80029A18(Car *, Car *, s32, s32);
extern void func_800045B0(Car *, s32, s32, Car *, s32, s32);

void Car_OnCollision(Car *a, s32 pa, Car *b, s32 pb, f32 force) {
    s32 dmgA;
    s32 dmgB;
    s32 num;
    s32 den;
    u8 flag;

    if (gGame->x10FC == 0x1D) {
        return;
    }
    if (a->xD) {
        Car_ApplyDamage(b, pb, force, 4);
        return;
    }
    if (b->xD) {
        Car_ApplyDamage(a, pa, force, 4);
        return;
    }
    if (D_800958A8 == 3) {
        if (D_800958AC == 1) {
            if (D_80096074[0] == a) {
                if (D_80096074[1] != b || (f64)gGame->x10A8 > 3000000000.0) {
                    func_800259A8(b);
                }
                return;
            }
            if (D_80096074[0] == b) {
                if (D_80096074[1] != a || (f64)gGame->x10A8 > 3000000000.0) {
                    func_800259A8(a);
                }
            }
            return;
        }
        if (D_800958AC == 2) {
            flag = 0;
            if (D_80096078->x8 == a && func_800267B4(D_80096080, b)) {
                b->x20++;
                D_80096080->x38 += 5;
                func_800266C8(D_80096078);
                flag = 1;
                Ui_RemoveElement(gUi, 0x135);
            }
            if (D_80096078->x8 == b && func_800267B4(D_80096080, a)) {
                a->x20++;
                D_80096080->x38 += 5;
                func_800266C8(D_80096078);
                flag = 1;
                Ui_RemoveElement(gUi, 0x135);
            }
            if (flag) {
                s32 list[2];
                list[0] = 0x7E;
                list[1] = 0x7F;
                Announcer_QueueRandom(list, 2, 0, 0, 0);
                flag = 0;
            }
            if (D_8009607C->x8 == a && func_800267B4(D_80096084, b)) {
                b->x20++;
                D_80096084->x38 += 5;
                func_800266C8(D_8009607C);
                flag = 1;
                Ui_RemoveElement(gUi, 0x134);
            }
            if (D_8009607C->x8 == b && func_800267B4(D_80096084, a)) {
                a->x20++;
                D_80096084->x38 += 5;
                func_800266C8(D_8009607C);
                flag = 1;
                Ui_RemoveElement(gUi, 0x134);
            }
            if (flag) {
                s32 list[2];
                list[0] = 0x7D;
                list[1] = 0x7F;
                Announcer_QueueRandom(list, 2, 0, 0, 0);
            }
            return;
        }
    }

    num = a->xBE2 ? 100 : a->parts[pa].x4;
    if (b->xBE2) den = 100; else den = b->parts[pb].x0;
    dmgA = (s32)(num * force) / den;
    func_80029A18(a, b, pb, dmgA);
    if (gGame->x10FC == 0x1C && a->x30 == 0 && dmgA >= 2 && (pa == 0 || pa == 4 || pa == 1)) {
        if (pb == 0) {
            Announcer_Request(0xB8, 0, 1);
        } else if (pb == 7) {
            Announcer_Request(0x65, 0, 1);
        }
    }

    num = b->xBE2 ? 100 : b->parts[pb].x4;
    if (a->xBE2) den = 100; else den = a->parts[pa].x0;
    dmgB = (s32)(num * force) / den;
    func_80029A18(b, a, pa, dmgB);
    if (gGame->x10FC == 0x1C && b->x30 == 0 && dmgB >= 2 && (pb == 0 || pb == 4 || pb == 1)) {
        if (pa == 0) {
            Announcer_Request(0xB8, 0, 1);
        } else if (pa == 7) {
            Announcer_Request(0x65, 0, 1);
        }
    }
    func_800045B0(a, dmgA, pa, b, dmgB, pb);
}
