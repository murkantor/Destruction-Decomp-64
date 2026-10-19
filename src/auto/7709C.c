#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0x10]; u32 bmCtlShadow; } TranxInfo;
typedef struct { u8 pad[0x14]; TranxInfo transferInfo; } DiskHandle;
extern DiskHandle *D_80124460;
extern u32 D_800B56A0;
void __osLeoResume(void);

#define IO_READ(a) (*(volatile u32 *)(a))
#define IO_WRITE(a, v) (*(volatile u32 *)(a) = (v))

void __osLeoAbnormalResume(void) {
    TranxInfo *info = &D_80124460->transferInfo;
    u32 pi_stat;

    pi_stat = IO_READ(0xA4600010);
    while (pi_stat & 3) {
        pi_stat = IO_READ(0xA4600010);
    }
    IO_WRITE(0xA5000510, info->bmCtlShadow | 0x10000000);
    pi_stat = IO_READ(0xA4600010);
    while (pi_stat & 3) {
        pi_stat = IO_READ(0xA4600010);
    }
    IO_WRITE(0xA5000510, info->bmCtlShadow);
    __osLeoResume();
    IO_WRITE(0xA4600010, 2);
    D_800B56A0 |= 0x100401;
}

extern OSMesgQueue *D_801310B8;
extern OSMesg D_801310BC;
extern OSThread *__osRunQueue;
OSThread *__osPopThread(OSThread **);
void __osEnqueueThread(OSThread **, OSThread *);

void __osLeoResume(void) {
    OSMesgQueue *mq = D_801310B8;

    if (mq != NULL && mq->validCount < mq->msgCount) {
        mq->msg[(mq->first + mq->validCount) % mq->msgCount] = D_801310BC;
        mq->validCount++;
        if (mq->mtqueue->next != NULL) {
            __osEnqueueThread(&__osRunQueue, __osPopThread(&mq->mtqueue));
        }
    }
}
