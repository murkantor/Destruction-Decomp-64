/*
 * libultra device functions: VI, PI, AI, SP, DP, SI (controllers and rumble),
 * caches and the low-memory OS globals. The Controller Pak calls are in pak.c,
 * threads and messages in os_thread.c.
 */
#include <SDL3/SDL.h>
#include <string.h>

#include "port.h"
#include "port_shadow.h"

/* Low-memory globals IPL3 and osInitialize set on the N64 */
s32 osTvType = OS_TV_NTSC;
s32 osRomType = 0;
u32 osRomBase = 0xB0000000;
s32 osResetType = 0;
s32 osCicId = 6102;
s32 osVersion = 0;
u32 osMemSize = 0x400000;
u8 osAppNMIBuffer[64];

void osInitialize(void) {
}

/* Code-segment symbols the game refers to but never needs on PC */
u8 _mainSegmentStart[1];

void entrypoint(void) {
    Port_Fatal("The game jumped to its reset entry point");
}

/* ---- Caches: no-ops, and switch points for the thread layer ---- */

void osInvalDCache(void *vaddr, s32 nbytes) {
    Port_SafePoint();
}

void osInvalICache(void *vaddr, s32 nbytes) {
    Port_SafePoint();
}

void osWritebackDCache(void *vaddr, s32 nbytes) {
    Port_SafePoint();
}

void osWritebackDCacheAll(void) {
    Port_SafePoint();
}

/* Display lists hold host pointers on PC; port/fast3d resolves segments itself. */
u32 osVirtualToPhysical(void *addr) {
    return (u32)addr;
}

void *osPhysicalToVirtual(u32 addr) {
    return (void *)addr;
}

/* ---- PI: the cart is the ROM image in memory ---- */

static OSPiHandle sCartHandle;
static OSMesgQueue sPiCmdQueue;
static OSMesg sPiCmdBuf[1];

static void piCopy(s32 direction, u32 devAddr, void *dramAddr, u32 size) {
    u32 off = devAddr & 0x1FFFFFFF;

    if (off >= 0x10000000) {
        off -= 0x10000000;
    }
    if (direction != OS_READ) {
        return;
    }
    if (off + size > gPortRomSize) {
        Port_Fatal("PI DMA outside the ROM: 0x%08X + 0x%X", devAddr, size);
    }
    if (((u32)dramAddr & 7) != 0 || (off & 1) != 0) {
        Port_Log("pi: unaligned DMA ROM %X -> %p, %u bytes, from %p", off, dramAddr, (unsigned)size,
                 __builtin_return_address(1));
    }
    memcpy(dramAddr, gPortRom + off, size);
    /* Converted to host order where the game first reads it (shadow.c) */
    if ((u32)dramAddr - (u32)gRam >= PORT_RAM_SIZE) {
        /* Outside the RAM image (a stack buffer): nothing would convert it */
        Port_Fatal("DMA into memory outside the RAM image: %p (%u bytes from ROM 0x%X), from %p", dramAddr,
                   (unsigned)size, off, __builtin_return_address(1));
    }
    Port_MarkBE(dramAddr, size);
}

void osCreatePiManager(OSPri pri, OSMesgQueue *cmdQ, OSMesg *cmdBuf, s32 cmdMsgCnt) {
    osCreateMesgQueue(&sPiCmdQueue, sPiCmdBuf, 1);
}

OSMesgQueue *osPiGetCmdQueue(void) {
    return &sPiCmdQueue;
}

s32 osPiStartDma(OSIoMesg *mb, s32 priority, s32 direction, u32 devAddr, void *vAddr, u32 nbytes,
                 OSMesgQueue *mq) {
    piCopy(direction, devAddr, vAddr, nbytes);
    mb->hdr.type = direction == OS_READ ? OS_MESG_TYPE_DMAREAD : OS_MESG_TYPE_DMAWRITE;
    mb->hdr.pri = priority;
    mb->hdr.retQueue = mq;
    mb->dramAddr = vAddr;
    mb->devAddr = devAddr;
    mb->size = nbytes;
    mb->piHandle = NULL;
    if (mq != NULL) {
        Port_PostMesg(mq, (OSMesg)mb);
    }
    return 0;
}

