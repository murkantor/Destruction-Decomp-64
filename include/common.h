#ifndef COMMON_H
#define COMMON_H

#include "ultra64.h"
#include "include_asm.h"

/* The cars a race can hold: the N64's 12, or on PC twice that for the doubled field
   ([game] double_field; port/game/field.c). The race record's car list (at 0x74) and
   every table kept per car are this size; on the N64 nothing changes. */
#ifdef TARGET_PC
#define RACE_CARS 24
#else
#define RACE_CARS 12
#endif

#endif
