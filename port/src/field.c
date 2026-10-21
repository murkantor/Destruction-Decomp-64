/*
 * The drones ([game] drones, 1-10): races with the computer cars times that many,
 * up to RACE_CARS (120), the extra ones computer cars, as many as the game's heap
 * holds. [game] cpu_unlockables lets the computer drive the unlockable cars too.
 * The race code asks here while it loads the cars (Race_LoadCarResources) and
 * places them (Race_PlaceCarsOnGrid); the announcer asks before calling out the
 * cars left (Port_FieldAnnounceCarsLeft).
 */
#include <SDL3/SDL.h>

#include "port.h"
#include "port_config.h"
#include "race_cars.h"
#include "heap.h"

#define VANILLA_CARS 12

static s32 sPoolScale = 1;

/* The particle and matrix pool's size is set at boot (Main_InitSystems): the
   drones setting then times the N64's, which a race's cars stay within */
s32 Port_FieldPoolScale(void) {
    sPoolScale = SDL_clamp(gPortConfig.drones, 1, 10);
    return sPoolScale;
}

/* The game's object list (gGame 0x340) holds 256; a race adds about 13 to what is
   there when the cars load (the drivers and the like), and each extra car 2 (its
   car and its driver). Past 256 the game drops objects (ObjList_Add): props, drivers */
#define OBJECTS_MAX 256
#define OBJECTS_RACE 13
#define OBJECTS_SPARE 8

/* How many computer cars to add to a race of `cars`, with `objects` in the game's
   object list (the most: Port_FieldRoomForCar stops the loading when the heap is full) */
s32 Port_FieldExtraCars(s32 cars, s32 objects) {
    s32 k = gPortConfig.drones < sPoolScale ? gPortConfig.drones : sPoolScale;
    s32 extra, room;

    if (k <= 1 || cars <= 0) {
        return 0;
    }
    extra = cars * (k - 1);
    if (extra > RACE_CARS - cars) {
        extra = RACE_CARS - cars;
    }
    room = (OBJECTS_MAX - objects - OBJECTS_RACE - OBJECTS_SPARE) / 2;
    if (extra > room) {
        Port_Log("drones: the game's object list holds %d of the %d extra cars", (int)SDL_max(room, 0), (int)extra);
        extra = SDL_max(room, 0);
    }
    return extra;
}

/* The car type a computer car of base type `type` (0-11) drives: the type, or with
   cpu_unlockables, as often its unlockable twin (type + 12) */
s32 Port_FieldCarType(s32 type) {
    if (gPortConfig.cpuUnlockables && type >= 0 && type < 12 && SDL_rand(2) == 1) {
        return type + 12;
    }
    return type;
}

/* The car type a drone (a car past the game's own field) of base type `type`
   drives: [game] drone_car, else as the computer cars (Port_FieldCarType). Never
   a player's car (`players`, `n` of them, 0-23): the game tells the players' cars
   by their type, so a player's car gives its twin, and if a player has that too,
   the default */
s32 Port_FieldDroneType(s32 type, const s32 *players, s32 n) {
    s32 car = gPortConfig.droneCar, i, twin;

    if (car < 0 || car > 23) {
        return Port_FieldCarType(type);
    }
    twin = car < 12 ? car + 12 : car - 12;
    for (i = 0; i < n; i++) {
        if (players[i] == car) {
            car = twin;
            break;
        }
    }
    for (i = 0; i < n; i++) {
        if (players[i] == car) {
            return Port_FieldCarType(type);
        }
    }
    return car;
}

/* [game] announce_all_wrecks. The announcer's cars-left lines (D_800B8840: 0x8E
   "one" to 0x97 "ten") open with the same phrase, then a pause, then the number;
   past ten cars nothing is said. With the option, a wreck there queues the ten
   line (0x97) cut at its pause (Announcer_Update asks Port_FieldAnnounceCut), one
   at a time, as the game's own queue paces it */
#define LINE_TEN 0x97
#define LINE_OPENING_NS 1250000000ull
void Msg_Queue(s32 id, s32 delay, u8 force, u8 once);

static s32 sWreckLast = 0x7FFF;
static s32 sCutPending;

void Port_FieldAnnounceWreck(s32 left, s32 racing) {
    if (!racing || left > sWreckLast) {
        sWreckLast = left; /* a new race, the countdown */
        if (!racing) {
            sCutPending = 0;
        }
        return;
    }
    if (left < sWreckLast) {
        sWreckLast = left;
        if (gPortConfig.announceAllWrecks && left >= 11 && sCutPending == 0) {
            sCutPending = 1;
            Msg_Queue(LINE_TEN, 0, 1, 0);
        }
    }
}

/* The announcer is about to play `id`: how long, in ns, if it is to be cut (0: whole) */
u64 Port_FieldAnnounceCut(s32 id) {
    if (id == LINE_TEN && sCutPending > 0) {
        sCutPending--;
        return LINE_OPENING_NS;
    }
    return 0;
}

/* The announcer may call out how many cars are left: only once they are within the
   original game's field */
s32 Port_FieldAnnounceCarsLeft(s32 left) {
    return left <= VANILLA_CARS - 1;
}

/* The largest free block of the game's heap */
static u32 heapLargest(void) {
    const FreeBlock *b;
    u32 most = 0;

    for (b = gMainHeap.free; b != NULL; b = b->next) {
        if (b->size > most) {
            most = b->size;
        }
    }
    return most;
}

/* Room in the game's heap for one more drone: its resources now, its model, driver
   and the rest when the race is set up (about 45 KB a car; 32 MB of RAM holds the
   120 with some 25 MB to spare), and what the race needs after them */
#define HEAP_RESERVE (2u * 1024 * 1024)

s32 Port_FieldRoomForCar(s32 cars) {
    if (heapLargest() > HEAP_RESERVE) {
        return 1;
    }
    Port_Log("drones: the heap is full at %d cars (%u bytes free)", (int)cars, (unsigned)gMainHeap.bytesFree);
    return 0;
}
