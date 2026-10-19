#include "common.h"
#include "m2c.h"

typedef struct ALLink_s {
    struct ALLink_s *next;
    struct ALLink_s *prev;
} ALLink;
typedef struct {
    u8 pad[0x16];
    s16 priority;
} ALVoice;
typedef struct {
    ALLink node;
    ALVoice *vvoice;
    u8 padC[0xCC];
    s32 offset;
} PVoice;
typedef struct {
    void *head;
    ALLink pFreeList;
    ALLink pAllocList;
    ALLink pLameList;
} ALSynthLists;
void alUnlink(ALLink *);
void alLink(ALLink *, ALLink *);

s32 _allocatePVoice(ALSynthLists *drvr, PVoice **pvoice, s16 priority) {
    ALLink *dl;
    PVoice *pv;
    s32 stolen = 0;

    if ((dl = drvr->pLameList.next) != 0) {
        *pvoice = (PVoice *)dl;
        alUnlink(dl);
        alLink(dl, &drvr->pAllocList);
    } else if ((dl = drvr->pFreeList.next) != 0) {
        *pvoice = (PVoice *)dl;
        alUnlink(dl);
        alLink(dl, &drvr->pAllocList);
    } else {
        for (dl = drvr->pAllocList.next; dl != 0; dl = dl->next) {
            pv = (PVoice *)dl;
            if ((pv->vvoice->priority <= priority) && (pv->offset == 0)) {
                *pvoice = pv;
                priority = pv->vvoice->priority;
                stolen = 1;
            }
        }
    }
    return stolen;
}
