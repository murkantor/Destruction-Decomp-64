/* Big-endian marks on game memory: see port/src/shadow.c. */
#ifndef PORT_SHADOW_H
#define PORT_SHADOW_H

#include "ultra64.h"

#ifdef __cplusplus
extern "C" {
#endif

void Port_MarkBE(const void *p, u32 len);
void Port_MarkNative(const void *p, u32 len);
int Port_IsBE(const void *p);
u16 Port_Rd16(const void *p);
u32 Port_Rd32(const void *p);

#ifdef __cplusplus
}
#endif

#endif
