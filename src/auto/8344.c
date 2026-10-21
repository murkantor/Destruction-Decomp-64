#include "common.h"
#include "m2c.h"

typedef struct {
    f32 x, y, z;
} Vec3f;

typedef struct {
    s16 x0;
    s16 x2;
    s32 x4;
    u8 pad8[4];
    s32 xC;
    s16 x10;
    u8 pad12[2];
    s32 x14;
    Vec3f x18;
    Vec3f x24;
    f32 x30;
    s32 x34;
    s64 x38;
    s32 x40;
    u8 x44;
    u8 x45;
    u8 pad46[2];
    s32 x48;
    s32 x4C;
    u8 x50;
} Req7744;

extern Vec3f D_800B7F20;
extern u8 D_8007EE48[];
extern s32 D_801328FC[];
extern s32 D_801326A8[];
extern s64 D_80132200[];
extern u64 gGameTime;

s16 Obj_NextId(void);
u64 Time_Get(u64 *);
void Pool_PushCopy(void *, Req7744 *);

#ifdef TARGET_PC
static s64 sVoiceFor = -1; /* Sound_PlayVoiceFor's length, ns (-1: the line's own) */
#endif

void Sound_PlayVoice(void *obj, Vec3f pos, s32 id) {
    Req7744 req;
    Vec3f zero = D_800B7F20;
    s64 *t;

    req.x0 = 0x3F4;
    req.x4 = 0;
    req.x2 = Obj_NextId();
    req.xC = 1;
    req.x18 = pos;
    req.x24 = zero;
    req.x34 = 0;
    req.x40 = 0;
    req.x44 = 1;
    req.x45 = 0;
    req.x4C = -1;
    req.x50 = D_8007EE48[req.xC];
    req.x10 = id;
    id -= 100;
    req.x30 = D_801328FC[id];
    req.x14 = D_801326A8[id];
    t = &D_80132200[id];
    req.x38 = (*t != -1) ? *t + Time_Get(&gGameTime) : -1;
#ifdef TARGET_PC
    if (sVoiceFor >= 0) {
        req.x38 = sVoiceFor + Time_Get(&gGameTime); /* stopped there (the voices expire at x38) */
    }
#endif
    req.x48 = 0;
    Pool_PushCopy(obj, &req);
}

#ifdef TARGET_PC
/* A line cut after `ns` ([game] announce_all_wrecks, Announcer_Update) */
void Sound_PlayVoiceFor(void *obj, Vec3f pos, s32 id, u64 ns) {
    sVoiceFor = (s64)ns;
    Sound_PlayVoice(obj, pos, id);
    sVoiceFor = -1;
}
#endif
