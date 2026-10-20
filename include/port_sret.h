/*
 * Functions that return a struct by value, for callers written the MIPS way.
 *
 * On MIPS the hidden result pointer goes in a0 like a first argument, so m2c
 * output often declares `Vec3f f(Car *)` as `void f(Vec3f *out, Car *)`; both
 * work on the N64. On x86 they do not: 32-bit Linux has the callee pop the
 * hidden pointer, and MinGW returns small structs in registers. A caller
 * declaring a function the MIPS way includes this header on PC instead: the
 * real prototype, and a macro of the same name turning `f(out, args)` into a
 * call that stores the whole result through out, as the N64 does.
 *
 * tools/port/check_protos.py reports callers that need this.
 */
#ifndef PORT_SRET_H
#define PORT_SRET_H

#ifdef TARGET_PC

#include <string.h>

typedef struct { u8 b[12]; } PortSRet12;
typedef struct { u8 b[24]; } PortSRet24;
typedef struct { u8 b[0xC0]; } PortSRetFbContext;

#define PORT_SRET(T, f, out, ...)            \
    do {                                     \
        T port_sret_ = f(__VA_ARGS__);       \
        memcpy((out), &port_sret_, sizeof(T)); \
    } while (0)

PortSRet12 Car_GetPos(void *obj);
PortSRet12 Car_GetVelocity(void *obj);
PortSRet12 func_8001FC08(void *obj);
PortSRet24 Contact_ComputeImpulse(void *car, void *contact);
PortSRetFbContext Fb_CreateContext(void);
PortSRet12 Mtx3_MulDiff(void *m, void *a, void *b);
PortSRet12 Mtx3_MulVec3fTransposed(void *m, void *v);
PortSRet12 Tensor_MulVec3f(void *t, void *v);
PortSRet12 Ui_GetElementPos(void *ui, s32 kind, s32 id);
PortSRet12 Vec3f_TransformTransposed(void *m, void *t, void *v);
PortSRet12 Wheel_ComputeTireForce(void *car, void *wheel, f32 load, void *normal, void *out, s32 idx);

#define Car_GetPos(out, ...) PORT_SRET(PortSRet12, Car_GetPos, out, __VA_ARGS__)
#define Car_GetVelocity(out, ...) PORT_SRET(PortSRet12, Car_GetVelocity, out, __VA_ARGS__)
#define func_8001FC08(out, ...) PORT_SRET(PortSRet12, func_8001FC08, out, __VA_ARGS__)
#define Contact_ComputeImpulse(out, ...) PORT_SRET(PortSRet24, Contact_ComputeImpulse, out, __VA_ARGS__)
#define Fb_CreateContext(out)                                    \
    do {                                                         \
        PortSRetFbContext port_sret_ = Fb_CreateContext();       \
        memcpy((out), &port_sret_, sizeof(port_sret_));          \
    } while (0)
#define Mtx3_MulDiff(out, ...) PORT_SRET(PortSRet12, Mtx3_MulDiff, out, __VA_ARGS__)
#define Mtx3_MulVec3fTransposed(out, ...) PORT_SRET(PortSRet12, Mtx3_MulVec3fTransposed, out, __VA_ARGS__)
#define Tensor_MulVec3f(out, ...) PORT_SRET(PortSRet12, Tensor_MulVec3f, out, __VA_ARGS__)
#define Ui_GetElementPos(out, ...) PORT_SRET(PortSRet12, Ui_GetElementPos, out, __VA_ARGS__)
#define Vec3f_TransformTransposed(out, ...) PORT_SRET(PortSRet12, Vec3f_TransformTransposed, out, __VA_ARGS__)
#define Wheel_ComputeTireForce(out, ...) PORT_SRET(PortSRet12, Wheel_ComputeTireForce, out, __VA_ARGS__)

#endif

#endif
