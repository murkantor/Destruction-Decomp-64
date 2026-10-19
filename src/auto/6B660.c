#include "common.h"
#include "m2c.h"

typedef struct {
    void *messageQueue;
    void *message;
} __OSEventState;
extern __OSEventState __osEventStateTab[];
u32 __osDisableInt(void);
void __osRestoreInt(u32);

void osSetEventMesg(s32 event, void *mq, void *msg) {
    register u32 saveMask = __osDisableInt();
    __OSEventState *es = &__osEventStateTab[event];

    es->messageQueue = mq;
    es->message = msg;
    __osRestoreInt(saveMask);
}
