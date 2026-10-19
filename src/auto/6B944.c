#include "common.h"
#include "m2c.h"

typedef struct OSTimer {
    struct OSTimer *next;
    struct OSTimer *prev;
    u64 interval;
    u64 remaining;
    OSMesgQueue *mq;
    OSMesg msg;
} OSTimer;
extern OSTimer *__osTimerList;
extern u32 __osTimerCounter;
u32 __osDisableInt(void);
void __osRestoreInt(u32);
void __osSetCompare(u32);
u32 osGetCount(void);
void __osSetTimerIntr(u64);
s32 osSendMesg(OSMesgQueue *, OSMesg, s32);
u64 __osInsertTimer(OSTimer *);

void __osTimerInterrupt(void) {
    OSTimer *t;
    u32 count;
    u32 elapsed_cycles;

    if (__osTimerList->next == __osTimerList) {
        return;
    }
    for (;;) {
        t = __osTimerList->next;
        if (t == __osTimerList) {
            __osSetCompare(0);
            __osTimerCounter = 0;
            break;
        }
        count = osGetCount();
        elapsed_cycles = count - __osTimerCounter;
        __osTimerCounter = count;
        if (elapsed_cycles < t->remaining) {
            t->remaining -= elapsed_cycles;
            __osSetTimerIntr(t->remaining);
            return;
        } else {
            t->prev->next = t->next;
            t->next->prev = t->prev;
            t->next = NULL;
            t->prev = NULL;
            if (t->mq != NULL) {
                osSendMesg(t->mq, t->msg, OS_MESG_NOBLOCK);
            }
            if (t->interval != 0) {
                t->remaining = t->interval;
                __osInsertTimer(t);
            }
        }
    }
}
