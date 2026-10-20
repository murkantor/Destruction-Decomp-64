#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0xC]; u8 xC; u8 xD; u8 xE; u8 pad2[0x194 - 0xF]; s32 x194; } Slot;
typedef struct { u8 pad[0x74]; Slot *x74[RACE_CARS]; s32 xA4; } Race;
typedef struct { u8 pad[0x1100]; Race *x1100; } Game;
typedef struct { s32 x0; s32 x4; u8 x8; u8 pad[7]; } Track;
typedef struct { s32 x0; s32 x4; s32 x8; } Snd3;
extern Game *gGame;
extern Track D_801311D4[];
extern Snd3 D_8007EDC0[];
extern s32 Car_GetDriveDir(Slot *);
extern void EngineSfx_Crossfade(s32, s32, s32);
extern void Sfx_StopPair(s32, s32);
#ifdef TARGET_PC
/* the doubled field: the engine sounds of RACE_CARS cars (port/game/field_part2.c);
   D_801311D4 is the table's second word */
extern Track gFieldEngineSfx[];
extern void Port_FieldSfxStopPair(s32, s32);
#define D_801311D4 ((Track *)((u8 *)gFieldEngineSfx + 4))
#define Sfx_StopPair Port_FieldSfxStopPair
#endif

void EngineSfx_UpdateAll(s32 arg0) {
    s32 i;
    Slot *s;
    s32 v;

    for (i = 0; i < gGame->x1100->xA4; i++) {
        s = gGame->x1100->x74[i];
        if (!s->xC) {
            return;
        }
        if (s->xE) {
            D_801311D4[i].x4 = 2;
        }
        if (D_801311D4[i].x4 != 2) {
            v = Car_GetDriveDir(s);
            if (v != D_801311D4[i].x4 && D_801311D4[i].x0 == 0) {
                D_801311D4[i].x8 = 1;
                D_801311D4[i].x4 = v;
            }
        }
        switch (D_801311D4[i].x4) {
        case -1:
            EngineSfx_Crossfade(arg0, i, D_8007EDC0[s->x194].x0);
            break;
        case 0:
            EngineSfx_Crossfade(arg0, i, D_8007EDC0[s->x194].x4);
            break;
        case 1:
            EngineSfx_Crossfade(arg0, i, D_8007EDC0[s->x194].x8);
            break;
        case 2:
            if (s->xE) {
                Sfx_StopPair(arg0, i);
            }
            break;
        }
    }
}
