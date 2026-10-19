#include "common.h"
#include "m2c.h"

typedef struct { void *next; void *msgQ; } OSScClient;
void osCreateMesgQueue(OSMesgQueue *, OSMesg *, s32);
void osScAddClient(void *, OSScClient *, OSMesgQueue *);
void osScRemoveClient(void *, OSScClient *);
s32 osRecvMesg(OSMesgQueue *, OSMesg *, s32);
extern u8 D_80130250[];

void Sched_WaitRetrace(void) {
    OSScClient client;
    OSMesgQueue mq;
    OSMesg msgs[2];

    osCreateMesgQueue(&mq, msgs, 2);
    osScAddClient(D_80130250, &client, &mq);
    osRecvMesg(&mq, NULL, OS_MESG_BLOCK);
    osScRemoveClient(D_80130250, &client);
}
