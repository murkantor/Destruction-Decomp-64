/*
 * Temporary stand-ins for video, audio, input, the Controller Pak and events,
 * so the core can boot headless while the real ones are written.
 */
#include <SDL3/SDL.h>
#include <string.h>

#include "port.h"

static int sGfxTasks, sAudioTasks;

void Port_VideoInit(void) {
}

void Port_VideoRunTask(OSTask *task) {
    if (++sGfxTasks % 60 == 1) {
        Port_Log("gfx task %d", sGfxTasks);
    }
}

void Port_VideoSwap(void *framebuffer) {
}

void Port_VideoSetBlack(int black) {
}

void Port_VideoPresentIdle(void) {
}

void Port_AudioInit(void) {
}

void Port_AudioSetFrequency(u32 freq) {
    Port_Log("audio frequency %u", (unsigned)freq);
}

void Port_AudioQueue(const void *buf, u32 bytes) {
}

u32 Port_AudioQueuedBytes(void) {
    return 0;
}

void Port_AudioRunTask(OSTask *task) {
    if (++sAudioTasks % 60 == 1) {
        Port_Log("audio task %d", sAudioTasks);
    }
}

void Port_InputInit(void) {
}

void Port_InputUpdate(void) {
}

int Port_InputConnected(int channel) {
    return channel == 0;
}

void Port_InputGetPad(int channel, OSContPad *pad) {
    memset(pad, 0, sizeof(*pad));
}

void Port_InputRumble(int channel, int on) {
}

int Port_PakPresent(int channel) {
    return 0;
}

void Port_PumpEvents(void) {
    SDL_Event e;

    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_EVENT_QUIT) {
            Port_RequestQuit();
        }
    }
}

s32 osPfsInitPak(OSMesgQueue *mq, OSPfs *pfs, int channel) {
    return PFS_ERR_NOPACK;
}

s32 osPfsFindFile(OSPfs *pfs, u16 company, u32 game, u8 *name, u8 *ext, s32 *file) {
    return PFS_ERR_NOPACK;
}

s32 osPfsAllocateFile(OSPfs *pfs, u16 company, u32 game, u8 *name, u8 *ext, int length, s32 *file) {
    return PFS_ERR_NOPACK;
}

s32 osPfsDeleteFile(OSPfs *pfs, u16 company, u32 game, u8 *name, u8 *ext) {
    return PFS_ERR_NOPACK;
}

s32 osPfsReadWriteFile(OSPfs *pfs, s32 file, u8 flag, int offset, int size, u8 *data) {
    return PFS_ERR_NOPACK;
}

s32 osPfsFreeBlocks(OSPfs *pfs, s32 *bytes) {
    return PFS_ERR_NOPACK;
}

s32 osPfsFileState(OSPfs *pfs, s32 file, OSPfsState *state) {
    return PFS_ERR_NOPACK;
}

s32 osPfsChecker(OSPfs *pfs) {
    return PFS_ERR_NOPACK;
}

/* Re-read the pak ID after a change of pak */
s32 func_8006FAA0(OSPfs *pfs) {
    return PFS_ERR_INVALID;
}
