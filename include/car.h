#ifndef CAR_H
#define CAR_H

#include "ultra64.h"

/*
 * Car fields named from the prototype's debug menu (docs/prototype.md).
 * Offsets were read from the prototype; the damage zones are also used by retail code.
 */

typedef struct AiDriver {
    /* 0x00 */ u8 unk0[0x20];
    /* 0x20 */ f32 tendency;        /* "T Tendancy" */
    /* 0x24 */ u8 unk24[0x10];
    /* 0x34 */ f32 newTarget;       /* "A New Target" */
    /* 0x38 */ f32 hitTarget;       /* "A Hit Target" */
    /* 0x3C */ s32 defence;         /* "A Defence", 0 to 99 */
    /* 0x40 */ s32 boolClose;       /* "A Bool Close" */
    /* 0x44 */ s32 boolDeath;       /* "A Bool Death" */
    /* 0x48 */ s32 boolLead;        /* "A Bool Lead" */
    /* 0x4C */ s32 targetBias;      /* "A Base Tenac", an AiTargetBias */
    /* 0x50 */ s32 baseTenacity;    /* second "A Base Tenac", 0 to 9 */
    /* 0x54 */ u8 unk54[0x4];
    /* 0x58 */ s32 baseSearch;      /* "A Base Searc", 0 to 9 */
} AiDriver;

/* One of eight damage zones; "Off" and "Def" in the menu, 0 to 99. */
typedef struct DamageZone {
    /* 0x00 */ s32 defence;
    /* 0x04 */ s32 offence;
    /* 0x08 */ u8 unk8[0xC];
} DamageZone; /* size 0x14 */

typedef enum DamageZoneId {
    /* 0 */ DAMAGE_ZONE_FRONT,
    /* 1 */ DAMAGE_ZONE_RIGHT_FRONT,
    /* 2 */ DAMAGE_ZONE_RIGHT_SIDE,
    /* 3 */ DAMAGE_ZONE_RIGHT_BACK,
    /* 4 */ DAMAGE_ZONE_LEFT_FRONT,
    /* 5 */ DAMAGE_ZONE_LEFT_SIDE,
    /* 6 */ DAMAGE_ZONE_LEFT_BACK,
    /* 7 */ DAMAGE_ZONE_BACK,
    /* 8 */ DAMAGE_ZONE_COUNT
} DamageZoneId;

typedef struct Car {
    /* 0x000 */ u8 unk0[0x10];
    /* 0x010 */ s32 health;           /* "Health", 0 to 200 */
    /* 0x014 */ u8 unk14[0x20];
    /* 0x034 */ s32 model;            /* "Name", 0 to 11 */
    /* 0x038 */ AiDriver *ai;
    /* 0x03C */ u8 unk3C[0x2CC];
    /* 0x308 */ f32 viewingScale;     /* "Viewing Scale" */
    /* 0x30C */ u8 unk30C[0x760];
    /* 0xA6C */ DamageZone damage[DAMAGE_ZONE_COUNT];
    /* 0xB0C */ u8 unkB0C[0xEC];
    /* 0xBF8 */ f32 traction;         /* "traction" */
    /* 0xBFC */ f32 braking;          /* "braking" */
    /* 0xC00 */ f32 engineBraking;    /* "engine braking" */
    /* 0xC04 */ f32 offroadHandling;  /* "offroad handle" */
    /* 0xC08 */ f32 steerResponse;    /* "steer response" */
    /* 0xC0C */ u8 unkC0C[0xC];
    /* 0xC18 */ f32 torqueAt0Rpm;     /* "torque 0 rpm" */
    /* 0xC1C */ f32 maxTorque;        /* "max torque" */
    /* 0xC20 */ u8 unkC20[0xC];
    /* 0xC2C */ f32 gearRatios[6];    /* "gear ratio R", "gear ratio 1" to "5" */
    /* 0xC44 */ f32 finalRatio;       /* "final ratio" */
    /* 0xC48 */ u8 unkC48[0x1C];
    /* 0xC64 */ f32 mass;             /* "mass", clamped to 500 to 2000 */
    /* 0xC68 */ u8 unkC68[0x14];
    /* 0xC7C */ f32 slowSteerAngle;   /* "slow steer angle" */
    /* 0xC80 */ f32 fastSteerAngle;   /* "fast steer angle" */
    /* 0xC84 */ u8 unkC84[0x18];
    /* 0xC9C */ f32 springK;          /* "spring K" */
    /* 0xCA0 */ f32 springB;          /* "spring B" */
    /* 0xCA4 */ f32 equilibrium;      /* "equilibrium" */
    /* 0xCA8 */ f32 minSpringLength;  /* "min spring len" */
    /* 0xCAC */ f32 maxSpringLength;  /* "max spring len" */
    /* 0xCB0 */ f32 springYPos;       /* "spring ypos" */
} Car;

#endif
