#include "common.h"
#include "m2c.h"

typedef struct {
    u8 pad[0x334];
    u8 x334[1];
} Game;

typedef struct {
    u8 pad[0x10];
    s32 x10;
} Obj4BB9C;

extern Game *gGame[];
extern s32 D_800970F4;
extern s32 D_800970F8;
extern s32 D_800964F0[];
extern u8 D_80097337;
extern u8 D_80097335;
extern u8 D_80097336;
extern s32 D_80097D50;
extern u8 D_8009734C;
extern u8 D_8009734D;
extern u8 D_800972F0;

void ObjList_Remove(void *, s32);
s32 CarObj_Free(s32);
u8 Pak_Open(s32 *);
u8 Pak_HandleError(s32);
void Game_SetNextMode(s32, s32);
void func_80063380(s32);

s32 Podium_Continue(Obj4BB9C *obj) {
    s32 next;
    s32 tmp;
    s32 r = -1;

    ObjList_Remove(gGame[0]->x334, D_800970F4);
    if (D_800970F8 != 0) {
        ObjList_Remove(gGame[0]->x334, D_800970F8);
    }
    ObjList_Remove(gGame[0]->x334, D_800964F0[0]);
    D_800964F0[0] = CarObj_Free(D_800964F0[0]);
    if (D_80097337) {
        next = 0x2D;
    } else if (D_80097335) {
        next = 0x26;
    } else if (D_80097336) {
        if (D_80097D50 != r) {
            D_8009734C = 0;
            D_8009734D = 1;
            if (Pak_Open(&tmp)) {
                if (Pak_HandleError(tmp)) {
                    next = 0xB;
                } else {
                    next = 6;
                }
            } else {
                next = 6;
            }
        } else if (D_800972F0) {
            next = 0x2E;
        } else {
            func_80063380(2);
            next = 0x19;
        }
    } else if (D_800972F0) {
        Game_SetNextMode(0x2E, 1);
        goto end;
    } else {
        func_80063380(2);
        next = 0x19;
    }
    Game_SetNextMode(next, 1);
    r = 0;
end:
    obj->x10 = r;
    return 1;
}
