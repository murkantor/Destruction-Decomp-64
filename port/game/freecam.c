/*
 * The free camera: each player's right stick swings their view around the car.
 * The stick's direction is the view's: up is the game's own view, right looks
 * from the right, down looks back at the car from in front, and a part push
 * turns part of the way; let go and the view is home at once. It moves the eye
 * only where the view is built (Camera_BuildProjection), on a copy: the game's
 * camera, which the sound and everything else go by, is left as the game set
 * it. It works only while the players drive (the countdown and the race, the
 * modes in which the game reads their controls: func_80024690), not in the
 * course intro's fly-by or the title screen's demo races. A stick within
 * SNAP of straight up, right, down or left is taken as exactly that, so the
 * straight views hold steady. [input] free_camera turns it off.
 */
#include "common.h"

typedef struct { f32 x, y, z; } Vec3f;

extern u8 *gGame; /* its mode at 0x10FC */

int Port_InputRightStick(int player, f32 *x, f32 *y);
f32 sinf(f32);
f32 cosf(f32);
f32 sqrtf(f32);
f32 atan2f(f32, f32);

#define SNAP (10.0f * 3.14159265f / 180.0f) /* radians either side of each quarter turn */
#define QUARTER (3.14159265f / 2.0f)

/* The eye to draw from, for the view of player p (0-3) */
void Port_FreeCamEye(int p, const Vec3f *eye, const Vec3f *at, Vec3f *out) {
    f32 sx, sy, m, yaw, dx, dz, c, s;
    s16 mode;

    *out = *eye;
    mode = *(s16 *)(gGame + 0x10FC);
    if (mode != 0x1B && mode != 0x1C) {
        return; /* the players are not driving: the game's camera as it is */
    }
    if (!Port_InputRightStick(p, &sx, &sy) || (sx == 0.0f && sy == 0.0f)) {
        return; /* neutral: the game's own view */
    }
    m = sqrtf(sx * sx + sy * sy);
    if (m > 1.0f) {
        m = 1.0f;
    }
    yaw = atan2f(sx, sy); /* up 0, right a quarter turn, down a half */
    {
        f32 q = (yaw >= 0.0f ? (s32)(yaw / QUARTER + 0.5f) : -(s32)(-yaw / QUARTER + 0.5f)) * QUARTER;
        f32 d = yaw - q;
        if (d > -SNAP && d < SNAP) {
            yaw = q;
        }
    }
    yaw *= m;
    c = cosf(yaw);
    s = sinf(yaw);
    dx = eye->x - at->x;
    dz = eye->z - at->z;
    out->x = at->x + dx * c - dz * s;
    out->z = at->z + dx * s + dz * c;
}
