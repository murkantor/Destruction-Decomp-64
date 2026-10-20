#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0x10]; s32 x10; } Menu;
extern u8 D_800964E1;
extern void *gUi[];
extern char D_80123BF2[];
extern char D_800BA288[];
void Pak_RepairId(void);
void Game_SetNextMode(s32, s32);
void Ui_RemoveElement(void *, s32);
s32 sprintf(char *, const char *, ...);

s32 PakMenu_DamagedRepair(Menu *m) {
    if (D_800964E1) {
        Pak_RepairId();
        Game_SetNextMode(5, 1);
        D_800964E1 = 0;
        m->x10 = 0;
    } else {
        Ui_RemoveElement(gUi[0], 0x17A);
        Ui_RemoveElement(gUi[0], 0x17C);
        Ui_RemoveElement(gUi[0], 0x130);
        sprintf(D_80123BF2, D_800BA288);
        D_800964E1 = 1;
        m->x10 = 0;
    }
    return 1;
}
