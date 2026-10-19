#include "common.h"
#include "m2c.h"

typedef u64 OSTime;
typedef struct OSTimer_s {
    struct OSTimer_s *next;
    struct OSTimer_s *prev;
    OSTime interval;
    OSTime value;
    OSMesgQueue *mq;
    OSMesg msg;
} OSTimer;

extern OSTimer *__osTimerList;
extern OSTime __osInsertTimer(OSTimer *t);
extern void __osSetTimerIntr(OSTime t);

int osSetTimer(OSTimer *t, OSTime countdown, OSTime interval, OSMesgQueue *mq, OSMesg msg)
{
    OSTime time;
    t->next = NULL;
    t->prev = NULL;
    t->value = countdown;
    t->interval = interval;
    if (countdown == 0) {
        t->value = interval;
    }
    t->mq = mq;
    t->msg = msg;
    time = __osInsertTimer(t);
    if (__osTimerList->next == t) {
        __osSetTimerIntr(time);
    }
    return 0;
}
