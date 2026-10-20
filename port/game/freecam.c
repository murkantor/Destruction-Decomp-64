/*
 * The free camera: each player's right stick swings their view around the car.
 * The stick's direction is the view's: up is the game's own view, right looks
 * from the right, down looks back at the car from in front, and a part push
 * turns part of the way; let go and the view is home at once. It moves the eye
 * only where the view is built (Camera_BuildProjection), on a copy: the game's
 * camera, which the sound and everything else go by, is left as the game set
 * it. [input] free_camera turns it off.
 */
#include "common.h"

typedef struct { f32 x, y, z; } Vec3f;

int Port_InputRightStick(int player, f32 *x, f32 *y);
f32 sinf(f32);
f32 cosf(f32);
f32 sqrtf(f32);
f32 atan2f(f32, f32);

/* The eye to draw from, for the view of player p (0-3) */
void Port_FreeCamEye(int p, const Vec3f *eye, const Vec3f *at, Vec3f *out) {
    f32 sx, sy, m, yaw, dx, dz, c, s;

    *out = *eye;
    if (!Port_InputRightStick(p, &sx, &sy) || (sx == 0.0f && sy == 0.0f)) {
        return; /* neutral: the game's own view */
    }
    m = sqrtf(sx * sx + sy * sy);
    if (m > 1.0f) {
        m = 1.0f;
    }
    yaw = atan2f(sx, sy) * m; /* up 0, right a quarter turn, down a half */
    c = cosf(yaw);
    s = sinf(yaw);
    dx = eye->x - at->x;
    dz = eye->z - at->z;
    out->x = at->x + dx * c - dz * s;
    out->z = at->z + dx * s + dz * c;
}
