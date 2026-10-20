/*
 * Temporary stand-ins for audio, so the core runs while the real one is written.
 */
#include <SDL3/SDL.h>
#include <string.h>

#include "port.h"

static int sAudioTasks;

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
