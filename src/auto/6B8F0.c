#include "common.h"
#include "m2c.h"

typedef struct OSTimer {
    struct OSTimer *next;
    struct OSTimer *prev;
    u64 interval;
    u64 remaining;
    void *mq;
    void *msg;
} OSTimer;
extern OSTimer *__osTimerList;
extern u64 __osCurrentTime;
extern u32 __osBaseCounter;
extern u32 __osViIntrCount;

void __osTimerServicesInit(void) {
    __osCurrentTime = 0;
    __osBaseCounter = 0;
    __osViIntrCount = 0;
    __osTimerList->remaining = 0;
    __osTimerList->interval = __osTimerList->remaining;
    __osTimerList->prev = __osTimerList;
    __osTimerList->next = __osTimerList->prev;
    __osTimerList->mq = NULL;
    __osTimerList->msg = NULL;
}
