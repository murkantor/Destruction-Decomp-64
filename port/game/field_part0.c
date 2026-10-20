/*
 * The doubled field ([game] double_field, port/src/field.c): per-car tables the
 * game keeps 12 (or 13) long, RACE_CARS long on PC. Files using one say so under
 * TARGET_PC:
 *
 *   #define D_800D1B08 gFieldLastPos   each slot's position at its last track check
 *   #define D_800D1C20 gFieldCarIds    the type of each car, in the order they were made
 *
 * The game finds a car's "slot" in per-car tables by its type (car->x34), looking it
 * up in D_800D1C20 (Race_FindCarSlot). The extra cars are of the computer cars'
 * types, so the lookup gives an extra car its twin's slot. Port_Field0CarSlot gives
 * each extra car its own slot instead, its index in the race's car list (12-23).
 * A race of up to 12 cars uses the first 12 entries, as before.
 */
#include "common.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef struct { u8 pad[0x74]; void *cars[RACE_CARS]; s32 ncars; } Race;
typedef struct { u8 pad[0x1100]; Race *race; } Game;

extern Game *gGame;
extern s32 D_800D1C20[];    /* 12: the original of gFieldCarIds */
extern s32 D_80095DF4[];    /* 13: each slot's place when it was wrecked (derbies) */

Vec3f gFieldLastPos[RACE_CARS];
s32 gFieldCarIds[RACE_CARS];
extern s32 gFieldD_80095DF4[]; /* D_80095DF4's slots past 11 (field_part1.c) */

/* Records the type of the car made as the race's car `index` (Car_Create): in
   gFieldCarIds, and the first 12 in D_800D1C20 too, for code still reading it */
void Port_Field0SetCarType(s32 index, s32 type) {
    if (index < 0 || index >= RACE_CARS) {
        return;
    }
    if (index < 12) {
        D_800D1C20[index] = type;
    }
    gFieldCarIds[index] = type;
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

/* D_80095DF4[slot]; its slots past 11 are kept where Results_TallyStandings (2BA70.c,
   FIELD_SLOT) keeps them */
s32 *Port_Field0WreckPlace(s32 slot) {
    if (slot >= 12) {
        return &gFieldD_80095DF4[(slot < RACE_CARS ? slot : RACE_CARS) - 12];
    }
    return &D_80095DF4[slot];
}

/* Whether the race holds more than the N64's 12 cars. Cars of one type are then not
   one car: code that tells cars apart by type compares the cars themselves. */
s32 Port_Field0Doubled(void) {
    return gGame->race != NULL && gGame->race->ncars > 12;
}

extern void *gUi[];
extern char gSaveSlotText[][0x32];
void Ui_AddElement(void *, s32, s32);
s32 sprintf(char *, const char *, ...);
f32 Font_CenterX(char *, s32, f32);
void Ui_SetElementX(void *, s32, s32, f32);

/* The finish's place past 12th (Race_WaitForOthers, Race_UpdateFinish): the game has
   sprites ("1st" to "12th") and announcer call-outs for 12 places only, so the
   place, from 0, is written out on the text line under "you finished", centred as
   the game centres those lines (the font has capitals only) */
void Port_FieldShowPlace(s32 place) {
    s32 n = place + 1;
    const char *sfx = "TH";

    if (n % 100 < 11 || n % 100 > 13) {
        sfx = n % 10 == 1 ? "ST" : n % 10 == 2 ? "ND" : n % 10 == 3 ? "RD" : "TH";
    }
    sprintf(gSaveSlotText[1], "%d%s", n, sfx);
    Ui_AddElement(gUi[0], 0, 0x17B);
    Ui_SetElementX(gUi[0], 0, 0x17B, Font_CenterX(gSaveSlotText[1], 0x111F8, 160.0f));
}
