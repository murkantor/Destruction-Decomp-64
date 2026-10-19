#include "common.h"
#include "m2c.h"

typedef struct { u8 x0; u8 x1; u8 x2; u8 x3; u8 x4; u8 pad5[0x13]; } Slot;
extern u8 D_801253F8;
extern u8 D_801253F9;
extern Slot D_8012540C[4];
void func_8002D3B8(s32);

void func_8002D320(void) {
    s32 i;

    D_801253F8 = 0;
    D_801253F9 = 0;
    for (i = 0; i < 4; i++) {
        D_8012540C[i].x0 = 0;
        D_8012540C[i].x1 = 0;
        D_8012540C[i].x2 = 0;
        D_8012540C[i].x3 = 0;
        D_8012540C[i].x4 = 0;
    }
    func_8002D3B8(0);
    func_8002D3B8(1);
    func_8002D3B8(2);
    func_8002D3B8(3);
}