s32 osEPiStartDma(OSPiHandle *handle, OSIoMesg *mb, s32 direction) {
    piCopy(direction, handle->baseAddress | mb->devAddr, mb->dramAddr, mb->size);
    if (mb->hdr.retQueue != NULL) {
        Port_PostMesg(mb->hdr.retQueue, (OSMesg)mb);
    }
    return 0;
}

s32 osPiRawStartDma(s32 direction, u32 devAddr, void *dramAddr, u32 size) {
    piCopy(direction, devAddr, dramAddr, size);
    return 0;
}

s32 osEPiRawStartDma(OSPiHandle *handle, s32 direction, u32 devAddr, void *dramAddr, u32 size) {
    piCopy(direction, handle->baseAddress | devAddr, dramAddr, size);
    return 0;
}

s32 osPiRawReadIo(u32 devAddr, u32 *data) {
    *data = Port_RomRead32(devAddr & 0x0FFFFFFF);
    return 0;
}

s32 osEPiRawReadIo(OSPiHandle *handle, u32 devAddr, u32 *data) {
    *data = Port_RomRead32((handle->baseAddress | devAddr) & 0x0FFFFFFF);
    return 0;
}

s32 osPiReadIo(u32 devAddr, u32 *data) {
    return osPiRawReadIo(devAddr, data);
}

u32 osPiGetStatus(void) {
    return 0;
}

OSPiHandle *osCartRomInit(void) {
    sCartHandle.type = 0;
    sCartHandle.baseAddress = 0xB0000000;
    sCartHandle.domain = 0;
    return &sCartHandle;
}

OSPiHandle *osDriveRomInit(void) {
    return osCartRomInit();
}

/* ---- VI ---- */

static OSMesgQueue *sViEventQueue;
static OSMesg sViEventMsg;
static u32 sViRetraceCount = 1;
static u32 sViRetraces;
static void *sViCurrentFb;
static void *sViNextFb;

void osCreateViManager(OSPri pri) {
}

void osViSetEvent(OSMesgQueue *mq, OSMesg msg, u32 retraceCount) {
    sViEventQueue = mq;
    sViEventMsg = msg;
    sViRetraceCount = retraceCount != 0 ? retraceCount : 1;
}

void osViSetMode(void *mode) {
}

void osViSetSpecialFeatures(u32 func) {
}

void osViSetXScale(f32 value) {
}

void osViSetYScale(f32 value) {
}

void osViBlack(u8 active) {
    Port_VideoSetBlack(active);
}

void osViSwapBuffer(void *frameBufPtr) {
    sViNextFb = frameBufPtr;
}

void *osViGetCurrentFramebuffer(void) {
    return sViCurrentFb;
}

void *osViGetNextFramebuffer(void) {
    return sViNextFb;
}

/* Host main thread, once per retrace (60 Hz). */
void Port_ViRetrace(void) {
    if (sViNextFb != sViCurrentFb) {
        sViCurrentFb = sViNextFb;
        Port_VideoSwap(sViCurrentFb);
    }
    sViRetraces++;
    if (sViEventQueue != NULL && sViRetraces % sViRetraceCount == 0) {
        Port_PostMesg(sViEventQueue, sViEventMsg);
    }
}

/* ---- AI ---- */

#define VI_NTSC_CLOCK 48681812 /* osViClock */

static u32 sAiFrequency = 22050;

/*
 * As libultra's: the DAC divides the VI clock, so the rate it gives (returned,
 * and what the game sizes its frames and pitches by) is not quite the one
 * asked for. The output plays at that rate.
 */
s32 osAiSetFrequency(u32 frequency) {
    f32 f = VI_NTSC_CLOCK / (f32)frequency + .5f;
    u32 dacRate = f;

    if (dacRate < 132) {
        return -1;
    }
    sAiFrequency = VI_NTSC_CLOCK / (s32)dacRate;
    Port_AudioSetFrequency(sAiFrequency);
    return sAiFrequency;
}

