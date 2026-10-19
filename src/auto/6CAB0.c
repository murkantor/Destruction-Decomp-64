#include "common.h"
#include "m2c.h"

extern s32 D_800B5698;

s32 osAiSetFrequency(u32 frequency) {
    register u32 dacRate;
    register u8 bitRate;
    register f32 f;

    f = D_800B5698 / (f32)frequency + .5f;
    dacRate = f;
    if (dacRate < 132) {
        return -1;
    }
    bitRate = dacRate / 66;
    if (bitRate > 16) {
        bitRate = 16;
    }
    *(volatile u32 *)0xA4500010 = dacRate - 1;
    *(volatile u32 *)0xA4500014 = bitRate - 1;
    *(volatile u32 *)0xA4500008 = 1;
    return D_800B5698 / (s32)dacRate;
}
