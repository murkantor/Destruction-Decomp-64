/*
 * The doubled field ([game] double_field, port/src/field.c): per-car tables the
 * game keeps 12 (or 13) long, for the cars past the 12th.
 *
 * The game finds a car's "slot" in those tables by its type (car->x34), looking it
 * up in D_800D1C20, the types of the race's cars in the order they were made
 * (Race_FindCarSlot). The extra cars are of the computer cars' types, so the lookup
 * gives an extra car its twin's slot (or 12, "none", for an unlockable type the
 * first 12 do not have). Port_Field0CarSlot gives each extra car its own slot
 * instead, its index in the race's car list (12-23); the tables here hold those
 * slots. Slots 0-12 stay in the game's tables, so a race of up to 12 cars is
 * exactly as before.
 */
#include "common.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef struct { u8 pad[0x74]; void *cars[RACE_CARS]; s32 ncars; } Race;
typedef struct { u8 pad[0x1100]; Race *race; } Game;

extern Game *gGame;
extern Vec3f D_800D1B08[];  /* 12: each slot's position at its last track segment check */
extern s32 D_800D1C20[];    /* 12: the type of each car, in the order they were made */
extern s32 D_80095DF4[];    /* 13: each slot's place when it was wrecked (derbies) */

/* The types of all the race's cars (D_800D1C20 holds the first 12) */
s32 gField0CarTypes[RACE_CARS];
static Vec3f sLastPos[RACE_CARS];
static s32 sWreckPlace[RACE_CARS];

/* Records the type of the car made as the race's car `index` (Car_Create) */
void Port_Field0SetCarType(s32 index, s32 type) {
    if (index < 0 || index >= RACE_CARS) {
        return;
    }
    if (index < 12) {
        D_800D1C20[index] = type;
    }
    gField0CarTypes[index] = type;
}

/* The slot of `car`: `slot` (what the type lookup found) for the first 12 cars, the
   car's own index in the race's list for an extra one */
s32 Port_Field0CarSlot(void *car, s32 slot) {
    Race *race = gGame->race;
    s32 i;

    if (race == NULL) {
        return slot;
    }
    for (i = 12; i < race->ncars && i < RACE_CARS; i++) {
        if (race->cars[i] == car) {
            return i;
        }
    }
    return slot;
}

/* D_800D1B08[slot], or the extra cars' table for slots past 12 */
void *Port_Field0LastPos(s32 slot) {
    if (slot > 12 || (slot == 12 && gGame->race != NULL && gGame->race->ncars > 12)) {
        return &sLastPos[slot < RACE_CARS ? slot : RACE_CARS - 1];
    }
    return &D_800D1B08[slot];
}

/* D_80095DF4[slot], or the extra cars' table for slots past 12 */
s32 *Port_Field0WreckPlace(s32 slot) {
    if (slot > 12 || (slot == 12 && gGame->race != NULL && gGame->race->ncars > 12)) {
        return &sWreckPlace[slot < RACE_CARS ? slot : RACE_CARS - 1];
    }
    return &D_80095DF4[slot];
}

/* Whether the race holds more than the N64's 12 cars. Cars of one type are then not
   one car: code that tells cars apart by type compares the cars themselves. */
s32 Port_Field0Doubled(void) {
    return gGame->race != NULL && gGame->race->ncars > 12;
}
