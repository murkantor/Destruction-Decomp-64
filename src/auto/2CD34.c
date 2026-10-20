#include "common.h"
#include "m2c.h"

typedef struct { void *ptr; u8 pad[0x64]; } Slot68;
typedef struct { u8 pad[0x70]; void *x70; } Race;
typedef struct { u8 pad[0xC]; s16 mode; u8 pad0E[0x1100 - 0xE]; Race *race; } Game;
extern Game *gGame;
extern Slot68 D_800958DC[];
extern s8 D_800D31CC[];
void Mem_Free(void *);
void Cars_FreeObjects(void);
void Track_FreeResources(void *);
void Race_Free(Race *);

void Game_FreeModeResources(Game *game) {
    s8 i;

    switch (gGame->mode) {
    case 0:
        for (i = 0; i < D_800D31CC[0]; i++) {
            Mem_Free(D_800958DC[i].ptr);
            D_800958DC[i].ptr = NULL;
        }
        break;
    case 1:
        for (i = 0; i < D_800D31CC[0]; i++) {
            Mem_Free(D_800958DC[i].ptr);
            D_800958DC[i].ptr = NULL;
        }
        Cars_FreeObjects();
        break;
    case 2:
        Track_FreeResources(gGame->race->x70);
        Race_Free(game->race);
        game->race = NULL;
        for (i = 0; i < D_800D31CC[0]; i++) {
            Mem_Free(D_800958DC[i].ptr);
            D_800958DC[i].ptr = NULL;
        }
        break;
    }
}
