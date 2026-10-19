#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef struct {
    u8 pad[0xC];
    u32 xC;
    u8 pad10[0x44 - 0x10];
    u8 x44;
    u8 x45;
} Obj;
typedef struct {
    u8 pad[0xC];
    s32 xC;
    u16 x10;
    u8 pad12[2];
    s32 x14;
    Vec3f x18;
    Vec3f x24;
    f32 x30;
    f32 x34;
    u8 pad38[4];
    s32 x3C;
    s32 x40;
    s32 x44;
    s32 x48;
} Snd;
typedef struct {
    u8 pad[0xC];
    Obj *objs[61];
    s32 x100;
    u8 pad104[0x4E4 - 0x104];
    Snd *snds[9];
    s32 x508;
    u8 pad50C[0x518 - 0x50C];
    f32 x518;
    f32 x51C;
} World;
typedef struct {
    u8 pad[0x104];
    Snd *snds[4][60];
    u8 pad4C4[0x10];
    s32 x4D4[4];
    u8 pad4E4[0x524 - 0x4E4];
    u8 x524;
} SndMgr;
typedef struct { s32 x0; s32 x4; } PanState;
typedef struct {
    Vec3f *x0;
    Vec3f *x4;
    Vec3f *x8;
    s32 xC;
    PanState *x10;
} View;
typedef struct { u8 pad[0xC]; s16 xC; u8 padE[0x104C - 0xE]; s16 x104C; } Game;

extern Game *gGame;
extern s8 D_800D31CC;
extern View D_80123B58[];
extern SndMgr *D_8011EEF8;
extern u8 D_8010AAA8[];
extern f32 D_8007EEC4;
extern f32 D_8007EEC8;
extern void World_FlagAllObjects(World *);
extern void func_80009638(void);
extern void DrawList_Update(World *, s32, s32 *, Obj *);
extern u8 func_8005CF28(Vec3f *, Obj *);
extern void Sound_FadeOut(s32, s32);
extern void World_RemoveObject(World *, Snd *, s32, s32);
extern s32 Sound_CountPlaying(s32);
extern s32 Sound_AllocVoice(s32, s32, s32, s32, s32);
extern s32 func_8006621C(void *, s32, s32, s32, s32, s32);
extern void func_80009200(World *);
extern f32 func_8005D208(Vec3f *, Vec3f *);
extern s32 Vec3f_HeadingAngleXZ(Vec3f *, Vec3f *, Vec3f *);
extern f32 func_8005D114(Vec3f *, Vec3f *, Vec3f *, Vec3f *);
extern void func_8005D4AC(Snd *, s32 *);
extern void Sound_SetPitch(s32, f32);
extern s32 Sound_SetVolumeById(s32, s32);
extern s32 Sound_CountVoices(s32);
extern s32 Sound_SetVolume(s32, s32);
extern s32 Sound_SetFieldA8(s32, s32);
extern s32 Sound_SetPanById(s32, s32);
extern f32 sqrtf(f32);
extern f64 fabs(f64);

#define CLAMP256(v) if ((v) > 256) (v) = 256; else if ((v) < 0) (v) = 0

