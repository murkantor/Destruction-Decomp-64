#ifndef RACE_CARS_H
#define RACE_CARS_H

/* The cars a race can hold: the N64's 12, or on PC ten times that for the drones
   ([game] drones; port/src/field.c). The race record's car list (at 0x74) and
   every table kept per car are this size; on the N64 nothing changes. */
#ifdef TARGET_PC
#define RACE_CARS 120
#else
#define RACE_CARS 12
#endif

#endif
