#ifndef ULTRA64_H
#define ULTRA64_H

/* Minimal libultra declarations used by the decompiled code. */

typedef signed char s8;
typedef unsigned char u8;
typedef signed short s16;
typedef unsigned short u16;
typedef signed long s32;
typedef unsigned long u32;
typedef volatile unsigned long vu32;
typedef signed long long s64;
typedef unsigned long long u64;
typedef float f32;
typedef double f64;

#define NULL 0
#define TRUE 1
#define FALSE 0

typedef s32 OSPri;
typedef s32 OSId;

typedef struct OSThread_s {
    /* 0x000 */ u8 pad[0x1B0];
} OSThread;

#define OS_READ 0
#define OS_WRITE 1

#define OS_PRIORITY_IDLE 0

#define OS_TV_PAL 0
#define OS_TV_NTSC 1
#define OS_TV_MPAL 2

#define PI_STATUS_DMA_BUSY (1 << 0)
#define PI_STATUS_IO_BUSY (1 << 1)
#define PI_STATUS_ERROR (1 << 2)

extern s32 osTvType;

void osInitialize(void);
void osCreateThread(OSThread *t, OSId id, void (*entry)(void *), void *arg, void *sp, OSPri pri);
void osStartThread(OSThread *t);
void osSetThreadPri(OSThread *t, OSPri pri);
u32 osPiGetStatus(void);
s32 osPiRawStartDma(s32 direction, u32 devAddr, void *dramAddr, u32 size);

#endif
