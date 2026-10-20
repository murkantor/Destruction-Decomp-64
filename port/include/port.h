/*
 * Interfaces between the parts of the PC port (port/src). Game code does not
 * include this; it calls the libultra API, which port/src/os_*.c implements.
 */
#ifndef PORT_H
#define PORT_H

#include <stdint.h>

#include "port_os.h"

#ifdef __cplusplus
extern "C" {
#endif

/* main.c */
void Port_Fatal(const char *fmt, ...);
void Port_Log(const char *fmt, ...);
void Port_RequestQuit(void);
extern volatile int gPortQuit;

/* rom.c: the user's cart image, kept big-endian as on the cart */
extern u8 *gPortRom;
extern u32 gPortRomSize;
int Port_RomLoad(void);
u32 Port_RomRead32(u32 offset);

/* ram.c: the N64 RAM image, mapped at its own addresses (0x80000000 up) */
void Port_RamInit(void);
void *Port_N64ToHost(uint32_t addr);
uint32_t Port_HostToN64(const void *p);
void Port_BlockLoaded(void *dst, u32 size);

/* os_thread.c: threads, message queues, events and timers */
void Port_OsInit(void);
void Port_PostEvent(OSEvent event);
void Port_PostMesg(OSMesgQueue *mq, OSMesg msg);
void Port_SafePoint(void);
OSTime Port_GetTime(void);
void Port_ViStart(void);
void Port_ViRetrace(void);

/* rcp.c: graphics tasks, run on the host main thread; audio tasks, run where they are started */
void Port_RcpInit(void);
void Port_RcpSubmit(OSTask *task);
void Port_RcpYield(void);
int Port_RcpYielded(OSTask *task);
void Port_RcpWake(void);
void Port_RcpWait(u64 untilNs);
/* A precise wait, mostly asleep (os_thread.c): one per waiting thread */
typedef struct {
    u64 margin; /* the spin before the deadline, learnt */
    u64 waits, spunNs; /* (testing) */
} PortSleeper;
void Port_SleepUntilNs(u64 until, PortSleeper *sl);
void Port_RcpRunPending(void);

/* audio.c */
void Port_AudioInit(void);
void Port_AudioSetFrequency(u32 freq);
void Port_AudioQueue(const void *buf, u32 bytes);
u32 Port_AudioQueuedBytes(void);
void Port_AudioRunTask(OSTask *task);

/* video.c */
void Port_VideoInit(void);
void Port_VideoRunTask(OSTask *task);
void Port_VideoSwap(void *framebuffer);
void Port_VideoSetBlack(int black);
void Port_VideoPresentInterp(void);
void Port_VideoApplySettings(void);
/* The PC options (options.c) */
void Port_OptionsApplyPending(void);
int Port_OptIsOpen(void);
void Port_InputNoteMenuPad(const OSContPad *pad);
union SDL_Event;
int Port_InputCaptureEvent(const union SDL_Event *e);

/* input.c */
void Port_InputInit(void);
int Port_InputConnected(int channel);
void Port_InputGetPad(int channel, OSContPad *pad);
void Port_InputRumble(int channel, int on);

/* pak.c: Controller Pak images on disk */
int Port_PakPresent(int channel);

/* events.c: SDL events (window, quit, devices) */
void Port_PumpEvents(void);

#ifdef __cplusplus
}
#endif

#endif
