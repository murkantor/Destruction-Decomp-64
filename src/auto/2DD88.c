#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0xC]; s32 xC; } Controller;
typedef struct {
    s32 a[4];
    u8 b[4][0x68];
} PlayerTab;

extern u8 D_800D31CC;
extern u8 D_800D31F8;
extern u8 D_80095898;
extern s16 D_8011EEEA;
extern s32 osTvType;
extern s32 gControllers;
extern PlayerTab D_800958C0;

void Pad_LatchAll(void);
Controller *Pad_GetLatched(void);
void func_80063380(s32);
void Game_SetNextMode(s32, s32);
u8 Pak_Open(s32 *);
s32 func_80020F00(void *, s8);
void Cheat_ResetAll(void);
#ifdef TARGET_PC
int Port_PakScreens(void); /* port/src/pak.c: [saves] pak_screens */
#endif

void Game_RunStartupChecks(void) {
    Controller *c;
    s32 type;
    s32 i;

    D_800D31CC = 1;
    Pad_LatchAll();
    c = Pad_GetLatched();
    if (osTvType != 1 && osTvType != 2) {
        func_80063380(1);
        Game_SetNextMode(0x11, 1);
    } else if (gControllers == 0) {
        func_80063380(1);
        Game_SetNextMode(0x12, 1);
    } else if (Pak_Open(&type)) {
#ifdef TARGET_PC
        if (!Port_PakScreens()) {
            goto fail; /* no note manager or repair with START held: on to the logos */
        }
#endif
        if (type == 0) {
            if ((c->xC & 0x1000) || D_800D31F8) {
                func_80063380(1);
                Game_SetNextMode(3, 1);
            } else {
                goto fail;
            }
        } else {
            if (c->xC & 0x1000) {
                D_80095898 = 1;
                if (type == 3 || type == 4 || type == 10) {
                    func_80063380(1);
                    Game_SetNextMode(0xD, 1);
                }
            }
            if (D_8011EEEA == 0xD) {
                goto loop;
            }
            goto fail;
        }
    } else {
    fail:
        func_80063380(0);
        Game_SetNextMode(2, 1);
    }
loop:
    for (i = 0; i < 4; i++) {
        func_80020F00(D_800958C0.b[i], i);
        D_800958C0.a[i] = -1;
    }
    Cheat_ResetAll();
}
