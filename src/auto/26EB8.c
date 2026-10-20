#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef struct {
    u8 pad0[4];
    s32 x4;
    void *x8;
    u8 padC[4];
    Vec3f x10;
    u8 pad1C[0x40 - 0x1C];
    u64 x40;
    u8 pad48;
    u8 x49;
    u8 x4A;
} Tut;
typedef struct { u8 pad[0x10A8]; u64 x10A8; } Game;
typedef struct { u8 pad[0x38]; Tut *x38; } Obj;

extern Game *gGame[];
extern Tut *D_8009607C;
extern Tut *D_80096078;
extern u64 D_800960A0;
extern u64 D_800960A8;
extern const double D_800B8D78;
extern const double D_800B8D80;
extern const double D_800B8D88;
u64 Time_Get(u64 *);
void Msg_Queue(s32, s32, s32, s32);
Vec3f Car_GetPos(void *);
void Ui_RemoveElement(void *, s32);

void func_800262B8(Obj *obj)
{
    u64 diff;
    Tut *t = D_8009607C;

    if (t == obj->x38) {
        if (t->x4 == 1) {
            if (!t->x49) {
                t->x49 = 1;
                Msg_Queue(0x81, 0, 1, 0);
            }
            diff = Time_Get(&gGame[0]->x10A8) - D_8009607C->x40;
            if (!D_8009607C->x4A && (f64)diff > D_800B8D78) {
                D_8009607C->x4A = 1;
                Msg_Queue(0x7B, 0, 1, 0);
            }
        } else {
            t->x49 = 0;
            D_8009607C->x4A = 0;
            D_8009607C->x40 = Time_Get(&gGame[0]->x10A8);
        }
    } else {
        if (D_80096078->x4 == 1) {
            D_80096078->x10 = Car_GetPos(D_80096078->x8);
            D_80096078->x10.y += D_800B8D80;
            if (!D_80096078->x49) {
                D_80096078->x49 = 1;
                Msg_Queue(0x80, 0, 1, 0);
            }
            diff = Time_Get(&gGame[0]->x10A8) - D_80096078->x40;
            if (!D_80096078->x4A && (f64)diff > D_800B8D88) {
                D_80096078->x4A = 1;
                Msg_Queue(0x7C, 0, 1, 0);
            }
        } else {
            D_80096078->x49 = 0;
            D_80096078->x4A = 0;
            D_80096078->x40 = Time_Get(&gGame[0]->x10A8);
        }
    }
    if (Time_Get(&gGame[0]->x10A8) > D_800960A0 && D_800960A0 != 0) {
        D_800960A0 = 0;
        Ui_RemoveElement(gGame[4], 0x134);
        Ui_RemoveElement(gGame[4], 0x136);
    }
    if (Time_Get(&gGame[0]->x10A8) > D_800960A8 && D_800960A8 != 0) {
        D_800960A8 = 0;
        Ui_RemoveElement(gGame[4], 0x135);
        Ui_RemoveElement(gGame[4], 0x138);
    }
}
