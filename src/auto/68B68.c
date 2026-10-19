#include "common.h"
#include "m2c.h"

typedef struct { void *next; OSMesgQueue *msgQ; } OSScClient;
typedef struct { s16 type; s16 pad; s32 x4; } Msg;
typedef struct {
    void *x0[0x1C0 / 4];
    OSMesgQueue q;
    OSMesg qbuf[8];
    OSMesgQueue q2;
} Sched;
extern void *D_800CEB08;
extern Sched D_8011F088;
extern u32 D_800B4E40;
extern s32 D_800B4E50;
extern u8 D_8011F2B8[];
void osScAddClient(void *, OSScClient *, OSMesgQueue *);
s32 osRecvMesg(OSMesgQueue *, OSMesg *, s32);
s32 func_800680A8(void *, s32);
void func_80068260(s32);
void func_8006B08C(void *);

void Audio_SchedThread(void) {
    OSScClient client;
    Msg *msg;
    s32 done = 0;

    osScAddClient(D_800CEB08, &client, &D_8011F088.q);
    do {
        osRecvMesg(&D_8011F088.q, (OSMesg *)&msg, 1);
        switch (msg->type) {
            case 1:
                if (func_800680A8(D_8011F088.x0[D_800B4E40 % 3], D_800B4E50)) {
                    osRecvMesg(&D_8011F088.q2, (OSMesg *)&msg, 1);
                    func_80068260(msg->x4);
                    D_800B4E50 = msg->x4;
                }
                break;
            case 4:
                break;
            case 10:
                done = 1;
                break;
        }
    } while (!done);
    func_8006B08C(D_8011F2B8);
}
