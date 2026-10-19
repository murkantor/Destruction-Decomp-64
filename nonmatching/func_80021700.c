typedef struct { f32 x, y, z; } Vec3f;
typedef struct { u8 pad[0x2C4]; f32 x2C4; } Car;
typedef struct { u8 pad[0x10]; s8 x10; u8 pad2[0xF]; Car *x20; } CamCfg;
typedef struct { u8 pad[0x68]; Vec3f x68; u8 pad2[0xB0 - 0x74]; CamCfg *xB0; u8 xB4; } Cam;
extern u8 D_80095628;
extern f32 D_8009562C;
extern s8 D_800D31CC;
extern f32 D_80095614[];
extern void Vec3f_Set(Vec3f *, f32, f32, f32);
extern void Camera_UpdateCheckpointCam(Cam *, Car *, f32);
extern void Camera_SetChaseFromCar(Cam *, Car *, f32, f32, f32, f32, f32);
extern void Camera_UpdateChase2(Cam *, Car *, f32, f32, f32, f32, f32, f32, f32);
extern void func_8002205C(Cam *, Car *, f32, f32 *);
extern void func_80022A6C(Cam *, Car *, f32);
extern void func_80022E1C(Cam *, f32);
extern void func_80044AB4(Vec3f *, Vec3f *, Vec3f *, f32 *);
extern void Vec3f_LerpOffset(Cam *, Vec3f *, Vec3f *, Vec3f *, f32);
extern void func_80021C20(Cam *, Car *, f32);

void func_80021700(Cam *cam, f32 dt) {
    CamCfg *cfg = cam->xB0;
    Car *car = cfg->x20;
    Vec3f a;
    Vec3f b;
    Vec3f c;
    f32 t;
    f32 h;
    f32 y;
    f32 z;
    f32 lo;
    f32 w;
    f32 p, q, r;
    f32 k;

    if (D_80095628) {
        Camera_UpdateCheckpointCam(cam, car, dt);
    }
    Vec3f_Set(&cam->x68, 0.0f, 1.0f, 0.0f);
    switch (cam->xB4) {
    case 0:
        y = 1.1f;
        h = 0.0f;
        w = 10.0f;
        lo = 0.0f;
        if (car->x2C4 < 0.0) {
            h = car->x2C4;
            y += h;
        }
        Camera_SetChaseFromCar(cam, car, lo, lo, y, w, h);
        break;
    case 1:
        Camera_UpdateChase2(cam, car, 0.0f, 6.0f, 2.0f, 0.0f, 2.0f, 5.0f, dt);
        break;
    case 2:
        z = 8.0f;
        p = 2.5f;
        q = 2.0f;
        r = 5.0f;
        k = 0.0f;
        if (D_800D31CC >= 3) {
            z = 12.0f;
        }
        Camera_UpdateChase2(cam, car, k, z, p, k, q, r, dt);
        break;
    case 7:
        func_8002205C(cam, car, dt, &D_80095614[cfg->x10]);
        break;
    case 3:
        func_80022A6C(cam, car, dt);
        break;
    case 4:
    case 5:
        func_80022E1C(cam, dt);
        break;
    case 6:
        func_80044AB4(&a, &b, &c, &t);
        Vec3f_LerpOffset(cam, &a, &b, &c, t);
        break;
    case 8:
        break;
    case 9:
        if (D_80095628) {
            func_80021C20(cam, car, D_8009562C);
        }
        break;
    }
}
/* agent45: only case 0 differs (one insn): original materialises 0.0f in f0, h = mov f0, lo = mov h (in bc1f delay). Tried orders, lo = h, h = lo, chained, literal 0.0f args. */