s32 osAiSetNextBuffer(void *bufPtr, u32 size) {
    Port_AudioQueue(bufPtr, size);
    return 0;
}

/* Bytes still to play in the current AI DMA; the audio manager sizes frames from it (port/src/audio.c). */
u32 osAiGetLength(void) {
    return Port_AudioQueuedBytes();
}

u32 osAiGetStatus(void) {
    return 0;
}

/* ---- SP and DP: tasks go to port/src/rcp.c ---- */

void osSpTaskLoad(OSTask *task) {
}

void osSpTaskStartGo(OSTask *task) {
    Port_RcpSubmit(task);
}

void osSpTaskYield(void) {
}

u32 osSpTaskYielded(OSTask *task) {
    return 0;
}

s32 __osSpSetPc(u32 pc) {
    return 0;
}

u32 osDpGetStatus(void) {
    return 0;
}

void osDpSetStatus(u32 data) {
}

void osDpGetCounters(u32 *counters) {
    counters[0] = counters[1] = counters[2] = counters[3] = 0;
}

s32 osDpSetNextBuffer(void *bufPtr, u64 size) {
    return 0;
}

/* ---- SI: controllers, read from port/src/input.c ---- */

s32 osContInit(OSMesgQueue *mq, u8 *bitpattern, OSContStatus *data) {
    u8 bits = 0;
    int i;

    for (i = 0; i < MAXCONTROLLERS; i++) {
        if (Port_InputConnected(i)) {
            bits |= 1 << i;
            data[i].type = CONT_TYPE_NORMAL;
            data[i].status = Port_PakPresent(i) ? CONT_CARD_ON : 0;
            data[i].errnum = 0;
        } else {
            data[i].type = 0;
            data[i].status = 0;
            data[i].errnum = CONT_NO_RESPONSE_ERROR;
        }
    }
    *bitpattern = bits;
    return 0;
}

s32 osContReset(OSMesgQueue *mq, OSContStatus *data) {
    u8 bits;

    return osContInit(mq, &bits, data);
}

s32 osContStartQuery(OSMesgQueue *mq) {
    Port_PostEvent(OS_EVENT_SI);
    return 0;
}

void osContGetQuery(OSContStatus *data) {
    u8 bits;

    osContInit(NULL, &bits, data);
}

s32 osContStartReadData(OSMesgQueue *mq) {
    Port_InputUpdate();
    Port_PostEvent(OS_EVENT_SI);
    return 0;
}

void osContGetReadData(OSContPad *data) {
    int i;

    for (i = 0; i < MAXCONTROLLERS; i++) {
        if (Port_InputConnected(i)) {
            Port_InputGetPad(i, &data[i]);
            data[i].errnum = 0;
        } else {
            memset(&data[i], 0, sizeof(data[i]));
            data[i].errnum = CONT_NO_RESPONSE_ERROR;
        }
    }
}

/* Rumble Pak: every connected controller has one, alongside its Controller Pak. */
s32 osMotorInit(OSMesgQueue *mq, OSPfs *pfs, int channel) {
    pfs->queue = mq;
    pfs->channel = channel;
    pfs->status = PFS_MOTOR_INITIALIZED;
    return Port_InputConnected(channel) ? 0 : PFS_ERR_NOPACK;
}

s32 __osMotorAccess(OSPfs *pfs, s32 flag) {
    Port_InputRumble(pfs->channel, flag);
    return 0;
}

/* ---- libc pieces the SDK supplied ---- */

#ifdef _WIN32 /* glibc has these */
void bzero(void *p, size_t n) {
    memset(p, 0, n);
}

void bcopy(const void *src, void *dst, size_t n) {
    memmove(dst, src, n);
}

int bcmp(const void *a, const void *b, size_t n) {
    return memcmp(a, b, n);
}
#endif
