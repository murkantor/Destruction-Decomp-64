#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef struct { Vec3f a; Vec3f b; } Seg;
typedef struct { Vec3f a; Vec3f b; s32 x18; } GridSlot;
typedef struct { u8 pad0[4]; s32 type; Vec3f a; Vec3f b; u8 pad20[8]; } TrackObj;
typedef struct {
    u8 pad0[0x28];
    s32 seg;
    u8 pad2C[0x34 - 0x2C];
    s32 id;
    u8 pad38[0x58 - 0x38];
    s32 dir;
    u8 pad5C[0x218 - 0x5C];
    Vec3f pos;
    u8 pad224[0x23C - 0x224];
    Vec3f basis;
    u8 pad248[0x290 - 0x248];
    Vec3f pos2;
    u8 pad29C[0x2B4 - 0x29C];
    Vec3f basis2;
} Car;
typedef struct { u8 pad[0x6C]; s32 x6C; } TrackRes;
typedef struct { u8 pad[0xC]; TrackRes *res; } TrackInfo;
typedef struct {
    u8 pad0[0x10];
    s32 x10;
    u8 pad14[0x30 - 0x14];
    Seg *segs;
    u8 pad34[4];
    s32 nsegs;
    u8 pad3C[0x44 - 0x3C];
    s32 nobjs;
    TrackObj *objs;
    u8 pad4C[0x70 - 0x4C];
    TrackInfo *info;
    Car *cars[12];
    s32 ncars;
} Track;
typedef struct { u8 pad[0x10FC]; s16 x10FC; u8 pad10FE[2]; Track *track; } Game;
typedef struct { s32 x0; s32 x4; u8 pad[8]; } StartDef;

extern Game *gGame[];
extern s32 D_800958A4[];
extern s32 D_801252F4;
extern StartDef *D_80095CA8[];
extern s32 D_80095CB8[];
extern Vec3f D_800D1B08[];
extern void *Res_Get(s32 id);
extern void Mem_Free(void *p);
extern void Basis_Orthonormalize(Vec3f *v);
extern f32 Vec3f_DistXZ(Vec3f a, Vec3f b);
extern s32 Track_FindNearestSegment(Vec3f p);
extern s32 Race_FindCarSlot(s32 id);

