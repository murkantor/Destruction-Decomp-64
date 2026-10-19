#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0x10]; s32 x10; } Obj4F2A8;
extern u8 *gGame;
extern s32 D_800964F0[];
extern u8 D_80097335;
extern s32 D_80097D50;
extern u8 D_8009734C;
extern u8 D_8009734D;
extern u8 D_800972F0;
void func_8006417C(void *, s32);
s32 func_8003E9F8(s32);
u8 Pak_Open(s32 *);
u8 Menu_HandleSelect(s32);
void func_80063564(s32, s32);
void func_80063380(s32);

s32 func_8004F2A8(Obj4F2A8 *arg0) {
    s32 sp10;

    func_8006417C(gGame + 0x334, D_800964F0[0]);
    D_800964F0[0] = func_8003E9F8(D_800964F0[0]);
    if (D_80097335) {
        arg0->x10 = 0;
        func_80063564(0x26, 1);
    } else if (D_80097D50 != -1) {
        D_8009734C = 0;
        D_8009734D = 1;
        if (Pak_Open(&sp10)) {
            if (Menu_HandleSelect(sp10)) {
                func_80063564(0xB, 1);
            } else {
                func_80063564(6, 1);
            }
        } else {
            func_80063564(6, 1);
        }
    } else if (D_800972F0) {
        func_80063564(0x2E, 1);
    } else {
        func_80063380(2);
        func_80063564(0x19, 1);
        arg0->x10 = 0;
    }
    return 1;
}
