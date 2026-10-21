#include "common.h"
#include "m2c.h"

extern s16 D_8011EEEA;
extern s8 D_8011EEEC;
#ifdef TARGET_PC
s16 Port_PakAuto(s16 mode); /* port/game/pakauto.c: no Controller Pak screens */
#endif

void Game_SetNextMode(s16 arg0, s8 arg1) {
#ifdef TARGET_PC
    arg0 = Port_PakAuto(arg0);
#endif
    D_8011EEEA = arg0;
    D_8011EEEC = arg1;
}
