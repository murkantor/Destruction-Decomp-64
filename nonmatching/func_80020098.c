typedef struct { f32 x, y, z; } Vec3f;
typedef struct { Vec3f pos; u8 pad[0xC]; } Node;
typedef struct { u8 pad[0x30]; Node *x30; } Race;
typedef struct { u8 pad[0x1100]; Race *x1100; } Game0;
typedef struct { u8 pad[0x10FC]; s16 x10FC; u8 pad2[0x1100 - 0x10FE]; Race *x1100; } Game;
typedef struct { u8 pad[0xB4]; u8 xB4; } Model;
typedef struct { u8 pad[0xC]; Model *xC; s8 x10; } Driver;
typedef struct { u8 pad[0x28]; s32 x28; u8 pad2[4]; s32 x30; u8 pad3[8]; Driver *x3C; u8 x40; } Obj;
extern Game *gGame[];
extern void *gUi;
extern f32 D_80091B18[];
extern u8 D_80091B14[];
extern s32 D_800958A8;
extern f32 Vec3f_DistXZ(Vec3f, Vec3f);
extern void Ui_AddElement(void *, s32, s32);
extern void Msg_Queue(s32, s32, s32, s32);
extern void func_80008BB8(s32 *, s32, s32, s32, s32);

void func_80020098(Obj *o, s32 idx) {
    Race *race = gGame[0]->x1100;
    s32 msg[2];
    s16 mode;

    if (o->x30 == 0) {
        D_80091B18[o->x3C->x10] += Vec3f_DistXZ(race->x30[o->x28].pos, race->x30[idx].pos);
        mode = gGame[0]->x10FC;
        if (mode == 28) {
            if (D_80091B18[o->x3C->x10] > 25.0 && !D_80091B14[o->x3C->x10]) {
                if (o->x3C->xC->xB4 == 7) {
                    return;
                }
                msg[0] = 0x86;
                msg[1] = 0x87;
                if (gGame[0]->x10FC != 25 && !D_80091B14[o->x3C->x10]) {
                    D_80091B14[o->x3C->x10] = 1;
                    Ui_AddElement(gUi, 2, o->x3C->x10 + 0xE2);
                    if (gGame[0]->x10FC == mode) {
                        if (D_800958A8 == 2) {
                            Msg_Queue(0x87, 0, 1, 0);
                        } else {
                            func_80008BB8(msg, 2, 0, 1, 0);
                        }
                    }
                }
            }
        }
        if (o->x40) {
            o->x40 = 0;
        }
    }
    o->x28 = idx;
}
