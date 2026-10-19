#include "common.h"
#include "m2c.h"

typedef struct {
    u8 pad0[0x10];
    s32 x10;
} State;

extern u8 D_800972F0;
extern u8 D_8009734C;
extern u8 D_8009734D;
u8 Pak_Open(s32 *);
void func_80050684(s32 *);
u8 func_80050910(s32);
void Pak_WriteSave(s32 *);
void func_80063564(s32, s32);
void func_80063380(s32);

s32 func_8003E1C0(State *st) {
    s32 a;
    s32 b;

    switch (st->x10) {
        case 0:
            if (!Pak_Open(&a)) {
                return 1;
            }
            func_80050684(&a);
            if (func_80050910(a)) {
                func_80063564(0xB, 1);
            }
            break;
        case 1:
            if (Pak_Open(&b)) {
                if (!func_80050910(b)) {
                    return 1;
                }
                Pak_WriteSave(&b);
                if (!func_80050910(b)) {
                    return 1;
                }
                if (D_800972F0) {
                    func_80063564(0x2E, 1);
                } else {
                    func_80063380(2);
                    func_80063564(0x19, 1);
                    st->x10 = 0;
                }
            } else {
                D_8009734C = 0;
                D_8009734D = 1;
                func_80063564(6, 1);
            }
            break;
        case 2:
            if (D_800972F0) {
                func_80063564(0x2E, 1);
            } else {
                func_80063380(2);
                func_80063564(0x19, 1);
                st->x10 = 0;
            }
            break;
    }
    return 1;
}
