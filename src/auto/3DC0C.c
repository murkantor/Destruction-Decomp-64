#include "common.h"
#include "m2c.h"

typedef struct {
    u8 pad0[0x10];
    s32 x10;
} Item;
extern u8 D_8009734C;
extern u8 D_8009734D;
u8 Pak_Open(s32 *);
u8 func_80050910(s32);
void Pak_WriteSave(s32 *);
void func_80063380(s32);
void func_80063564(s32, s32);

s32 func_8003D00C(Item *item) {
    s32 err;
    s32 code;

    switch (item->x10) {
        case 0:
            code = 9;
            break;
        case 1:
            if (!Pak_Open(&err)) {
                code = 5;
                break;
            }
            if (!func_80050910(err)) {
                code = 5;
                break;
            }
            Pak_WriteSave(&err);
            if (func_80050910(err)) {
                func_80063380(2);
                func_80063564(0x19, 1);
                item->x10 = 0;
                D_8009734C = 0;
                D_8009734D = 0;
            }
            return 1;
        default:
            return 1;
    }
    func_80063564(code, 1);
    return 1;
}
