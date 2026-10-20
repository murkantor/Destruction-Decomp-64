#include "common.h"
#include "m2c.h"

typedef struct ALLink_s { struct ALLink_s *next; struct ALLink_s *prev; } ALLink;
typedef struct { ALLink node; s32 x8; u32 xC; } Item682E0;
typedef struct { Item682E0 *head; ALLink *x4; } List682E0;
extern u32 D_800B4E44;
extern u32 D_800B4E40;
extern OSMesgQueue D_800CEAF0;
extern List682E0 D_800CEAD4[];
s32 osRecvMesg(OSMesgQueue *, OSMesg *, s32);
void alUnlink(ALLink *);
void alLink(ALLink *, ALLink *);

void __clearAudioDMA(void) {
    u32 i;
    OSMesg msg;
    Item682E0 *item;
    Item682E0 *next;
    List682E0 *list;

    for (i = 0; i < D_800B4E44; i++) {
        osRecvMesg(&D_800CEAF0, &msg, 0);
    }

    item = D_800CEAD4[0].head;
    while (item != NULL) {
        next = (Item682E0 *)item->node.next;
        if (item->xC + 1 < D_800B4E40) {
            if (D_800CEAD4[0].head == item) {
                D_800CEAD4[0].head = next;
            }
            alUnlink(&item->node);
            if (D_800CEAD4[0].x4 != NULL) {
                alLink(&item->node, D_800CEAD4[0].x4);
            } else {
                D_800CEAD4[0].x4 = &item->node;
                item->node.next = NULL;
                item->node.prev = NULL;
            }
        }
        item = next;
    }
    D_800B4E44 = 0;
    D_800B4E40++;
}
