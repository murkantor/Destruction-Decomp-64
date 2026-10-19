#ifndef CAR_H
#define CAR_H

#include "ultra64.h"

/*
 * Car fields named from the prototype's debug menu (docs/prototype.md) and from
 * Car_Create and RigidBody_Init in retail.
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

typedef struct Vec3f {
    /* 0x0 */ f32 x;
    /* 0x4 */ f32 y;
    /* 0x8 */ f32 z;
} Vec3f;

/*
 * Per-model handling data, 0x2B0 bytes. Car_Create copies it into Car.stats and
 * RigidBody_Init reads the block starting at mass.
 */
typedef struct CarStats {
    /* 0x000 */ u8 unk0[0xC];
    /* 0x00C */ f32 traction;         /* "traction" */
    /* 0x010 */ f32 braking;          /* "braking" */
    /* 0x014 */ f32 engineBraking;    /* "engine braking" */
    /* 0x018 */ f32 offroadHandling;  /* "offroad handle" */
    /* 0x01C */ f32 steerResponse;    /* "steer response" */
    /* 0x020 */ u8 unk20[0xC];
    /* 0x02C */ f32 torqueAt0Rpm;     /* "torque 0 rpm" */
    /* 0x030 */ f32 maxTorque;        /* "max torque" */
    /* 0x034 */ u8 unk34[0xC];
    /* 0x040 */ f32 gearRatios[6];    /* "gear ratio R", "gear ratio 1" to "5" */
    /* 0x058 */ f32 finalRatio;       /* "final ratio" */
    /* 0x05C */ u8 unk5C[0x1C];
    /* 0x078 */ f32 mass;             /* "mass", clamped to 500 to 2000 */
    /* 0x07C */ u8 unk7C[0x4];
    /* 0x080 */ f32 viewingScale;     /* "Viewing Scale" */
    /* 0x084 */ Vec3f halfExtents;
    /* 0x090 */ f32 slowSteerAngle;   /* "slow steer angle" */
    /* 0x094 */ f32 fastSteerAngle;   /* "fast steer angle" */
    /* 0x098 */ u8 unk98[0x18];
    /* 0x0B0 */ f32 springK;          /* "spring K", set from mass by Car_Create */
    /* 0x0B4 */ f32 springB;          /* "spring B", set to 0.25 by Car_Create */
    /* 0x0B8 */ f32 equilibrium;      /* "equilibrium", set to 1.2 by Car_Create */
    /* 0x0BC */ f32 minSpringLength;  /* "min spring len" */
    /* 0x0C0 */ f32 maxSpringLength;  /* "max spring len", set to 1.2 by Car_Create */
    /* 0x0C4 */ f32 springYPos;       /* "spring ypos" */
    /* 0x0C8 */ u8 unkC8[0x1E8];
} CarStats; /* size 0x2B0 */

/* Set up by RigidBody_Init from CarStats.mass onwards. */
typedef struct RigidBody {
    /* 0x000 */ u8 unk0[0xC];
    /* 0x00C */ f32 mass;
    /* 0x010 */ f32 invMass;
    /* 0x014 */ Vec3f inertia;        /* box inertia from mass and halfExtents */
    /* 0x020 */ Vec3f invInertia;
    /* 0x02C */ u8 unk2C[0xF0];
    /* 0x11C */ f32 viewingScale;     /* from CarStats.viewingScale */
    /* 0x120 */ f32 radius;           /* length of halfExtents plus a margin */
    /* 0x124 */ u8 unk124[0xC];
    /* 0x130 */ Vec3f halfExtents;    /* from CarStats.halfExtents */
    /* 0x13C */ u8 unk13C[0x744];
} RigidBody; /* size 0x880 */

typedef struct Car {
    /* 0x000 */ u8 unk0[0x10];
    /* 0x010 */ s32 health;           /* "Health", 0 to 200 */
    /* 0x014 */ u8 unk14[0x20];
    /* 0x034 */ s32 model;            /* "Name", 0 to 11 */
    /* 0x038 */ AiDriver *ai;
    /* 0x03C */ u8 unk3C[0x1B0];
    /* 0x1EC */ RigidBody body;
    /* 0xA6C */ DamageZone damage[DAMAGE_ZONE_COUNT];
    /* 0xB0C */ u8 unkB0C[0xE0];
    /* 0xBEC */ CarStats stats;
    /* 0xE9C */ u8 unkE9C[0x4];
} Car; /* size 0xEA0, allocated by Car_Create */

#endif
