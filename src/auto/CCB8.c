#include "common.h"
#include "m2c.h"

typedef struct { void *x0; u8 pad4[0x64]; } PlayerSlot;
typedef struct { u8 pad[0x1100]; u8 *x1100; } Game;
extern Game *gGame;
extern PlayerSlot D_800958F0[];
extern char D_8007F770[][3];
s8 ItemList_IndexOf(void *, void *);
s32 sprintf(char *, const char *, ...);
extern char D_800B81A0[];

s32 Hud_FormatPosition(s32 unused, s8 i) {
#ifdef TARGET_PC
    /* the drones: a place past 99 would write 4 bytes into the 3 of the HUD's
       string (the next one after it): 99 at most */
    s32 place = ItemList_IndexOf(gGame->x1100 + 0x14, D_800958F0[i].x0);
    sprintf(D_8007F770[i], D_800B81A0, place < 99 ? place : 99);
#else
    sprintf(D_8007F770[i], D_800B81A0, ItemList_IndexOf(gGame->x1100 + 0x14, D_800958F0[i].x0));
#endif
    D_8007F770[i][2] = 0;
    return 0;
}
