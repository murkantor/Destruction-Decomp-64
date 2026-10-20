#include "common.h"
#include "m2c.h"

typedef struct {
    u8 pad[0x74];
    s32 cars[RACE_CARS];
    s32 numCars;
} Race;
typedef struct {
    u8 pad[0x1100];
    Race *race;
} Game;
extern Game *gGame;

void Car_AddToLists(u8 *obj, s32 car) {
    Race *race = gGame->race;
    s16 i;

    race->cars[race->numCars++] = car;
    i = (*(s16 *)(obj + 0xD12))++;
    ((s32 *)(obj + 0x40C))[i] = car;
}
