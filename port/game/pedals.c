/*
 * Analog pedals ([input] analog_pedals): how far a human player's gas and brake
 * are pressed scales their car's engine torque and wheel braking
 * (Wheel_ComputeTireForce). The game's own flags (A, B) still decide whether the
 * car drives or brakes at all, and a full press, or a button, is the N64's
 * force: 1. Computer cars always get 1, and so does a pedal not pressed at all
 * (port/src/input.c), when the game drives a player's car itself.
 */
#include "common.h"

float Port_InputGas(int channel);
float Port_InputBrake(int channel);

/* The car's player controller, or -1 for a computer car (car->x30, car->x3C->x10) */
static s32 carChannel(void *car) {
    u8 *c = car;

    if (*(s32 *)(c + 0x30) != 0) {
        return -1;
    }
    return *(s8 *)(*(u8 **)(c + 0x3C) + 0x10);
}

/* The engine's share: the gas pedal, or in reverse (the game drives backwards
   while B is held) the brake pedal */
f32 Port_CarDrive(void *car, s32 reversing) {
    s32 ch = carChannel(car);

    if (ch < 0) {
        return 1.0f;
    }
    return reversing ? Port_InputBrake(ch) : Port_InputGas(ch);
}

f32 Port_CarBrake(void *car) {
    s32 ch = carChannel(car);

    return ch < 0 ? 1.0f : Port_InputBrake(ch);
}