void Race_PlaceCarsOnGrid(void)
{
    s32 n2 = 0;
    s32 sign = 0;
    s32 unk = 0;
    Vec3f dir;
    Vec3f mid;
    s32 segs[6];
    Vec3f tbl[4][8] = {
        { { -42.26301956176758f, 7.1458001136779785f, 49.68244934082031f },
          { -51.458518981933594f, 6.916560173034668f, 31.12343978881836f },
          { -22.061519622802734f, 5.519110202789307f, 62.85523986816406f },
          { -32.58441925048828f, 0.20000000298023224f, 45.18003845214844f },
          { 40.5129508972168f, 1.749500036239624f, 53.55918884277344f },
          { 23.51218032836914f, 1.5191099643707275f, 61.72254943847656f },
          { 51.218650817871094f, 1.9165600538253784f, 37.17399978637695f },
          { 32.58441925048828f, 0.20000000298023224f, 45.18003845214844f } },
        { { 39.96255874633789f, 7.1458001136779785f, -56.10580062866211f },
          { 48.57918167114258f, 5.519110202789307f, -49.191978454589844f },
          { 32.15121841430664f, 5.442339897155762f, -49.907859802246094f },
          { 40.129459381103516f, 0.10000000149011612f, -49.27920913696289f },
          { -40.18748092651367f, 8.993619918823242f, -57.46282958984375f },
          { -30.892009735107422f, 7.502359867095947f, -50.854400634765625f },
          { -49.05437088012695f, 6.916560173034668f, -48.91141128540039f },
          { -39.584510803222656f, 0.10000000149011612f, -49.27920913696289f } },
        { { 5.842629909515381f, -13.662969589233398f, -44.61314010620117f },
          { 0.5855000019073486f, -13.762909889221191f, -51.07836151123047f },
          { 12.473099708557129f, -13.661470413208008f, -51.687339782714844f },
          { 6.250720024108887f, -14.769490242004395f, 62.839839935302734f },
          { 6.495639801025391f, -13.237680435180664f, 57.22373962402344f },
          { 11.932640075683594f, -13.430480003356934f, 62.06155014038086f },
          { 0.560699999332428f, -13.637669563293457f, 61.86090850830078f },
          { 63.0536003112793f, -15.069490432739258f, 6.5304999351501465f } },
        { { -25.249740600585938f, 10.644880294799805f, 65.47569274902344f },
          { -20.32992935180664f, 10.644880294799805f, 73.4651107788086f },
          { -30.760669708251953f, 10.644880294799805f, 64.04319763183594f },
          { -28.58068084716797f, 10.644880294799805f, 68.14787292480469f },
          { -26.699609756469727f, 10.644880294799805f, -51.404239654541016f },
          { -32.427879333496094f, 10.644880294799805f, -50.616580963134766f },
          { -25.94141960144043f, 10.020990371704102f, -56.89072036743164f },
          { -29.736669540405273f, 7.844880104064941f, -54.45349884033203f } }

    };
    Track *track;
    GridSlot *data;
    GridSlot *slot;
    Car *car;
    s32 i;
    s32 j;
    s32 idx, other;
    s32 rem;
    s32 nc;
    s32 seg, prev;
    s32 k;
    f32 f;
    f32 len;
    s32 gap;
    s32 q, t;

    track = gGame[0]->track;
    if (track->x10 != 0) {
        data = Res_Get(track->info->res->x6C);
        for (nc = 0; nc < gGame[0]->track->ncars; nc++) {
            car = gGame[0]->track->cars[nc];
            if (D_800958A4[1] == 3 && D_800958A4[2] == 2) {
                if (D_800958A4[0] == 2) {
                    if (nc < 3) {
                        idx = nc;
                        other = 3;
                    } else {
                        idx = nc + 1;
                        other = 7;
                    }
                } else {
                    if (nc < 2) {
                        idx = nc;
                        other = 3;
                    } else {
                        idx = nc + 2;
                        other = 7;
                    }
                }
                data[nc].a = tbl[D_800958A4[4]][idx];
                data[nc].b = tbl[D_800958A4[4]][other];
            }
            car->pos = data[nc].a;
            car->pos2 = data[nc].a;
            dir.x = data[nc].b.x - data[nc].a.x;
            dir.y = data[nc].b.y - data[nc].a.y;
            dir.z = data[nc].b.z - data[nc].a.z;
            car->basis = dir;
            Basis_Orthonormalize(&car->basis);
            car->basis2 = dir;
            Basis_Orthonormalize(&car->basis2);
        }
        Mem_Free(data);
        return;
    }
    {
        s32 counts[3] = { 0, 0, 0 };
        Vec3f p0;
        Vec3f p1;
        s32 start = -1;

        if (D_800958A4[1] == 2) {
            car = gGame[0]->track->cars[0];
            car->dir = 1;
            k = track->nsegs - 1;
            p0.x = (track->segs[k].a.x + track->segs[k].b.x) * 0.5;
            p0.y = (track->segs[k].a.y + track->segs[k].b.y) * 0.5 + 1.0;
            p0.z = (track->segs[k].a.z + track->segs[k].b.z) * 0.5;
            p1.x = (track->segs[0].a.x + track->segs[0].b.x) * 0.5;
            p1.y = (track->segs[0].a.y + track->segs[0].b.y) * 0.5 + 1.0;
            p1.z = (track->segs[0].a.z + track->segs[0].b.z) * 0.5;
            len = Vec3f_DistXZ(p0, p1);
            p0.x = p0.x + (p1.x - p0.x) / len * 0.5;
            p0.y = p0.y + (p1.y - p0.y) / len * 0.5;
            p0.z = p0.z + (p1.z - p0.z) / len * 0.5;
            car->pos = p0;
            car->pos2 = p0;
            dir.x = p1.x - p0.x;
            dir.y = p1.y - p0.y;
            dir.z = p1.z - p0.z;
            car->basis = dir;
            Basis_Orthonormalize(&car->basis);
            car->basis2 = dir;
            Basis_Orthonormalize(&car->basis2);
            k = Race_FindCarSlot(car->id);
            D_800D1B08[k].x = p0.x;
            D_800D1B08[k].y = p0.y;
            D_800D1B08[k].z = p0.z;
            return;
        }
        for (i = 0; i < track->nobjs; i++) {
            if (track->objs[i].type == 3) {
                mid.x = (track->objs[i].a.x + track->objs[i].b.x) * 0.5;
                mid.y = (track->objs[i].a.y + track->objs[i].b.y) * 0.5;
                mid.z = (track->objs[i].a.z + track->objs[i].b.z) * 0.5;
                segs[n2++] = Track_FindNearestSegment(mid);
            }
        }
        if (D_800958A4[1] == 3) {
            n2 = 2;
        }
        rem = gGame[0]->track->ncars % n2;
        for (i = 0; i < n2; i++) {
            q = gGame[0]->track->ncars / n2;
            counts[i] = q;
            if (rem == 1 && n2 - i < 2) {
                counts[i] = q + 1;
            }
            if (rem == 2 && n2 - i < 3) {
                counts[i]++;
            }
        }
        nc = 0;
        if (D_800958A4[1] == 0) {
            start = D_801252F4;
        } else {
            for (i = 0; i < D_80095CB8[D_800958A4[3]]; i++) {
                if (D_80095CA8[D_800958A4[3]][i].x0 == D_800958A4[4]) {
                    start = i;
                    break;
                }
            }
        }
        gap = 2;
        for (i = 0; i < n2; i++) {
            if (D_800958A4[1] != 3 && gGame[0]->x10FC != 25) {
                t = D_80095CA8[D_800958A4[3]][start].x4;
                if (t == 1) {
                    sign = (i == 0) ? -1 : 1;
                    unk = -1;
                } else if (t == 0) {
                    sign = (i != 0) ? -1 : 1;
                    unk = 1;
                } else if (t == 2) {
                    sign = (i != 0) ? -1 : 1;
                    unk = 1;
                } else if (t == 3) {
                    sign = (i == 0) ? -1 : 1;
                    unk = -1;
                }
            } else {
                sign = (i != 0) ? -1 : 1;
                unk = 1;
            }
            for (j = 0; j < counts[i]; j++) {
                f = (j & 1) ? 0.3f : 0.7f;
                car = gGame[0]->track->cars[nc];
                if ((D_800958A4[1] == 3) & (i == 0)) {
                    if (nc > 0) {
                        car = gGame[0]->track->cars[nc + 1];
                    }
                }
                if ((D_800958A4[1] == 3) & (i == 1)) {
                    if (j == 0) {
                        car = gGame[0]->track->cars[1];
                    }
                }
                car->dir = sign;
                seg = segs[i] - (counts[i] - j) * gap * sign;
                if (seg < 0) {
                    seg += track->nsegs;
                }
                if (seg >= track->nsegs) {
                    seg -= track->nsegs;
                }
                prev = seg - sign;
                if (prev < 0) {
                    prev = track->nsegs - 1;
                }
                if (prev >= track->nsegs) {
                    prev = 0;
                }
                car->seg = seg;
                p0.x = track->segs[prev].a.x + (track->segs[prev].b.x - track->segs[prev].a.x) * f;
                p0.y = track->segs[prev].a.y + (track->segs[prev].b.y - track->segs[prev].a.y) * f + 1.0;
                p0.z = track->segs[prev].a.z + (track->segs[prev].b.z - track->segs[prev].a.z) * f;
                p1.x = track->segs[seg].a.x + (track->segs[seg].b.x - track->segs[seg].a.x) * f;
                p1.y = track->segs[seg].a.y + (track->segs[seg].b.y - track->segs[seg].a.y) * f + 1.0;
                p1.z = track->segs[seg].a.z + (track->segs[seg].b.z - track->segs[seg].a.z) * f;
                nc++;
                len = Vec3f_DistXZ(p0, p1);
                p0.x = p0.x + (p1.x - p0.x) / len * 0.5;
                p0.y = p0.y + (p1.y - p0.y) / len * 0.5;
                p0.z = p0.z + (p1.z - p0.z) / len * 0.5;
                car->pos = p0;
                car->pos2 = p0;
                dir.x = p1.x - p0.x;
                dir.y = p1.y - p0.y;
                dir.z = p1.z - p0.z;
                car->basis = dir;
                Basis_Orthonormalize(&car->basis);
                car->basis2 = dir;
                Basis_Orthonormalize(&car->basis2);
                k = Race_FindCarSlot(car->id);
                D_800D1B08[k].x = p1.x;
                D_800D1B08[k].y = p1.y;
                if (unk > 0) {
                    D_800D1B08[k].z = p1.z;
                } else {
                    D_800D1B08[k].z = p1.z;
                }
                if (gGame[0]->track->ncars == nc) {
                    i = n2;
                    j = counts[i];
                }
            }
        }
    }
}
