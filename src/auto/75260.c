#include "common.h"
#include "m2c.h"

typedef struct {
    u32 type;
    u32 flags;
    u64 *ucode_boot;
    u32 ucode_boot_size;
    u64 *ucode;
    u32 ucode_size;
    u64 *ucode_data;
    u32 ucode_data_size;
    u64 *dram_stack;
    u32 dram_stack_size;
    u64 *output_buff;
    u64 *output_buff_size;
    u64 *data_ptr;
    u32 data_size;
    u64 *yield_data_ptr;
    u32 yield_data_size;
} OSTask_t;
typedef union {
    OSTask_t t;
    long long force_structure_alignment;
} OSTask;
extern OSTask tmp_task;
void bcopy(const void *, void *, s32);
u32 osVirtualToPhysical(void *);
void osWritebackDCache(void *, s32);
void __osSpSetStatus(u32);
s32 __osSpSetPc(u32);
s32 __osSpRawStartDma(s32, u32, void *, u32);
s32 __osSpDeviceBusy(void);

#define _osVirtualToPhysical(ptr) \
    if (ptr != NULL) { \
        ptr = (void *)osVirtualToPhysical(ptr); \
    }

static inline OSTask *_VirtualToPhysicalTask(OSTask *intp) {
    OSTask *tp = &tmp_task;

    bcopy(intp, tp, sizeof(OSTask));
    _osVirtualToPhysical(tp->t.ucode);
    _osVirtualToPhysical(tp->t.ucode_data);
    _osVirtualToPhysical(tp->t.dram_stack);
    _osVirtualToPhysical(tp->t.output_buff);
    _osVirtualToPhysical(tp->t.output_buff_size);
    _osVirtualToPhysical(tp->t.data_ptr);
    _osVirtualToPhysical(tp->t.yield_data_ptr);
    return tp;
}

void osSpTaskLoad(OSTask *intp) {
    OSTask *tp = _VirtualToPhysicalTask(intp);

    if (tp->t.flags & 1) {
        tp->t.ucode_data = tp->t.yield_data_ptr;
        tp->t.ucode_data_size = tp->t.yield_data_size;
        intp->t.flags &= ~1;
        if (tp->t.flags & 4) {
            tp->t.ucode = (u64 *)*(vu32 *)(((u32)intp->t.yield_data_ptr + 0xC00 - 4) | 0xA0000000);
        }
    }
    osWritebackDCache(tp, sizeof(OSTask));
    __osSpSetStatus(0x2B00);
    while (__osSpSetPc(0x04001000) == -1) {
    }
    while (__osSpRawStartDma(1, 0x04001000 - sizeof(*tp), tp, sizeof(OSTask)) == -1) {
    }
    while (__osSpDeviceBusy()) {
    }
    while (__osSpRawStartDma(1, 0x04001000, tp->t.ucode_boot, tp->t.ucode_boot_size) == -1) {
    }
}
