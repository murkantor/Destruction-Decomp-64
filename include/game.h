#ifndef GAME_H
#define GAME_H

#include "car.h"

/* The cars in the current event. */
typedef struct Race {
    /* 0x00 */ u8 unk0[0x74];
    /* 0x74 */ Car *cars[12];
    /* 0xA4 */ s32 numCars;
} Race;

/* The game state that gGame points to. */
typedef struct Game {
    /* 0x0000 */ u8 unk0[0xC];
    /* 0x000C */ s16 mode;        /* the prototype's debug menu only applies car edits when this is 2 */
    /* 0x000E */ u8 unkE[0x10EE];
    /* 0x10FC */ s16 unk10FC;
    /* 0x10FE */ s8 unk10FE;
    /* 0x10FF */ u8 unk10FF[0x1];
    /* 0x1100 */ Race *race;
} Game;

extern Game *gGame;

#endif
