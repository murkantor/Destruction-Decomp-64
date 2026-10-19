#include "common.h"
#include "m2c.h"

typedef struct {
    u8 pad[0x1058];
    u64 clock;
} GameState;
extern GameState *gGame;
extern s32 D_80095908;
extern u32 D_80097350;
extern u64 D_80131298;
u64 Time_Get(u64 *);
void Ui_ResetScreen(s32, s32);
s32 Menu_NextFreeSlot(s32);

void func_8004B774(void) {
    Ui_ResetScreen(0x112EC, 0x112ED);
    D_80095908 = Menu_NextFreeSlot(-1);
    D_80097350 = 0;
    D_80131298 = Time_Get(&gGame->clock) + 5000000000.0;
}
