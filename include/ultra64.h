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

typedef struct {
    /* 0x00 */ u64 at, v0, v1, a0, a1, a2, a3;
    /* 0x38 */ u64 t0, t1, t2, t3, t4, t5, t6, t7;
    /* 0x78 */ u64 s0, s1, s2, s3, s4, s5, s6, s7;
    /* 0xB8 */ u64 t8, t9, gp, sp, s8, ra;
    /* 0xE8 */ u64 lo, hi;
    /* 0xF8 */ u32 sr, pc, cause, badvaddr, rcp;
    /* 0x10C */ u32 fpcsr;
    /* 0x110 */ f64 fp[16];
} __OSThreadContext; /* size = 0x190 */

typedef struct OSThread_s {
    /* 0x000 */ struct OSThread_s *next;
    /* 0x004 */ OSPri priority;
    /* 0x008 */ struct OSThread_s **queue;
    /* 0x00C */ struct OSThread_s *tlnext;
    /* 0x010 */ u16 state;
    /* 0x012 */ u16 flags;
    /* 0x014 */ OSId id;
    /* 0x018 */ s32 fp;
    /* 0x01C */ void *thprof;
    /* 0x020 */ __OSThreadContext context;
} OSThread; /* size = 0x1B0 */

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
