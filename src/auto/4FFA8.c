#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0x14]; u8 *x14; u8 pad2[0x30]; } Menu48;
typedef struct { u8 pad[0x68]; } Timer68;
extern Menu48 D_800BB438;
extern u8 *D_80097338[];
extern s32 D_800964F0;
extern void *gUi[];
extern s8 D_800D31CC;
extern u8 D_80097337;
extern Timer68 D_800958F8[];
void Ui_ResetScreen(s32, s32);
s32 func_8003E644(u8 *, s32);
void func_8005F050(void *, Menu48 *);
void Ui_AddElement(void *, s32, s32);
void func_80042114(u8 *);
void Timer_Start(Timer68 *, u64);

void func_8004F3A8(void) {
    Menu48 m = D_800BB438;
    u8 *s;
    s32 i;

    Ui_ResetScreen(0x112FE, 0x112FF);
    s = D_80097338[0] + 0xC;
    D_800964F0 = func_8003E644(s, 0);
    m.x14 = s + 0x11371;
    func_8005F050(gUi[0], &m);
    Ui_AddElement(gUi[0], 2, 0x3C);
    func_80042114(s);
    D_80097337 = 0;
    D_80097338[0] = 0;
    for (i = 0; i < D_800D31CC; i++) {
        Timer_Start(&D_800958F8[i], 1000000000);
    }
}
