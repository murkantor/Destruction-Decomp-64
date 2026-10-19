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

u64 __osInsertTimer(OSTimer *t) {
    OSTimer *timep;
    u64 tim;
    u32 savedMask = __osDisableInt();

    for (timep = __osTimerList->next, tim = t->remaining;
         timep != __osTimerList && tim > timep->remaining;
         tim -= timep->remaining, timep = timep->next) {
    }
    t->remaining = tim;
    if (timep != __osTimerList) {
        timep->remaining -= tim;
    }
    t->next = timep;
    t->prev = timep->prev;
    timep->prev->next = t;
    timep->prev = t;
    __osRestoreInt(savedMask);
    return tim;
}