void World_UpdateSounds(World *w) {
    s32 dl[5] = { 0, 0, 0, 0, 0 };
    s32 dl0 = 0;
    s32 vol;
    s16 i;
    s16 j;
    s32 cnt;
    s32 a;
    s32 a2;
    s32 v;
    s32 pan;
    f32 d2;
    f32 f;
    f32 pitch;
    Obj *o;
    Snd *s;
    Vec3f *cam;
    PanState *ps;

    if (gGame->x104C <= 0) {
        return;
    }
    World_FlagAllObjects(w);
    func_80009638();
    for (i = 0; i < w->x100; i++) {
        o = w->objs[i];
        if (o != NULL && o->x44 && o->x45 != 1) {
            if (o->xC < 2) {
                DrawList_Update(w, 0, &dl0, o);
            } else {
                for (j = 0; j < D_800D31CC; j++) {
                    if (func_8005CF28(D_80123B58[j].x0, o)) {
                        DrawList_Update(w, j, &dl[j], o);
                    }
                }
            }
        }
    }
    for (i = 0; i < w->x508; i++) {
        s = w->snds[i];
        if (s != NULL && s->x40 == 1) {
            Sound_FadeOut(s->x3C, 0);
            World_RemoveObject(w, s, 0, i);
            i--;
        }
    }
    for (j = 0; j < D_800D31CC; j++) {
        for (i = 0; i < D_8011EEF8->x4D4[j]; i++) {
            s = D_8011EEF8->snds[j][i];
            if (s != NULL && s->x40 == 1) {
                Sound_FadeOut(s->x3C, 0);
                World_RemoveObject(w, s, j, i);
                i--;
            }
        }
    }
    cnt = Sound_CountPlaying(3);
    for (i = 0; i < w->x508; i++) {
        s = w->snds[i];
        if (s == NULL || cnt >= 24) {
            continue;
        }
        if (s->x3C != 0) {
            continue;
        }
        a2 = s->x18.x;
        CLAMP256(a2);
        if (s->xC == 0) {
            if (s->x10 == 0 || s->x10 >= 40) {
                s->x10 = 1;
            }
            a = s->x14 * w->x51C;
            CLAMP256(a);
            cnt++;
            s->x3C = Sound_AllocVoice(s->x10, a, a2, 0, 0);
        } else {
            if ((u16)(s->x10 - 101) >= 148) {
                s->x10 = 0x8D;
            }
            a = s->x14 * w->x518;
            CLAMP256(a);
            cnt++;
            s->x3C = func_8006621C(D_8010AAA8, s->x10, a, a2, 0, 0);
        }
    }
    if (gGame->xC != 2) {
        return;
    }
    func_80009200(w);
    for (j = 0; j < D_800D31CC; j++) {
        ps = D_80123B58[j].x10;
        for (i = 0; i < D_8011EEF8->x4D4[j]; i++) {
            s = D_8011EEF8->snds[j][i];
            if (s == NULL) {
                continue;
            }
            cam = D_80123B58[j].x0;
            d2 = func_8005D208(cam, &s->x18);
            if (d2 != 0.0 && s->x30 > 0.1) {
                f = fabs(s->x30 - sqrtf(d2)) / s->x30;
            } else {
                f = 1.0f;
            }
            vol = s->x14 * f;
            if (D_8011EEF8->x524) {
                pan = Vec3f_HeadingAngleXZ(cam, D_80123B58[j].x4, &s->x18);
            } else {
                pan = 0x80;
            }
            pitch = s->x34 + func_8005D114(cam, D_80123B58[j].x8, &s->x18, &s->x24);
            if (pitch > D_8007EEC4) {
                pitch = D_8007EEC4;
            } else if (pitch < D_8007EEC8) {
                pitch = D_8007EEC8;
            }
            if (s->x44 == D_80123B58[j].xC) {
                func_8005D4AC(s, &vol);
            }
            v = vol * w->x51C;
            CLAMP256(v);
            if (s->x3C == 0 && cnt < 23) {
                if (s->x10 == 0 || s->x10 >= 40) {
                    s->x10 = 1;
                }
                s->x3C = Sound_AllocVoice(s->x10, v, pan, 0, 0);
                Sound_SetPitch(s->x3C, pitch);
                cnt++;
                if (s->x48 != -1) {
                    Sound_SetVolumeById(s->x3C, s->x48);
                }
                if (ps->x0 != 0) {
                    Sound_SetPanById(s->x3C, ps->x0);
                }
            } else if (Sound_CountVoices(s->x3C)) {
                Sound_SetVolume(s->x3C, v);
                Sound_SetFieldA8(s->x3C, pan);
                Sound_SetPitch(s->x3C, pitch);
                if (s->x48 != -1) {
                    Sound_SetVolumeById(s->x3C, s->x48);
                }
                if (ps->x0 != ps->x4) {
                    Sound_SetPanById(s->x3C, ps->x0);
                }
            }
        }
        ps->x4 = ps->x0;
    }
}
