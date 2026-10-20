#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0x10]; s32 x10; } Foo;
extern s32 D_80097D50;
u8 Pak_Open(s32 *);
u8 Menu_HandleSelect(s32);
void func_80063380(s32);
void Game_SetNextMode(s32, s32);

s32 PakMenu_NoteExistsSelect(Foo *a) {
    s32 tmp;
    switch (a->x10) {
    case 0:
        if (Pak_Open(&tmp)) {
            if (Menu_HandleSelect(tmp)) {
                Game_SetNextMode(0xB, 1);
            } else {
                Game_SetNextMode(5, 1);
            }
        } else {
            Game_SetNextMode(5, 1);
        }
        break;
    case 1:
        Game_SetNextMode(0xA, 1);
        break;
    case 2:
        D_80097D50 = -1;
        func_80063380(2);
        Game_SetNextMode(0x19, 1);
        break;
    }
    a->x10 = 0;
    return 1;
}
