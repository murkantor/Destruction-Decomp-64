/* near: regs match with the while-loop form; original has an extra "j" after the id==0 test and the k-loop "counts.v[k]=1; break" block placed there (loop.c block move). 8 bytes short. */
typedef struct { f32 x, y, z; } Vec3f;
typedef struct {
    u8 pad0[0x1A8];
    s32 x1A8;
    u8 x1AC;
    u8 pad1AD[3];
    f32 x1B0;
    s32 x1B4;
    u8 pad1B8[4];
} Wheel;
typedef struct {
    u8 pad0[0x364];
    Wheel wheels[4];
    u8 pad[0xB70 - 0x364 - 0x1BC * 4];
    s32 xB70;
} Car;
typedef struct { Car *car; u8 pad[0x64]; } PlayerSlot;
typedef struct { s16 id; s32 type; } SndSlot;
typedef struct { s32 v[4]; } Counts;
typedef struct {
    u8 pad[0x10];
    u16 x10;
    s32 x14;
    Vec3f x18;
    Vec3f x24;
} Ent;

extern s8 D_800D31CC;
extern Counts D_800B7F8C;
extern PlayerSlot D_800958F0[];
extern SndSlot D_8007EDD8[];
extern s32 D_801320C8[];
void func_8001FB88(Vec3f *, Car *);
void func_8001FBA8(Vec3f *, Car *);
void List_RemoveById(void *, s16);
Ent *func_8005CC00(void *, s16);
s16 Sound_PlayCar(void *pool, Vec3f *a, Vec3f *b, s32 type, f32 vol, s32 mode);

void Sound_UpdateCarSurfaces(void *pool)
{
    Wheel *w[4];
    Counts counts = D_800B7F8C;
    Vec3f vel;
    Vec3f pos;
    s32 i, j, k, n;
    Car *car;
    u8 any;
    s32 type;
    f32 vol;
    s32 mode;
    s32 best;
    Ent *e;

    for (i = 0; i < D_800D31CC; i++) {
        car = D_800958F0[i].car;
        if (car->xB70 == 0) {
            continue;
        }
        any = 0;
        for (j = 0; j < 4; j++) {
            w[j] = &car->wheels[j];
            any |= w[j]->x1AC;
        }
        if (any) {
            for (k = 0; k < 4; k++) {
                if (w[k]->x1AC) {
                    counts.v[k] = 1;
                    break;
                }
            }
            for (j = k + 1; j < 4; j++) {
                for (n = 0; n < 4; n++) {
                    if (w[n]->x1AC && w[n]->x1A8 == w[j]->x1A8 && j != n) {
                        counts.v[n]++;
                        break;
                    }
                }
                if (n == 4) {
                    counts.v[j]++;
                }
            }
            best = counts.v[k];
            type = w[k]->x1A8;
            vol = w[k]->x1B0;
            mode = w[k]->x1B4;
            for (j = k + 1; j < 4; j++) {
                if (best < counts.v[j]) {
                    type = w[j]->x1A8;
                    vol = w[j]->x1B0;
                    best = counts.v[j];
                    mode = w[j]->x1B4;
                } else if (best == counts.v[j] && vol < w[j]->x1B0) {
                    type = w[j]->x1A8;
                    vol = w[j]->x1B0;
                    mode = w[j]->x1B4;
                }
            }
            func_8001FB88(&vel, car);
            func_8001FBA8(&pos, car);
            while (D_8007EDD8[i].id != 0) {
                if (D_8007EDD8[i].type == type) goto update;
                List_RemoveById(pool, D_8007EDD8[i].id);
                break;
            }
            D_8007EDD8[i].id = Sound_PlayCar(pool, &pos, &vel, type, vol, mode); D_8007EDD8[i].type = type;
            goto done;
        update:
            e = func_8005CC00(pool, D_8007EDD8[i].id); if (e != NULL) { e->x18 = pos; e->x24 = vel; e->x14 = D_801320C8[e->x10] * vol; }
        done:;
        }
        if (!any) {
            if (D_8007EDD8[i].id != 0) {
                List_RemoveById(pool, D_8007EDD8[i].id);
                D_8007EDD8[i].id = 0;
                D_8007EDD8[i].type = 0;
            }
        }
    }
}
