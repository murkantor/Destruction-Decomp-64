/*
 * The doubled field ([game] double_field): races with twice the cars, up to
 * RACE_CARS (24), the extra ones computer cars. [game] cpu_unlockables lets the
 * computer drive the unlockable cars too. The race code asks here while it loads
 * the cars (Race_LoadCarResources) and places them (Race_PlaceCarsOnGrid); the
 * announcer asks before calling out the cars left (Port_FieldAnnounceCarsLeft).
 */
#include <SDL3/SDL.h>

#include "port.h"
#include "port_config.h"

#define VANILLA_CARS 12
#define MAX_CARS 24

/* How many computer cars to add to a race of `cars` */
s32 Port_FieldExtraCars(s32 cars) {
    if (!gPortConfig.doubleField || cars <= 0) {
        return 0;
    }
    return cars < MAX_CARS - cars ? cars : MAX_CARS - cars;
}

/* The car type a computer car of base type `type` (0-11) drives: the type, or with
   cpu_unlockables, as often its unlockable twin (type + 12) */
s32 Port_FieldCarType(s32 type) {
    if (gPortConfig.cpuUnlockables && type >= 0 && type < 12 && SDL_rand(2) == 1) {
        return type + 12;
    }
    return type;
}

/* The announcer may call out how many cars are left: only once they are within the
   original game's field */
s32 Port_FieldAnnounceCarsLeft(s32 left) {
    return left <= VANILLA_CARS - 1;
}
