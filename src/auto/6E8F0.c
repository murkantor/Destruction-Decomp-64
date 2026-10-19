#include "common.h"
#include "m2c.h"

typedef struct OSPiHandle_s {
    struct OSPiHandle_s *next;
    u8 type;
    u8 latency;
    u8 pageSize;
    u8 relDuration;
    u8 pulse;
    u8 domain;
    u32 baseAddress;
    u32 speed;
    u8 transferInfo[0x60];
} OSPiHandle;

extern OSPiHandle __Dom1SpeedParam;
extern OSPiHandle __Dom2SpeedParam;

void func_8006DCF0(void) {
    __Dom1SpeedParam.domain = 0;
    __Dom2SpeedParam.domain = 1;
    __Dom1SpeedParam.latency = *(vu32 *)0xA4600014;
    __Dom1SpeedParam.pulse = *(vu32 *)0xA4600018;
    __Dom1SpeedParam.pageSize = *(vu32 *)0xA460001C;
    __Dom1SpeedParam.relDuration = *(vu32 *)0xA4600020;
    __Dom2SpeedParam.latency = *(vu32 *)0xA4600024;
    __Dom2SpeedParam.pulse = *(vu32 *)0xA4600028;
    __Dom2SpeedParam.pageSize = *(vu32 *)0xA460002C;
    __Dom2SpeedParam.relDuration = *(vu32 *)0xA4600030;
}
