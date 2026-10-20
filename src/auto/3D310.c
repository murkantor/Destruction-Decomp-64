#include "common.h"
#include "m2c.h"

typedef struct { u8 pad0[0x10FC]; s16 x10FC; } Game;
typedef struct { u8 pad0[0x10]; s32 x10; } Obj3C710;
extern Game *gGame;
extern s16 D_8011EEEA;
extern u8 D_8009734C;
extern u8 D_8009734D;
u8 Pak_Open(s32 *);
void Pak_SaveGame(s32 *);
u8 Menu_HandleSelect(s32);
void func_80063380(s32);
void Game_SetNextMode(s32, s32);
void Timer_Start(void *, s64);

s32 PakMenu_NoNoteSelect(Obj3C710 *obj) {
    s32 x;

    if (obj->x10 == 1) {
        Game_SetNextMode(8, 1);
        obj->x10 = 0;
    } else {
        if (Pak_Open(&x) && x == 5) {
            Pak_SaveGame(&x);
            if (Menu_HandleSelect(x)) {
                func_80063380(2);
                Game_SetNextMode(0x19, 1);
                obj->x10 = 0;
            }
        }
        if (gGame->x10FC == D_8011EEEA) {
            Game_SetNextMode(6, 1);
            obj->x10 = 0;
        }
    }
    Timer_Start(obj, 300000000);
    D_8009734C = 0;
    D_8009734D = 0;
    return 1;
}
